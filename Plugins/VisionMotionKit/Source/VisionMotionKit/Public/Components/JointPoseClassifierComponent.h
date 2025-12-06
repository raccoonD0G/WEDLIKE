// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PoseClassifierComponent.h"
#include "Utilities/VisionMotionUtils.h"
#include "JointPoseClassifierComponent.generated.h"

USTRUCT(BlueprintType)
struct FJointSample
{
    GENERATED_BODY()

    double    Time = 0.0;
    FVector2f Pos = FVector2f::ZeroVector;
};

USTRUCT(BlueprintType)
struct FJointMetrics
{
	GENERATED_BODY()

	double Peak = 0.0;
	double Avg = 0.0;
};
/**
 * 
 */
UCLASS()
class VISIONMOTIONKIT_API UJointPoseClassifierComponent : public UPoseClassifierComponent
{
	GENERATED_BODY()
	
public:
	virtual FVector2f GetJointPos(const FTimedPoseSnapshot* Snap);

protected:
	FVector2f GetJointPoseFromEnum(const FTimedPoseSnapshot* Snap, ECOCOKeypoint Joint);

	bool BuildSample(const TArray<const FTimedPoseSnapshot*>& Snaps, TArray<FJointSample>& OutSamples);

	FJointMetrics CalcMetricsPx(const TArray<FJointSample>& U) const;

	bool ResampleAndSmooth(const TArray<FJointSample>& InSamples, TArray<FJointSample>& OutU) const;
	TArray<FJointSample> ResampleUniform(const TArray<FJointSample>& In) const;
	void ApplyEMA(TArray<FJointSample>& U) const;

	int MapUToSnapIndex(const TArray<double>& SnapTimes, const TArray<FJointSample>& U, int Ui, int& Cursor) const;
};
