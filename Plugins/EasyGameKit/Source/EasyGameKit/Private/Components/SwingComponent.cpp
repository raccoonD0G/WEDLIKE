// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SwingComponent.h"


USwingComponent::USwingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USwingComponent::BeginPlay()
{
	Super::BeginPlay();

	StartSide = FMath::RandBool() ? 1.f : -1.f;
}

void USwingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bDoSwing)
	{
		SwingTick(DeltaTime);
	}
}

void USwingComponent::Init(USceneComponent* NewUpdatedComponent)
{
	SetUpdatedComponent(NewUpdatedComponent);
}

void USwingComponent::SetUpdatedComponent(USceneComponent* NewUpdatedComponent)
{
	UpdatedComponent = NewUpdatedComponent;
}

void USwingComponent::StartSwing()
{
	if (!UpdatedComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("SwingComponent not initialized."));
	}

	bDoSwing = true;
}

void USwingComponent::StopSwing()
{
	bDoSwing = false;
}

void USwingComponent::SwingTick(float DeltaTime)
{
	ElapsedTime += DeltaTime;

	const float Angle = SwingAmplitudeDeg * FMath::Sin(ElapsedTime * SwingSpeed) * StartSide;

	if (UpdatedComponent)
	{
		UpdatedComponent->SetRelativeRotation(FRotator(0.f, 0.f, Angle));
	}
}