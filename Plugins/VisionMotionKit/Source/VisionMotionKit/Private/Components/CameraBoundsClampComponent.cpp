// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/CameraBoundsClampComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/SpringArmComponent.h"

UCameraBoundsClampComponent::UCameraBoundsClampComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UCameraBoundsClampComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UCameraBoundsClampComponent::Init(UBoxComponent* InBoundsBox, USpringArmComponent* InSpringArm)
{
    BoundsBox = InBoundsBox;
    SpringArm = InSpringArm;
}

void UCameraBoundsClampComponent::TickComponent(float DeltaTime, ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    ClampNow();
}

void UCameraBoundsClampComponent::ClampNow()
{
    if (!BoundsBox || !SpringArm) return;

    // 스프링암의 시작점(피벗 위치, 즉 카메라 소켓 위치 아님!)
    const FVector PivotWorld = SpringArm->GetComponentLocation();

    // 박스 로컬 공간으로 변환
    const FTransform BoxXf = BoundsBox->GetComponentTransform();
    const FVector PivotLocal = BoxXf.InverseTransformPosition(PivotWorld);

    // 박스 로컬 extents
    const FVector Extent = BoundsBox->GetUnscaledBoxExtent();

    // 클램프
    FVector ClampedLocal = PivotLocal;
    ClampedLocal.X = FMath::Clamp(ClampedLocal.X, -Extent.X, Extent.X);
    ClampedLocal.Y = FMath::Clamp(ClampedLocal.Y, -Extent.Y, Extent.Y);
    ClampedLocal.Z = FMath::Clamp(ClampedLocal.Z, -Extent.Z, Extent.Z);

    // 다시 월드 위치로 변환
    const FVector ClampedWorld = BoxXf.TransformPosition(ClampedLocal);

    // 차이를 스프링암 부모 기준으로 보정
    USceneComponent* Parent = SpringArm->GetAttachParent();
    if (Parent)
    {
        const FVector Local = Parent->GetComponentTransform().InverseTransformPosition(ClampedWorld);
        SpringArm->SetRelativeLocation(Local);
    }
    else
    {
        SpringArm->SetWorldLocation(ClampedWorld);
    }
}