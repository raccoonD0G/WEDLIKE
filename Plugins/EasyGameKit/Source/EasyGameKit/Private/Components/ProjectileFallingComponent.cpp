// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/ProjectileFallingComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

UProjectileFallingComponent::UProjectileFallingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UProjectileFallingComponent::Init(float InSpeed)
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	if (ProjectileMovement)
	{
		UE_LOG(LogTemp, Warning, TEXT("ProjectileMovement exist"));
		return;
	}

	ProjectileMovement = NewObject<UProjectileMovementComponent>(Owner, UProjectileMovementComponent::StaticClass(), TEXT("DynamicProjectileMovement"));
	check(ProjectileMovement);
	
	ProjectileMovement->RegisterComponent();
	ProjectileMovement->SetUpdatedComponent(Owner->GetRootComponent());
	ProjectileMovement->ProjectileGravityScale = 0.f;

	InSpeed *= SpeedMultiplier;
	ProjectileMovement->InitialSpeed = InSpeed;
	ProjectileMovement->MaxSpeed = InSpeed;
	ProjectileMovement->Velocity = FVector(0.f, 0.f, -InSpeed);
}