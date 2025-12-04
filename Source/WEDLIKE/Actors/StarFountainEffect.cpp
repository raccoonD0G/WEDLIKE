// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/StarFountainEffect.h"
#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

AStarFountainEffect::AStarFountainEffect()
{
    PrimaryActorTick.bCanEverTick = true;
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
    ElapsedTime = 0.0f;
}

void AStarFountainEffect::BeginPlay()
{
    Super::BeginPlay();

    if (!StarFlipbookAsset)
    {
        UE_LOG(LogTemp, Warning, TEXT("Flipbook Asset is missing"));
        Destroy();
        return;
    }

    int32 NumStarsToSpawn = FMath::RandRange(MinStars, MaxStars);

    for (int32 i = 0; i < NumStarsToSpawn; ++i)
    {
        FString CompName = FString::Printf(TEXT("StarFlipbook_%d"), i);
        UPaperFlipbookComponent* NewStar = NewObject<UPaperFlipbookComponent>(this, FName(*CompName));

        if (NewStar)
        {
            NewStar->SetupAttachment(RootComponent);
            NewStar->SetFlipbook(StarFlipbookAsset);
            NewStar->RegisterComponent();
            NewStar->SetEnableGravity(true);

            float RandomScale = FMath::RandRange(MinScale, MaxScale);
            NewStar->SetWorldScale3D(FVector(RandomScale));
            InitialScales.Add(RandomScale);

            FVector RandomVelocity = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(FVector::UpVector, 30.f);
            float RandomSpeed = FMath::RandRange(MinImpulse.Z, MaxImpulse.Z);
            RandomVelocity *= RandomSpeed;

            StarVelocities.Add(RandomVelocity);
            StarComponents.Add(NewStar);
        }
    }
}

void AStarFountainEffect::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    ElapsedTime += DeltaTime;

    if (ElapsedTime >= EffectLifetime)
    {
        Destroy();
        return;
    }

    float ShrinkAlpha = 0.0f;
    if (ElapsedTime > ShrinkStartTime)
    {
        ShrinkAlpha = (ElapsedTime - ShrinkStartTime) / (EffectLifetime - ShrinkStartTime);
        ShrinkAlpha = FMath::Clamp(ShrinkAlpha, 0.0f, 1.0f);
    }

    FVector CameraLocation = FVector::ZeroVector;
    if (APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0))
    {
        CameraLocation = CameraManager->GetCameraLocation();
    }

    for (int32 i = 0; i < StarComponents.Num(); ++i)
    {
        UPaperFlipbookComponent* StarComp = StarComponents[i];
        if (!StarComp) continue;

        StarVelocities[i].Z += GravityZ * DeltaTime;
        FVector NewLocation = StarComp->GetRelativeLocation() + (StarVelocities[i] * DeltaTime);
        StarComp->SetRelativeLocation(NewLocation);

        if (ElapsedTime > ShrinkStartTime)
        {
            float CurrentScaleScaleFactor = FMath::Lerp(1.0f, 0.0f, ShrinkAlpha);
            float NewScale = InitialScales[i] * CurrentScaleScaleFactor;
            StarComp->SetWorldScale3D(FVector(NewScale));
        }

        if (!CameraLocation.IsZero())
        {
            FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(StarComp->GetComponentLocation(), CameraLocation);

            LookAtRot.Yaw += -90.0f;

            StarComp->SetWorldRotation(LookAtRot);
        }
    }
}