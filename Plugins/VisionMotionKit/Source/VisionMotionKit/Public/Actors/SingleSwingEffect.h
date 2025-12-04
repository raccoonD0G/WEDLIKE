// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SingleSwingEffect.generated.h"

UCLASS()
class VISIONMOTIONKIT_API ASingleSwingEffect : public AActor
{
    GENERATED_BODY()

public:
    ASingleSwingEffect();

public:
    void InitializeSize(const FVector& InCenter, const FRotator& InRotation, float InLength, float InWidth);

    void SpawnOrUpdateFX(const FVector& Center, const FRotator& Rotation, float InLength, float InWidth);

private:
    // 최소 전체 스윙 길이
    UPROPERTY(EditAnywhere)
    float MinAttackLength = 0.0f;

private:
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* SceneComponent;

protected:
    UPROPERTY(VisibleAnywhere)
    class UBoxComponent* PlaneBox;

private:
    UPROPERTY(EditAnywhere)
    float PlaneHalfThickness = 20.f; // OverlapPlaneOnce와 동일 의미의 두께 HalfExtent

    UPROPERTY(VisibleAnywhere)
    FVector2D PlaneHalfSize = FVector2D(50.f, 20.f); // X=HalfLen(진행), Y=HalfWidth(폭)

    float LifeSeconds = 1.0f;

    FVector AttackCenter;
    FRotator AttackRotation;
    float PlaneLength;
    float PlaneWidth;

private:
	UPROPERTY(EditAnywhere, Category = "Debug")
    uint8 bDoDebug : 1 = true;

// Effects Section
private:
	UPROPERTY(EditAnywhere, Category = "FX")
	TArray<TObjectPtr<class UNiagaraSystem>> SwingPlaneFXs;

    FName ScaleMultiplierParamName = TEXT("User.ScaleMultiplier");

// Sound Section
private:
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UAudioComponent> AudioComponent;

    UPROPERTY(EditAnywhere)
    TArray<TObjectPtr<USoundBase>> EffectSounds;

};
