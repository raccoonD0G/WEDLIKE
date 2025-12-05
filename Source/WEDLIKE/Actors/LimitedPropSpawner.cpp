// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/LimitedPropSpawner.h"
#include "Actors/GameProp.h"
#include "Components/IntHealthComponent.h"

AGameProp* ALimitedPropSpawner::BeginSpawnGameProp()
{
	AGameProp* NewProp = Super::BeginSpawnGameProp();

	AddSpawnedProp(NewProp);

	return NewProp;
}

AGameProp* ALimitedPropSpawner::EndSpawnGameProp(AGameProp* OnGoingGamePeop)
{
	AGameProp* NewProp = Super::EndSpawnGameProp(OnGoingGamePeop);

	if (FMath::FRand() <= LargeRatio)
	{
		NewProp->SetActorScale3D(LargeScale);
		UIntHealthComponent* IntHealthComponent = NewProp->GetIntHealthComponent();

		int32 NewMaxHealth = IntHealthComponent->GetMaxHealth() * 2;
		IntHealthComponent->SetMaxHealth(NewMaxHealth);
		IntHealthComponent->SetFullHealth();
	}

	return NewProp;
}

void ALimitedPropSpawner::AddSpawnedProp(class AGameProp* NewProp)
{
	if (NewProp)
	{
		NewProp->OnDestroyed.AddDynamic(this, &ALimitedPropSpawner::OnPropDestroyed);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NewProp nullptr"));
	}

	SpawnedProps.Add(NewProp);
	if (SpawnedProps.Num() >= MaxPropCount)
	{
		StopSpawning();
	}
}

void ALimitedPropSpawner::OnPropDestroyed(AActor* DestroyedActor)
{
	AGameProp* FallingProp = Cast<AGameProp>(DestroyedActor);
	SpawnedProps.Remove(FallingProp);
	StartSpawning();
}
