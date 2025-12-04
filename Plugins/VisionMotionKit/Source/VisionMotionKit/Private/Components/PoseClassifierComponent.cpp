// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PoseClassifierComponent.h"
#include "GameFramework/Actor.h"
#include "Utilities/VisionMotionMathLibraries.h"

UPoseClassifierComponent::UPoseClassifierComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UPoseClassifierComponent::BeginPlay()
{
    Super::BeginPlay();

    // Receiver 자동 탐색(같은 Actor에 있으면)
    if (!Receiver)
    {
        if (AActor* Owner = GetOwner())
        {
            Receiver = Owner->FindComponentByClass<UPoseUdpReceiverComponent>();
        }
    }
}

void UPoseClassifierComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    WindowBuffer.Reset();
    Super::EndPlay(EndPlayReason);
}

void UPoseClassifierComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    IngestLatestFromReceiver();

    // 리시버 타임스탬프 기준으로 오래된 스냅샷 제거
    PruneOld(NowMsFromReceiver());
    Detect();
}

void UPoseClassifierComponent::IngestLatestFromReceiver()
{
    if (!Receiver) return;

    const uint64 Ts = Receiver->GetLatestTimestamp();
    if (Ts == 0 || Ts == LastIngestedTs) return; // 중복 회피

    const TArray<FPersonPose>& LatestPoses = Receiver->GetLatestPoses();

    FTimedPoseSnapshot Snap;
    Snap.TimestampMs = Ts;
    Snap.Poses = LatestPoses;             // 값 복사

    // v2: 손 데이터도 함께 스냅샷에 저장 (v1이면 빈 배열)
    if (Receiver->GetLatestHands().Num() > 0)
    {
        const TArray<FHandPose>& LatestHands = Receiver->GetLatestHands();
        Snap.Hands = LatestHands;              // 값 복사
    }

    WindowBuffer.Add(MoveTemp(Snap));
    LastIngestedTs = Ts;

    OnPoseWindowUpdated.Broadcast();
}

void UPoseClassifierComponent::PruneOld(uint64 NowMs)
{
    if (WindowSeconds <= 0.f) { WindowBuffer.Reset(); return; }

    const uint64 SpanMs = static_cast<uint64>(WindowSeconds * 1000.0f);
    const uint64 Cutoff = (NowMs > SpanMs) ? (NowMs - SpanMs) : 0;

    int32 FirstValidIdx = 0;
    while (FirstValidIdx < WindowBuffer.Num() && WindowBuffer[FirstValidIdx].TimestampMs < Cutoff)
    {
        ++FirstValidIdx;
    }
    if (FirstValidIdx > 0)
    {
        WindowBuffer.RemoveAt(0, FirstValidIdx);
    }
}

uint64 UPoseClassifierComponent::NowMsFromReceiver() const
{
    // 수신 타임스탬프(송신 기준 ms)를 시간 기준으로 사용
    if (Receiver)
    {
        return Receiver->GetLatestTimestamp();
    }
    return 0;
}

int32 UPoseClassifierComponent::GetWindowTotalPersons() const
{
    int32 Sum = 0;
    for (const auto& Snap : WindowBuffer)
    {
        Sum += Snap.Poses.Num();
    }
    return Sum;
}

bool UPoseClassifierComponent::GetLatestJoint(int32 CocoIndex, FVector2D& OutXY) const
{
    if (WindowBuffer.Num() == 0) return false;

    const FTimedPoseSnapshot& Last = WindowBuffer.Last();
    if (Last.Poses.Num() == 0) return false;

    const FPersonPose& P = Last.Poses[0]; // 최신 스냅샷의 첫 사람
    if (!P.XY.IsValidIndex(CocoIndex)) return false;

    const FVector2f& V = P.XY[CocoIndex];
    if (!FMath::IsFinite(V.X) || !FMath::IsFinite(V.Y)) return false;

    OutXY = FVector2D(V.X, V.Y);
    return true;
}

int32 UPoseClassifierComponent::GetDominantPersonId() const
{
    TMap<int32, int32> Count;
    for (const auto& Snap : WindowBuffer)
    {
        for (const auto& PP : Snap.Poses)
        {
            Count.FindOrAdd((int32)PP.PersonId)++;
        }
    }
    int32 BestId = -1;
    int32 BestCnt = -1;
    for (const auto& It : Count)
    {
        if (It.Value > BestCnt)
        {
            BestCnt = It.Value;
            BestId = It.Key;
        }
    }
    return BestId;
}

bool UPoseClassifierComponent::PickBestPerson(const TArray<FPersonPose>& Poses, int32& OutIdx) const
{
    float BestDist = TNumericLimits<float>::Max();
    int32 Best = INDEX_NONE;

    for (int32 i = 0; i < Poses.Num(); ++i)
    {
        const auto& P = Poses[i];
        if (P.XY.Num() < 17) continue;

        // ⬇️ 최소 어깨폭 필터
        float ShoulderW = 0.f;
        if (!ComputeShoulderWidthPx(P, ShoulderW)) continue;

        const FVector2f& LWR = P.XY[9];
        const FVector2f& RWR = P.XY[10];
        const FVector2f& LSH = P.XY[5];
        const FVector2f& RSH = P.XY[6];

        if (!UVisionMotionMathLibraries::IsFinite2D(LWR) || !UVisionMotionMathLibraries::IsFinite2D(RWR) ||
            !UVisionMotionMathLibraries::IsFinite2D(LSH) || !UVisionMotionMathLibraries::IsFinite2D(RSH)) continue;

        const float Dist = FVector2f::Distance(LWR, RWR);
        if (Dist < BestDist)
        {
            BestDist = Dist;
            Best = i;
        }
    }
    OutIdx = Best;
    return Best != INDEX_NONE;
}

bool UPoseClassifierComponent::GetShoulderW(const FPersonPose& P, float& OutShoulderW) const
{
    float ShoulderW = 0.f;
    if (!ComputeShoulderWidthPx(P, ShoulderW)) return false;

    OutShoulderW = ShoulderW;
    return true;
}

bool UPoseClassifierComponent::GetCameraView(FVector& OutCamLoc, FVector& OutCamForward) const
{
    OutCamLoc = FVector::ZeroVector;
    OutCamForward = FVector::ForwardVector;

    if (!GetWorld()) return false;

    // 우선: 오너가 플레이어가 조종 중이면 그 컨트롤러의 뷰포인트
    if (AActor* Owner = GetOwner())
    {
        if (APawn* Pawn = Cast<APawn>(Owner))
        {
            if (AController* Ctrl = Pawn->GetController())
            {
                FVector Loc; FRotator Rot;
                Ctrl->GetPlayerViewPoint(Loc, Rot);
                OutCamLoc = Loc;
                OutCamForward = Rot.Vector().GetSafeNormal();
                return true;
            }
        }
    }

    // 폴백: 첫 번째 플레이어 카메라
    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        FVector Loc; FRotator Rot;
        PC->GetPlayerViewPoint(Loc, Rot);
        OutCamLoc = Loc;
        OutCamForward = Rot.Vector().GetSafeNormal();
        return true;
    }
    return false;
}

void UPoseClassifierComponent::Detect()
{
}

bool UPoseClassifierComponent::ComputeShoulderWidthPx(const FPersonPose& P, float& OutShoulderW) const
{
    if (P.XY.Num() < 11) return false; // 0~10까지 최소 보장
    const FVector2f& LSH = P.XY[5];
    const FVector2f& RSH = P.XY[6];

    if (!FMath::IsFinite(LSH.X) || !FMath::IsFinite(RSH.X)) return false;

    OutShoulderW = FMath::Abs(RSH.X - LSH.X);
    return OutShoulderW > KINDA_SMALL_NUMBER;
}

bool UPoseClassifierComponent::FindRun(const TArray<double>& SpeedNorm, int StartIdx, double Thr, double HoldSec, bool bAbove, int& OutIdx) const
{
    double acc = 0.0;
    for (int i = StartIdx; i < SpeedNorm.Num(); ++i)
    {
        const bool pass = bAbove ? (SpeedNorm[i] >= Thr) : (SpeedNorm[i] <= Thr);
        const double dt = (i > 0) ? KFixedDt : 0.0;
        if (pass) { acc += dt; if (acc >= HoldSec) { OutIdx = i; return true; } }
        else acc = 0.0;
    }
    return false;
}