// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/PhysicsProp.h"
#include "Components/IntHealthComponent.h"
#include "Components/BoxComponent.h"

APhysicsProp::APhysicsProp()
{
	PhysicsRootComponent = RootComponent;

	PhysicsBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("PhysicsBoxComponent"));
	RootComponent = PhysicsBoxComponent;

	PhysicsRootComponent->SetupAttachment(RootComponent);
	
	PhysicsBoxComponent->SetCollisionProfileName(TEXT("BlockAll"));
	PhysicsBoxComponent->SetSimulatePhysics(true);
	PhysicsBoxComponent->SetNotifyRigidBodyCollision(true);
	PhysicsBoxComponent->SetEnableGravity(true);
	PhysicsBoxComponent->SetBoxExtent(GetHitBoxComponent()->GetScaledBoxExtent());

	GetHitBoxComponent()->OnComponentBeginOverlap.AddDynamic(this, &APhysicsProp::OnDamaged);
}



void APhysicsProp::OnDamaged(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherComp->GetCollisionProfileName() == TEXT("DoDamageBox"))
	{
		if (PhysicsBoxComponent)
		{
			FVector ImpulseDir = (GetActorLocation() - OtherActor->GetActorLocation()).GetSafeNormal();

			PhysicsBoxComponent->AddImpulse(ImpulseDir * ImpulseStrength);
		}

		bAttacked = true;
	}
}

void APhysicsProp::OnWallHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (bAttacked && OtherActor->ActorHasTag("NotBouncyWall"))
	{
		FVector ExtraImpulse = NormalImpulse * -0.5f;
		PhysicsBoxComponent->AddImpulse(ExtraImpulse);
	}

	bAttacked = false;
}