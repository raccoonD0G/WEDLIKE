// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SwingClassifierComponent.h"
#include <Utilities/VisionMotionMathLibraries.h>

void USwingClassifierComponent::Detect()
{
    Super::Detect();
    if (WindowBuffer.Num() == 0) return;

    const uint64 SenderNow = NowMsFromReceiver();
    if (SenderNow == 0) return;

    const uint64 LocalNow = UVisionMotionMathLibraries::LocalNowMs();
    if (IsCooldownActive(LocalNow)) return;

    // 최근 구간 계산
    constexpr double kMinUseSec = 0.2;
    const double UseSec = FMath::Clamp((double)SingleSwingRecentSeconds, kMinUseSec, (double)WindowSeconds);

    // 스냅 수집
    TArray<const FTimedPoseSnapshot*> Snaps;
    if (!CollectRecentSnaps(SenderNow, UseSec, Snaps)) return;

    // 샘플 빌드
    TArray<FJointSample> Samples;
    if (!BuildSample(Snaps, Samples)) return;

    // 리샘플 + 스무딩
    TArray<FJointSample> U;
    if (!ResampleAndSmooth(Samples, U)) return;

    // (1) 최소 변위(px)
    if (!PassesMinDisplacementPx(U)) return;

    // (2) 속도 메트릭
    const FJointMetrics JointMetrics = CalcMetricsPx(U);

    const bool bPassSpeed = ((JointMetrics.Avg >= MinAvgSpeedPx) || (JointMetrics.Peak >= MinPeakSpeedPx));
    if (!bPassSpeed) return;

    // 스냅별 대표 인물 인덱스
    TArray<int32> PersonIdxOf;
    BuildPersonIndexTimeline(Snaps, PersonIdxOf);

    // (3) 속도 시퀀스
    TArray<double> SpeedPx;
    if (!BuildSpeedPx(U, SpeedPx)) return;

    // (4) 진입
    int32 iEnter = -1;
    if (!FindRun(SpeedPx, /*StartIdx*/ 1, EnterSpeedPx, HoldFast, /*above*/ true, iEnter))
        return;

    // (5) 종료 후보
    int32 iExitLow = -1;
    FindRun(SpeedPx, FMath::Max(1, iEnter + 1), ExitSpeedPx, HoldStill, /*above*/ false, iExitLow);

    int32 iBreak = -1;
    FindDirectionBreakIndexPx(U, /*StartIdx=*/ iEnter + 1, iBreak);

    // (6) 종료 선택
    int32 iExit = -1;
    if (iExitLow >= 0 && iBreak >= 0) {
        iExit = FMath::Min(iExitLow, iBreak);
    }
    else if (iExitLow >= 0) {
        iExit = iExitLow;
    }
    else if (iBreak >= 0) {
        iExit = iBreak;
    }
    else {
        return;
    }

    // (7) 경계 보정
    const int32 iEnterAfter = FMath::Clamp(iEnter + 1, 0, U.Num() - 1);
    const int32 iExitAfter = FMath::Clamp(iExit + 1, 0, U.Num() - 1);

    // (U -> 스냅 인덱스 매핑)
    int32 EnterSnapIdx = -1, ExitSnapIdx = -1;
    if (!MapWindowToSnaps(Snaps, U, iEnterAfter, iExitAfter, EnterSnapIdx, ExitSnapIdx)) return;

    // (8) 브로드캐스트 & 정리
    BroadcastAndFinalize(Snaps, PersonIdxOf, EnterSnapIdx, ExitSnapIdx);

    LastSingleSwingLoggedMs = LocalNow;
    WindowBuffer.Reset();
}

FVector2f USwingClassifierComponent::GetJointPos(const FTimedPoseSnapshot* Snap)
{
    Super::GetJointPos(Snap);

    return GetJointPoseFromEnum(Snap, TargetJoint);
}

bool USwingClassifierComponent::IsCooldownActive(uint64 LocalNow) const
{
    if (LastSingleSwingLoggedMs == 0) return false;
    constexpr double kMsPerSec = 1000.0;
    const uint64 CoolMs = (uint64)(SingleSwingCooldownSeconds * kMsPerSec);
    const uint64 Delta = (LocalNow >= LastSingleSwingLoggedMs) ? (LocalNow - LastSingleSwingLoggedMs) : 0;
    return (Delta < CoolMs);
}

bool USwingClassifierComponent::CollectRecentSnaps(uint64 SenderNow, double UseSec, TArray<const FTimedPoseSnapshot*>& OutSnaps) const
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

bool USwingClassifierComponent::PassesMinDisplacementPx(const TArray<FJointSample>& U) const
{
    if (U.Num() < 2) return false;

    // 최대 편차 기반(왕복 스윙 대응)
    const FVector2f Start = U[0].Pos;
    double maxDev = 0.0;
    for (int i = 1; i < U.Num(); ++i)
    {
        maxDev = FMath::Max(maxDev, (U[i].Pos - Start).Size());
    }

    if (maxDev >= MinDisplacementPx)
        return true;

    // (선택) 누적 경로 길이로 보조 판정
    double path = 0.0;
    for (int i = 1; i < U.Num(); ++i)
    {
        path += (U[i].Pos - U[i - 1].Pos).Size();
    }

    return (path >= MinDisplacementPx * 1.8);
}

bool USwingClassifierComponent::BuildPersonIndexTimeline(const TArray<const FTimedPoseSnapshot*>& Snaps, TArray<int32>& OutPersonIdxOf)
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

bool USwingClassifierComponent::BuildSpeedPx(const TArray<FJointSample>& U, TArray<double>& OutSpeedPx) const
{
    if (U.Num() < 2) return false;
    OutSpeedPx.Init(0.0, U.Num());
    for (int i = 1; i < U.Num(); ++i)
    {
        const FVector2f d = U[i].Pos - U[i - 1].Pos;
        OutSpeedPx[i] = (d.Size() / KFixedDt); // px/sec
    }
    return true;
}

bool USwingClassifierComponent::FindDirectionBreakIndexPx(const TArray<FJointSample>& U, int StartIdx, int& OutBreakIdx) const
{
    OutBreakIdx = -1;
    if (U.Num() < 2 || StartIdx < 1 || StartIdx >= U.Num() - 1) return false;

    const double CosThr = FMath::Cos(FMath::DegreesToRadians((double)DirectionBreakAngleDeg));
    const int HoldFrames = FMath::Max(1, (int)FMath::RoundToInt(DirectionBreakHoldSec / KFixedDt));

    // 1) 기준 방향: StartIdx ~ StartIdx+4 사이 유효 이동 평균
    FVector2f RefSum(0, 0);
    int RefCnt = 0;
    const int refEnd = FMath::Min(StartIdx + 4, U.Num() - 1);
    for (int i = StartIdx; i <= refEnd; ++i)
    {
        const FVector2f D = U[i].Pos - U[i - 1].Pos;
        const double V = D.Size() / KFixedDt; // px/sec
        if (V >= DirectionBreakMinSpeedPx)
        {
            RefSum += D;
            ++RefCnt;
        }
    }
    if (RefCnt == 0) return false;

    const FVector2f RefDir = RefSum.GetSafeNormal();
    if (RefDir.IsNearlyZero()) return false;

    // 2) 급변 탐지
    int ConSec = 0;
    for (int i = StartIdx + 1; i < U.Num(); ++i)
    {
        const FVector2f D = U[i].Pos - U[i - 1].Pos;
        const double V = D.Size() / KFixedDt; // px/sec
        if (V < DirectionBreakMinSpeedPx)
        {
            ConSec = 0;
            continue;
        }

        const FVector2f Dir = D.GetSafeNormal();

        bool bValidDir = !Dir.IsNearlyZero();
        double CS = 1.0;  // 기본값 (동일 방향)

        if (bValidDir)
        {
            double RawCs = FVector2f::DotProduct(Dir, RefDir);
            CS = FMath::Clamp(RawCs, -1.0, 1.0);
        }
        else
        {
            ConSec = 0;
            continue;
        }

        if (CS <= CosThr)
        {
            if (++ConSec >= HoldFrames)
            {
                OutBreakIdx = i;
                return true;
            }
        }
        else
        {
            ConSec = 0;
        }
    }
    return false;
}

bool USwingClassifierComponent::MapWindowToSnaps(const TArray<const FTimedPoseSnapshot*>& Snaps, const TArray<FJointSample>& U, int iEnterAfter, int iExitAfter, int& OutEnterSnapIdx, int& OutExitSnapIdx) const
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

void USwingClassifierComponent::BroadcastAndFinalize(const TArray<const FTimedPoseSnapshot*>& Snaps, const TArray<int32>& PersonIdxOf, int EnterSnapIdx, int ExitSnapIdx)
{
    TArray<FTimedPoseSnapshot> SnapsValue; SnapsValue.Reserve(Snaps.Num());
    for (const FTimedPoseSnapshot* S : Snaps) if (S) SnapsValue.Add(*S);

    OnSwingDetectedDelegate.Broadcast(SnapsValue, PersonIdxOf, EnterSnapIdx, ExitSnapIdx);
}