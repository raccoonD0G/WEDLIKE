// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/SingleSwingEffect.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include <Utilities/VisionMotionMathLibraries.h>

ASingleSwingEffect::ASingleSwingEffect()
{
    PrimaryActorTick.bCanEverTick = false;

    SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneComponent;

    PlaneBox = CreateDefaultSubobject<UBoxComponent>(TEXT("PlaneBox"));
    PlaneBox->SetupAttachment(RootComponent);
    
    AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
    AudioComponent->SetupAttachment(RootComponent);
}

void ASingleSwingEffect::InitializeSize(const FVector& InCenter, const FRotator& InRotation, float InLength, float InWidth)
{
    // 1) 기준 길이 잡기
    constexpr float L0 = 10.0f;

    // --- 0) 최소 길이 보정: InLength이 너무 짧으면 최소값으로 보정
    const float EffectiveLength = FMath::Max(InLength, MinAttackLength);

    // 2) 스케일 함수로 길이 스케일 계산
    const float LengthScale = UVisionMotionMathLibraries::SuperlinearScale(EffectiveLength, L0, 1.3f, 0.7f);

    if (EffectSounds.Num() > 0)
    {
        AudioComponent->SetSound(EffectSounds[FMath::RandRange(0, EffectSounds.Num() - 1)]);
        AudioComponent->Play();
    }

    // 3) HalfExtent 계산 시 길이에 스케일 및 최소값 적용
    const float YHalf = FMath::Max(1.f, (EffectiveLength * 0.5f) * LengthScale / 10.0f);

    const FVector HalfExtent(
        PlaneHalfThickness, // X: 두께 Half
        YHalf,              // Y: 진행방향 HalfLength (스케일 + 최소값 적용됨)
        FMath::Max(1.f, InWidth * 0.5f) // Z: 폭 Half
    );

    FRotator CorrectedRotation = InRotation;
    CorrectedRotation.Add(90.f, 0.f, 90.f);
    SetActorLocation(InCenter);
    SetActorRotation(CorrectedRotation);

    // 박스 크기 세팅
    PlaneBox->SetBoxExtent(HalfExtent, false);
    PlaneBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    PlaneBox->SetGenerateOverlapEvents(true);

    // -X 면 위치 보정
    PlaneBox->SetRelativeLocation(FVector(+HalfExtent.X, 0, 0));
    PlaneBox->SetRelativeRotation(FRotator::ZeroRotator);

    // 디버그 박스
    if (bDoDebug)
    {
        const FQuat Q = CorrectedRotation.Quaternion();
        const FVector CenterFromStartFace = InCenter + Q.GetAxisX() * HalfExtent.X;
        DrawDebugBox(GetWorld(), CenterFromStartFace, HalfExtent, Q, FColor::Cyan, false, 1.0f, 0, 2.f);
    }

    SpawnOrUpdateFX(InCenter, FRotator(90, 90, 0), EffectiveLength * LengthScale, InWidth);
}

void ASingleSwingEffect::SpawnOrUpdateFX(const FVector& Center, const FRotator& Rotation, float InLength, float InWidth)
{
    if (SwingPlaneFXs.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("SwingPlaneFXs is empty in ASingleSwingEffect"));
        return;
    }

    UNiagaraSystem* SwingPlaneFX = SwingPlaneFXs[FMath::RandRange(0, SwingPlaneFXs.Num() - 1)];

    UNiagaraComponent* NewFX = UNiagaraFunctionLibrary::SpawnSystemAttached(
        SwingPlaneFX,
        RootComponent,
        NAME_None,
        FVector::ZeroVector,
        Rotation,
        EAttachLocation::KeepRelativeOffset,
        true,
        true
    );

    if (NewFX)
    {
        const float NormLength = FMath::Max(InLength / 100.0f, MinAttackLength);
        const float NormWidth = FMath::Max(InWidth, 1.0f) / 100.0f;
        NewFX->SetVariableVec2(ScaleMultiplierParamName, FVector2D(1.0f, NormLength));
    }
}
