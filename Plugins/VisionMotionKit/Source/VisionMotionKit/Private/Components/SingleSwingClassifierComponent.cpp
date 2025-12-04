// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SingleSwingClassifierComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Utilities/VisionMotionMathLibraries.h"
#include "Global/VisionMotionUdpStruct.h"
#include "Utilities/VisionMotionUtils.h"

static void CalcDispStats(const TArray<FHandSample>& U, bool bRight,
    double& OutMaxDevPx, double& OutPathLenPx)
{
    OutMaxDevPx = 0.0;
    OutPathLenPx = 0.0;
    if (U.Num() < 2) return;

    const FVector2D Base = bRight ? U[0].R : U[0].L;
    FVector2D Prev = Base;

    for (int32 i = 1; i < U.Num(); ++i)
    {
        const FVector2D Cur = bRight ? U[i].R : U[i].L;

        OutMaxDevPx = FMath::Max(OutMaxDevPx, FVector2D::Distance(Base, Cur));
        OutPathLenPx += FVector2D::Distance(Prev, Cur);

        Prev = Cur;
    }
}

void USingleSwingClassifierComponent::Detect()
{
    Super::Detect();
    if (WindowBuffer.Num() == 0) return;

    FSingleSwingTrace Tr;
    Tr.bEnabled = bLogSingleSwingReasons;

    const uint64 SenderNow = NowMsFromReceiver();
    if (SenderNow == 0) return;
    Tr.SenderNowMs = SenderNow;

    const uint64 LocalNow = UVisionMotionMathLibraries::LocalNowMs();
    Tr.LocalNowMs = LocalNow;
    if (IsCooldownActive(LocalNow)) return;

    // 최근 구간 계산
    constexpr double kMinUseSec = 0.2;
    const double UseSec = FMath::Clamp((double)SingleSwingRecentSeconds, kMinUseSec, (double)WindowSeconds);
    Tr.UseSec = UseSec;

    // 스냅 수집
    TArray<const FTimedPoseSnapshot*> Snaps;
    if (!CollectRecentSnaps(SenderNow, UseSec, Snaps)) return;
    Tr.NumSnaps = Snaps.Num();

    // 샘플 빌드
    TArray<FHandSample> Samples;
    if (!BuildSamples(Snaps, Samples)) return;

    // 리샘플 + 스무딩
    TArray<FHandSample> U;
    if (!ResampleAndSmooth(Samples, U)) return;
    Tr.NumU = U.Num();

    // 현재 손 기준(왼손/오른손)
    const bool bRight = (GetCurrentHandSide() == EHandSide::Right);
    Tr.bRight = bRight;

    // 변위/경로 통계 계산
    CalcDispStats(U, bRight, Tr.MaxDeviationPx, Tr.PathLenPx);

    // (1) 최소 변위(px)
    Tr.MinDispThresholdPx = MinDisplacementPx; // 현재 세팅된 임계치
    Tr.bPassMinDisp = PassesMinDisplacementPx(U);
    if (!Tr.bPassMinDisp) return;

    // (2) 속도 메트릭
    const FHandMetrics ML = CalcMetricsPx(U, /*bLeft=*/true);
    const FHandMetrics MR = CalcMetricsPx(U, /*bLeft=*/false);
    const FHandMetrics& M = bRight ? MR : ML;

    Tr.AvgSpeedPx = M.Avg;
    Tr.PeakSpeedPx = M.Peak;
    Tr.MinAvgSpeedPx = MinAvgSpeedPx;
    Tr.MinPeakSpeedPx = MinPeakSpeedPx;
    // 선택적으로: Tr.PathLenPx = M.PathLen;  // 필드가 있다면

    const bool bPassSpeed = ((M.Avg >= MinAvgSpeedPx) || (M.Peak >= MinPeakSpeedPx));
    if (!bPassSpeed) return;

    // 스냅별 대표 인물 인덱스
    TArray<int32> PersonIdxOf;
    BuildPersonIndexTimeline(Snaps, PersonIdxOf);

    // (3) 속도 시퀀스
    TArray<double> SpeedPx;
    if (!BuildSpeedPx(U, SpeedPx)) return;

    // (4) 진입
    Tr.EnterSpeedPx = EnterSpeedPx;
    Tr.HoldFast = HoldFast;
    int32 iEnter = -1;
    if (!FindRun(SpeedPx, /*StartIdx*/ 1, EnterSpeedPx, HoldFast, /*above*/ true, iEnter))
        return;
    Tr.IEnter = iEnter;

    // (5) 종료 후보
    Tr.ExitSpeedPx = ExitSpeedPx;
    Tr.HoldStill = HoldStill;

    int32 iExitLow = -1;
    FindRun(SpeedPx, FMath::Max(1, iEnter + 1), ExitSpeedPx, HoldStill, /*above*/ false, iExitLow);
    Tr.IExitLow = iExitLow;

    int32 iBreak = -1;
    FindDirectionBreakIndexPx(U, /*StartIdx=*/ iEnter + 1, iBreak);
    Tr.IBreak = iBreak;

    // (6) 종료 선택
    int32 iExit = -1;
    if (iExitLow >= 0 && iBreak >= 0) {
        iExit = FMath::Min(iExitLow, iBreak);
        Tr.ExitCause = (iExit == iExitLow) ? EExitCause::LowSpeed : EExitCause::DirectionBreak;
    }
    else if (iExitLow >= 0) {
        iExit = iExitLow;
        Tr.ExitCause = EExitCause::LowSpeed;
    }
    else if (iBreak >= 0) {
        iExit = iBreak;
        Tr.ExitCause = EExitCause::DirectionBreak;
    }
    else {
        return;
    }
    Tr.IExit = iExit;

    // (7) 경계 보정
    const int32 iEnterAfter = FMath::Clamp(iEnter + 1, 0, U.Num() - 1);
    const int32 iExitAfter = FMath::Clamp(iExit + 1, 0, U.Num() - 1);

    // (U -> 스냅 인덱스 매핑)
    int32 EnterSnapIdx = -1, ExitSnapIdx = -1;
    if (!MapWindowToSnaps(Snaps, U, iEnterAfter, iExitAfter, EnterSnapIdx, ExitSnapIdx)) return;
    Tr.EnterSnapIdx = EnterSnapIdx;
    Tr.ExitSnapIdx = ExitSnapIdx;

    // 사람 인덱스 (있으면 기록)
    if (PersonIdxOf.IsValidIndex(EnterSnapIdx)) Tr.PersonEnter = PersonIdxOf[EnterSnapIdx];
    if (PersonIdxOf.IsValidIndex(ExitSnapIdx))  Tr.PersonExit = PersonIdxOf[ExitSnapIdx];

    // (8) 브로드캐스트 & 정리
    BroadcastAndFinalize(Snaps, PersonIdxOf, EnterSnapIdx, ExitSnapIdx);

    LastSingleSwingLoggedMs = LocalNow;
    WindowBuffer.Reset();
}

bool USingleSwingClassifierComponent::IsCooldownActive(uint64 LocalNow) const
{
    if (LastSingleSwingLoggedMs == 0) return false;
    constexpr double kMsPerSec = 1000.0;
    const uint64 CoolMs = (uint64)(SingleSwingCooldownSeconds * kMsPerSec);
    const uint64 Delta = (LocalNow >= LastSingleSwingLoggedMs) ? (LocalNow - LastSingleSwingLoggedMs) : 0;
    return (Delta < CoolMs);
}

bool USingleSwingClassifierComponent::CollectRecentSnaps(uint64 SenderNow, double UseSec, TArray<const FTimedPoseSnapshot*>& OutSnaps) const
{
    OutSnaps.Reset();
    const uint64 RecentOldest = (SenderNow > (uint64)(UseSec * 1000.0)) ? (SenderNow - (uint64)(UseSec * 1000.0)) : 0;

    OutSnaps.Reserve(WindowBuffer.Num());
    for (const auto& S : WindowBuffer)
    {
        if (S.TimestampMs >= RecentOldest && S.Poses.Num() > 0)
            OutSnaps.Add(&S);
    }
    return (OutSnaps.Num() >= 3);
}

bool USingleSwingClassifierComponent::BuildSamples(const TArray<const FTimedPoseSnapshot*>& Snaps, TArray<FHandSample>& OutSamples)
{
    OutSamples.Reset();
    OutSamples.Reserve(Snaps.Num());

    for (const FTimedPoseSnapshot* S : Snaps)
    {
        int32 PersonIdx = INDEX_NONE;
        if (!PickBestPerson(S->Poses, PersonIdx)) continue;

        const FPersonPose& P = S->Poses[PersonIdx];
        if (P.XY.Num() < 17) continue;

        if (!(P.XY.IsValidIndex(MotionCapture::COCO_LSH) && P.XY.IsValidIndex(MotionCapture::COCO_RSH))) continue;
        const FVector2f& LSH = P.XY[MotionCapture::COCO_LSH];
        const FVector2f& RSH = P.XY[MotionCapture::COCO_RSH];
        if (!UVisionMotionMathLibraries::IsFinite2D(LSH) || !UVisionMotionMathLibraries::IsFinite2D(RSH)) continue;

        const FVector2D C = 0.5 * FVector2D(LSH.X + RSH.X, LSH.Y + RSH.Y);

        const FVector2f& LWR = P.XY[MotionCapture::COCO_LWR];
        const FVector2f& RWR = P.XY[MotionCapture::COCO_RWR];
        if (!UVisionMotionMathLibraries::IsFinite2D(LWR) || !UVisionMotionMathLibraries::IsFinite2D(RWR)) continue;

        float ShoulderW = 0.f;
        GetShoulderW(P, ShoulderW);
        if (ShoulderW <= KINDA_SMALL_NUMBER) continue;

        FVector2D LCenter(LWR.X, LWR.Y);
        FVector2D RCenter(RWR.X, RWR.Y);

        if (S->Hands.Num() > 0)
        {
            const uint16 PersonId = (uint16)P.PersonId;
            FVector2D CandL; if (FindHandFor(S->Hands, PersonId, /*Left*/0, LWR, CandL)) LCenter = CandL;
            FVector2D CandR; if (FindHandFor(S->Hands, PersonId, /*Right*/1, RWR, CandR)) RCenter = CandR;
        }

        FHandSample Sample;
        Sample.T = (double)S->TimestampMs * 0.001;
        Sample.L = LCenter;
        Sample.R = RCenter;
        Sample.C = C;
        Sample.ShoulderW = ShoulderW;

        OutSamples.Add(MoveTemp(Sample));
    }

    return (OutSamples.Num() >= 3);
}

bool USingleSwingClassifierComponent::ResampleAndSmooth(const TArray<FHandSample>& InSamples, TArray<FHandSample>& OutU) const
{
    OutU = ResampleUniform(InSamples);
    if (OutU.Num() < 3) return false;
    ApplyEMA(OutU);
    return true;
}

bool USingleSwingClassifierComponent::ComputeMeanShoulderWidth(const TArray<FHandSample>& U, double& OutMeanSW) const
{
    if (U.Num() == 0) return false;
    double sum = 0.0;
    for (const auto& s : U) sum += (double)FMath::Max(s.ShoulderW, KINDA_SMALL_NUMBER);
    OutMeanSW = sum / (double)U.Num();
    return (OutMeanSW > KINDA_SMALL_NUMBER);
}

bool USingleSwingClassifierComponent::MapWindowToSnaps(const TArray<const FTimedPoseSnapshot*>& Snaps, const TArray<FHandSample>& U, int iEnterAfter, int iExitAfter, int& OutEnterSnapIdx, int& OutExitSnapIdx) const
{
    if (Snaps.Num() == 0) return false;

    TArray<double> SnapTimes; SnapTimes.Reserve(Snaps.Num());
    for (const auto* S : Snaps) SnapTimes.Add((double)S->TimestampMs * 0.001);

    int cur = 0;
    OutEnterSnapIdx = FMath::Clamp(MapUToSnapIndex(SnapTimes, U, iEnterAfter, cur), 0, Snaps.Num() - 1);
    OutExitSnapIdx = FMath::Clamp(MapUToSnapIndex(SnapTimes, U, iExitAfter, cur), 0, Snaps.Num() - 1);
    if (OutExitSnapIdx < OutEnterSnapIdx) OutExitSnapIdx = OutEnterSnapIdx;

    return true;
}

bool USingleSwingClassifierComponent::BuildPersonIndexTimeline(const TArray<const FTimedPoseSnapshot*>& Snaps, TArray<int32>& OutPersonIdxOf)
{
    OutPersonIdxOf.SetNum(Snaps.Num());
    for (int32 i = 0; i < Snaps.Num(); ++i)
    {
        int32 idx = INDEX_NONE;
        PickBestPerson(Snaps[i]->Poses, idx);
        OutPersonIdxOf[i] = idx;
    }
    return true;
}

void USingleSwingClassifierComponent::BroadcastAndFinalize(const TArray<const FTimedPoseSnapshot*>& Snaps, const TArray<int32>& PersonIdxOf, int EnterSnapIdx, int ExitSnapIdx)
{
    TArray<FTimedPoseSnapshot> SnapsValue; SnapsValue.Reserve(Snaps.Num());
    for (const FTimedPoseSnapshot* S : Snaps) if (S) SnapsValue.Add(*S);

    OnSingleSwingDetected.Broadcast(SnapsValue, PersonIdxOf, GetCurrentHandSide(), EnterSnapIdx, ExitSnapIdx);
}

bool USingleSwingClassifierComponent::PassesMinDisplacementPx(const TArray<FHandSample>& U) const
{
    if (U.Num() < 2) return false;

    // 최대 편차 기반(왕복 스윙 대응)
    const FVector2D Start = WristAt(U, 0);
    double maxDev = 0.0;
    for (int i = 1; i < U.Num(); ++i)
        maxDev = FMath::Max(maxDev, (WristAt(U, i) - Start).Size());

    if (maxDev >= MinDisplacementPx)
        return true;

    // (선택) 누적 경로 길이로 보조 판정
    double path = 0.0;
    for (int i = 1; i < U.Num(); ++i)
        path += (WristAt(U, i) - WristAt(U, i - 1)).Size();

    return (path >= MinDisplacementPx * 1.8);
}

bool USingleSwingClassifierComponent::BuildSpeedPx(const TArray<FHandSample>& U, TArray<double>& OutSpeedPx) const
{
    if (U.Num() < 2) return false;
    OutSpeedPx.Init(0.0, U.Num());
    for (int i = 1; i < U.Num(); ++i)
    {
        const FVector2D d = WristAt(U, i) - WristAt(U, i - 1);
        OutSpeedPx[i] = (d.Size() / KFixedDt); // px/sec
    }
    return true;
}

FHandMetrics USingleSwingClassifierComponent::CalcMetricsPx(const TArray<FHandSample>& U, bool bLeft) const
{
    FHandMetrics M{};
    if (U.Num() < 2) return M;

    double sum = 0.0;
    double peak = 0.0;
    int count = 0;

    for (int i = 1; i < U.Num(); ++i) {
        const FVector2D cur = bLeft ? U[i].L : U[i].R;
        const FVector2D prev = bLeft ? U[i - 1].L : U[i - 1].R;
        const double v = (cur - prev).Size() / KFixedDt; // px/sec
        sum += v;
        peak = FMath::Max(peak, v);
        ++count;
    }
    M.Avg = (count > 0) ? (sum / count) : 0.0;
    M.Peak = peak;
    return M;
}

bool USingleSwingClassifierComponent::FindDirectionBreakIndexPx(
    const TArray<FHandSample>& U, int StartIdx, int& OutBreakIdx) const
{
    OutBreakIdx = -1;
    if (U.Num() < 2 || StartIdx < 1 || StartIdx >= U.Num() - 1)
        return false;

    const bool bRightHand = (GetCurrentHandSide() == EHandSide::Right);

    const double cosThr = FMath::Cos(FMath::DegreesToRadians((double)DirectionBreakAngleDeg));
    const int holdFrames = FMath::Max(1, (int)FMath::RoundToInt(DirectionBreakHoldSec / KFixedDt));

    // 1) 기준 방향: StartIdx ~ StartIdx+4 사이 유효 이동 평균
    FVector2D refSum(0, 0);
    int refCnt = 0;
    const int refEnd = FMath::Min(StartIdx + 4, U.Num() - 1);
    for (int i = StartIdx; i <= refEnd; ++i)
    {
        const FVector2D d = (bRightHand ? U[i].R : U[i].L) - (bRightHand ? U[i - 1].R : U[i - 1].L);
        const double v = d.Size() / KFixedDt; // px/sec
        if (v >= DirectionBreakMinSpeedPx) {
            refSum += d;
            ++refCnt;
        }
    }
    if (refCnt == 0) return false;

    const FVector2D refDir = refSum.GetSafeNormal();
    if (refDir.IsNearlyZero()) return false;

    // 2) 급변 탐지
    int consec = 0;
    for (int i = StartIdx + 1; i < U.Num(); ++i)
    {
        const FVector2D d = (bRightHand ? U[i].R : U[i].L) - (bRightHand ? U[i - 1].R : U[i - 1].L);
        const double v = d.Size() / KFixedDt; // px/sec
        if (v < DirectionBreakMinSpeedPx) {
            consec = 0;
            continue;
        }

        const FVector2D dir = d.GetSafeNormal();

        bool bValidDir = !dir.IsNearlyZero();
        double cs = 1.0;  // 기본값 (동일 방향)

        if (bValidDir)
        {
            double rawCs = FVector2D::DotProduct(dir, refDir);
            cs = FMath::Clamp(rawCs, -1.0, 1.0);
        }
        else
        {
            consec = 0;
            continue;
        }

        if (cs <= cosThr) {
            if (++consec >= holdFrames) {
                OutBreakIdx = i;
                return true;
            }
        }
        else {
            consec = 0;
        }
    }
    return false;
}

