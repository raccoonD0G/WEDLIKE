// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/NDIReceiveActor.h"
#include "Interfaces/ClampVolumeInterface.h"
#include "MirroredPlayer.generated.h"

/**
 * 
 */
UCLASS()
class VISIONMOTIONKIT_API AMirroredPlayer : public ANDIReceiveActor, public IClampVolumeInterface
{
	GENERATED_BODY()

public:
	AMirroredPlayer();

	virtual class UBoxComponent* GetClampVolume() const override;

protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

private:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<class UBoxComponent> ClampVolume = nullptr;

public:
	FORCEINLINE class UPoseUdpReceiverComponent* GetPoseReceiver() { return PoseReceiver; }
	FORCEINLINE class USingleSwingClassifierComponent* GetLeftSingleSwingClassifierComponent() { return LeftSingleSwingClassifierComponent; }
	FORCEINLINE class USingleSwingClassifierComponent* GetRightSingleSwingClassifierComponent() { return RightSingleSwingClassifierComponent; }

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UPoseUdpReceiverComponent> PoseReceiver;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USingleSwingClassifierComponent> LeftSingleSwingClassifierComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USingleSwingClassifierComponent> RightSingleSwingClassifierComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UStaticMeshComponent> VideoComponent;
};
