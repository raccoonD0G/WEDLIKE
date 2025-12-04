// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/PhysicsProp.h"
#include "Components/IntHealthComponent.h"
#include "Components/BoxComponent.h"
#include "Actors/StarFountainEffect.h"

APhysicsProp::APhysicsProp()
{
	PhysicsRootComponent = RootComponent;

	GetIntHealthComponent()->OnHealthChangeDelegate.AddDynamic(this, &APhysicsProp::PlayDamagedEffect);
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

void APhysicsProp::PlayDamagedEffect(int32 NewCurrentHealth, int32 NewMaxHealth)
{
	UWorld* World = GetWorld();
	if (!World) return;

	FTransform EffectTransform = GetActorTransform();
	FVector EffectLocation = EffectTransform.GetLocation();
	EffectTransform.SetLocation(FVector(EffectLocation.X - 30.0f, EffectLocation.Y, EffectLocation.Z));

	check(StarFountainEffectClass);

	AStarFountainEffect* StarFountainEffect = World->SpawnActor<AStarFountainEffect>(StarFountainEffectClass, EffectTransform);
	check(StarFountainEffect);

	StarFountainEffect->SetActorRotation(FRotator());
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