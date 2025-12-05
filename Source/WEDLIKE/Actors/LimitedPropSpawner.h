// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/GamePropSpawner.h"
#include "LimitedPropSpawner.generated.h"

/**
 * 
 */
UCLASS()
class WEDLIKE_API ALimitedPropSpawner : public AGamePropSpawner
{
	GENERATED_BODY()
	
protected:
	virtual class AGameProp* BeginSpawnGameProp() override;
	virtual class AGameProp* EndSpawnGameProp(class AGameProp* OnGoingGamePeop);

private:
	UPROPERTY(VisibleAnywhere)
	TSet<TObjectPtr<class AGameProp>> SpawnedProps;

	void AddSpawnedProp(class AGameProp* NewProp);

	UFUNCTION()
	void OnPropDestroyed(AActor* DestroyedActor);

	UPROPERTY(EditAnywhere)
	int32 MaxPropCount = 1;

// Scale Section
private:
	UPROPERTY(EditAnywhere)
	FVector LargeScale = FVector(2.0f, 2.0f, 2.0f);

	UPROPERTY(EditAnywhere)
	float LargeRatio = 0.2f;
};
