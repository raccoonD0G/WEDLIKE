// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Actors/MirroredPlayer.h"
#include "HandFollower.generated.h"

UCLASS()
class VISIONMOTIONKIT_API AHandFollower : public AActor
{
	GENERATED_BODY()
	
public:
	AHandFollower();
	void SetOwningPlayer(AMirroredPlayer* InMirroredPlayer);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class AMirroredPlayer> OwningPlayer;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> SceneComponent;

// Pose Section
public:
	virtual void SetHandPoseClassifierComponent(class UHandPoseClassifierComponent* InHandPoseClassifierComponent);

protected:
	// 연장 비율 설정
	UPROPERTY(EditAnywhere, Category = "HandFollower|Pose")
	float ExtendRatio = 0.15f;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UHandPoseClassifierComponent> HandPoseClassifierComponent;

private:
	// ---- 바인딩 ----
	UFUNCTION()
	void OnPoseInput(const FVector2f& Pelvis2D, const TArray<FPersonPose>& Poses, float PixelToUU, const FTransform& OwnerXform);

	// ---- 포즈 처리 ----
	void SetHandFollowerPose(const FVector& CenterWorld);

	// 한쪽 손의 연장 지점 계산: 성공하면 OutPoint 반환
	bool TryComputeExtendedPoint(const FVector2f& Pelvis2D, const FPersonPose& Pose, const FTransform& OwnerXform, bool bRightHand, FVector& OutPoint, float* OutScore = nullptr) const;
};
