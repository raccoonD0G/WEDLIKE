// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/HandPoseClassifierComponent.h"
#include "Utilities/VisionMotionMathLibraries.h"

bool UHandPoseClassifierComponent::FindHandFor(const TArray<FHandPose>& Hands, uint16 PersonId, uint8 Which, const FVector2f& FallbackNearTo, FVector2D& OutCenter) const
{
    // 1) pid & which 정확히 일치
    for (int i = 0; i < Hands.Num(); ++i)
    {
        const FHandPose& H = Hands[i];
        if (H.PersonId == PersonId && H.Which == Which && UVisionMotionMathLibraries::IsFinite2D(H.Center))
        {
            OutCenter = (FVector2D)H.Center; return true;
        }
    }
    // 2) pid 일치 & which unknown(2): 손목 근처
    int bestIdx = INDEX_NONE; double bestD = DBL_MAX;
    for (int i = 0; i < Hands.Num(); ++i)
    {
        const FHandPose& H = Hands[i];
        if (H.PersonId == PersonId && UVisionMotionMathLibraries::IsFinite2D(H.Center))
        {
            const double d = FVector2D::Distance((FVector2D)H.Center, (FVector2D)FallbackNearTo);
            if (d < bestD) { bestD = d; bestIdx = i; }
        }
    }
    if (bestIdx != INDEX_NONE) { OutCenter = (FVector2D)Hands[bestIdx].Center; return true; }

    // 3) pid 불명: 전체 중 손목 근처
    bestIdx = INDEX_NONE; bestD = DBL_MAX;
    for (int i = 0; i < Hands.Num(); ++i)
    {
        const FHandPose& H = Hands[i];
        if (!UVisionMotionMathLibraries::IsFinite2D(H.Center)) continue;
        const double d = FVector2D::Distance((FVector2D)H.Center, (FVector2D)FallbackNearTo);
        if (d < bestD) { bestD = d; bestIdx = i; }
    }
    if (bestIdx != INDEX_NONE) { OutCenter = (FVector2D)Hands[bestIdx].Center; return true; }

    return false;
}

void UHandPoseClassifierComponent::ApplyEMA(TArray<FHandSample>& U) const
{
    if (U.Num() == 0) return;
    FVector2D l = U[0].L, r = U[0].R, c = U[0].C;
    constexpr double Alpha = 1.0f; // 기존 코드 유지
    for (int i = 0; i < U.Num(); ++i)
    {
        l = (1 - Alpha) * l + Alpha * U[i].L;
        r = (1 - Alpha) * r + Alpha * U[i].R;
        c = (1 - Alpha) * c + Alpha * U[i].C;
        U[i].L = l; U[i].R = r; U[i].C = c;
    }
}

TArray<FHandSample> UHandPoseClassifierComponent::ResampleUniform(const TArray<FHandSample>& In) const
{
    if (In.Num() < 2) return {};

    const double t0 = In[0].T;
    const double t1 = In.Last().T;
    const int    N = FMath::Max(2, (int)FMath::RoundToInt((t1 - t0) * KFixedHz) + 1);

    TArray<FHandSample> Out; Out.Reserve(N);
    int j = 0;
    for (int i = 0; i < N; ++i)
    {
        const double t = t0 + i * KFixedDt;
        while (j + 1 < In.Num() && In[j + 1].T < t) ++j;
        const int k = FMath::Min(j + 1, In.Num() - 1);

        const double tA = In[j].T, tB = In[k].T;
        const double a = (tB > tA) ? FMath::Clamp((t - tA) / (tB - tA), 0.0, 1.0) : 0.0;

        FHandSample o;
        o.T = t;
        o.L = FMath::Lerp(In[j].L, In[k].L, a);
        o.R = FMath::Lerp(In[j].R, In[k].R, a);
        o.C = FMath::Lerp(In[j].C, In[k].C, a);
        o.ShoulderW = FMath::Lerp(In[j].ShoulderW, In[k].ShoulderW, a);
        Out.Add(o);
    }
    return Out;
}

int UHandPoseClassifierComponent::MapUToSnapIndex(const TArray<double>& SnapTimes, const TArray<FHandSample>& U, int Ui, int& Cursor) const
{
    const double t = U[Ui].T;
    while (Cursor + 1 < SnapTimes.Num() && SnapTimes[Cursor + 1] <= t) ++Cursor;
    const int a = FMath::Max(0, Cursor);
    const int b = FMath::Min(Cursor + 1, SnapTimes.Num() - 1);
    const double da = FMath::Abs(SnapTimes[a] - t);
    const double db = FMath::Abs(SnapTimes[b] - t);
    return (db < da) ? b : a;
}
