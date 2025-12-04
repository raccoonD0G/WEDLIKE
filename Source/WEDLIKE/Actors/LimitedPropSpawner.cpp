// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/LimitedPropSpawner.h"
#include "Actors/GameProp.h"

AGameProp* ALimitedPropSpawner::SpawnFallingProp()
{
	AGameProp* NewProp = Super::SpawnFallingProp();

	AddSpawnedProp(NewProp);

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
