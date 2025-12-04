// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ViewPlayer.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerHealthChanged, int32, MaxHealth, int32, CurrentHealth);

UCLASS()
class VISIONMOTIONKIT_API AViewPlayer : public APawn
{
	GENERATED_BODY()

public:
	AViewPlayer();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

private:
	float ViewScale = 0.5f;

private:
    UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USceneComponent> SceneComponent;

// Spawn Actor Section
private:
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<class AMirroredPlayer> MirroredPlayer;

    UPROPERTY(EditAnywhere)
    TSubclassOf<class AMirroredPlayer> MirroredPlayerClass;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<class ASingleSwingFollower> SingleSwingFollowerLeft;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<class ASingleSwingFollower> SingleSwingFollowerRight;

    UPROPERTY(EditAnywhere)
    TSubclassOf<class ASingleSwingFollower> SingleSwingFollowerClass;

    UPROPERTY(EditAnywhere)
    float FollowLagSpeed = 12.f;

    UPROPERTY(EditAnywhere)
    FVector FollowWorldOffset = FVector::ZeroVector;

// Camera Section
public:
    FORCEINLINE class USpringArmComponent* GetSpringArmComponent() const { return SpringArmComponent; }

private:
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<class USpringArmComponent> SpringArmComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Follow", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<class UCameraBoundsClampComponent> CameraBoundsClampComponent;

    UPROPERTY(EditAnywhere)
    TSubclassOf<class APlayerCam> PlayerCamClass;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<class APlayerCam> PlayerCam;

// Niagara Section
public:
    void PlayNiagaraOverCamera(class UNiagaraSystem* NiagaraSystem, const FRotator& NiagaraRotation, const FVector& NiagaraScale);
};
