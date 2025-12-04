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
	virtual class AGameProp* SpawnFallingProp() override;

private:
	UPROPERTY(VisibleAnywhere)
	TSet<TObjectPtr<class AGameProp>> SpawnedProps;

	void AddSpawnedProp(class AGameProp* NewProp);

	UFUNCTION()
	void OnPropDestroyed(AActor* DestroyedActor);

	UPROPERTY(EditAnywhere)
	int32 MaxPropCount = 1;
};
