// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StarFountainEffect.generated.h"

UCLASS()
class WEDLIKE_API AStarFountainEffect : public AActor
{
    GENERATED_BODY()

public:
    AStarFountainEffect();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

protected:
    UPROPERTY(EditAnywhere)
    class UPaperFlipbook* StarFlipbookAsset;

    UPROPERTY(EditAnywhere)
    int32 MinStars = 10;

    UPROPERTY(EditAnywhere)
    int32 MaxStars = 20;

    UPROPERTY(EditAnywhere)
    float MinScale = 0.01;

    UPROPERTY(EditAnywhere)
    float MaxScale = 0.05;

    UPROPERTY(EditAnywhere)
    FVector MinImpulse = FVector(-400.f, -400.f, 300.f);

    UPROPERTY(EditAnywhere)
    FVector MaxImpulse = FVector(600.f, 600.f, 700.f);

    UPROPERTY(EditAnywhere)
    float GravityZ = -450.0f;

    UPROPERTY(EditAnywhere)
    float EffectLifetime = 1.0f;

    UPROPERTY(EditAnywhere)
    float ShrinkStartTime = 0.5f;

private:
    UPROPERTY(VisibleAnywhere)
    TArray<class UPaperFlipbookComponent*> StarComponents;

    TArray<FVector> StarVelocities;

    TArray<float> InitialScales;

    float ElapsedTime;
};
