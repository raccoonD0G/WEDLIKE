// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/JointPoseClassifierComponent.h"
#include <Utilities/VisionMotionMathLibraries.h>

FVector2f UJointPoseClassifierComponent::GetJointPos(const FTimedPoseSnapshot* Snap)
{
    return FVector2f();
}

FVector2f UJointPoseClassifierComponent::GetJointPoseFromEnum(const FTimedPoseSnapshot* Snap, ECOCOKeypoint Joint)
{
    int32 PersonIdx = INDEX_NONE;
    if (!PickBestPerson(Snap->Poses, PersonIdx)) return FVector2f();

    const FPersonPose& P = Snap->Poses[PersonIdx];
    if (P.XY.Num() < 17) return FVector2f();

    if (!(P.XY.IsValidIndex((int32)Joint))) return FVector2f();

    return P.XY[(int32)Joint];
}

bool UJointPoseClassifierComponent::BuildSample(const TArray<const FTimedPoseSnapshot*>& Snaps, TArray<FJointSample>& OutSamples)
{
    OutSamples.Reset();
    OutSamples.Reserve(Snaps.Num());

    for (const FTimedPoseSnapshot* S : Snaps)
    {
        const FVector2f& JointPos = GetJointPos(S);
        if (!UVisionMotionMathLibraries::IsFinite2D(JointPos)) continue;

        FJointSample Sample;
        Sample.Time = (double)S->TimestampMs * 0.001;
        Sample.Pos = JointPos;

        OutSamples.Add(MoveTemp(Sample));
    }

    return (OutSamples.Num() >= 3);
}

FJointMetrics UJointPoseClassifierComponent::CalcMetricsPx(const TArray<FJointSample>& U) const
{
    FJointMetrics M{};
    if (U.Num() < 2) return M;

    double Sum = 0.0;
    double Peak = 0.0;
    int32 Count = 0;

    for (int i = 1; i < U.Num(); ++i) {
        const FVector2f Cur = U[i].Pos;
        const FVector2f Prev = U[i - 1].Pos;
        const double V = (Cur - Prev).Size() / KFixedDt; // px/sec
        Sum += V;
        Peak = FMath::Max(Peak, V);
        ++Count;
    }
    M.Avg = (Count > 0) ? (Sum / Count) : 0.0;
    M.Peak = Peak;
    return M;
}

bool UJointPoseClassifierComponent::ResampleAndSmooth(const TArray<FJointSample>& InSamples, TArray<FJointSample>& OutU) const
{
    OutU = ResampleUniform(InSamples);
    if (OutU.Num() < 3) return false;
    ApplyEMA(OutU);
    return true;
}

TArray<FJointSample> UJointPoseClassifierComponent::ResampleUniform(const TArray<FJointSample>& In) const
{
    if (In.Num() < 2) return {};

    const double T0 = In[0].Time;
    const double T1 = In.Last().Time;
    const int32 N = FMath::Max(2, (int)FMath::RoundToInt((T1 - T0) * KFixedHz) + 1);

    TArray<FJointSample> Out; Out.Reserve(N);
    int32 Index = 0;
    for (int32 i = 0; i < N; ++i)
    {
        const double T = T0 + i * KFixedDt;
        while (Index + 1 < In.Num() && In[Index + 1].Time < T) ++Index;
        const int32 K = FMath::Min(Index + 1, In.Num() - 1);

        const double TA = In[Index].Time, tB = In[K].Time;
        const double A = (tB > TA) ? FMath::Clamp((T - TA) / (tB - TA), 0.0, 1.0) : 0.0;

        FJointSample O;
        O.Time = T;
        O.Pos = FMath::Lerp(In[Index].Pos, In[K].Pos, A);
        Out.Add(O);
    }
    return Out;
}

void UJointPoseClassifierComponent::ApplyEMA(TArray<FJointSample>& U) const
{
    if (U.Num() == 0) return;
    FVector2f Pos = U[0].Pos;
    constexpr double Alpha = 1.0f; // 기존 코드 유지
    for (int32 i = 0; i < U.Num(); ++i)
    {
        Pos = (1 - Alpha) * Pos + Alpha * U[i].Pos;
        U[i].Pos = Pos;
    }
}

int UJointPoseClassifierComponent::MapUToSnapIndex(const TArray<double>& SnapTimes, const TArray<FJointSample>& U, int Ui, int& Cursor) const
{
    const double Time = U[Ui].Time;
    while (Cursor + 1 < SnapTimes.Num() && SnapTimes[Cursor + 1] <= Time) ++Cursor;
    const int32 A = FMath::Max(0, Cursor);
    const int32 B = FMath::Min(Cursor + 1, SnapTimes.Num() - 1);
    const double Da = FMath::Abs(SnapTimes[A] - Time);
    const double Db = FMath::Abs(SnapTimes[B] - Time);
    return (Db < Da) ? B : A;
}