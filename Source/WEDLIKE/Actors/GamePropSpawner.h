// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enums/GameStage.h"
#include "GamePropSpawner.generated.h"


UCLASS()
class WEDLIKE_API AGamePropSpawner : public AActor
{
	GENERATED_BODY()
	
public:
	AGamePropSpawner();

protected:
	virtual void BeginPlay() override;

// Component Section
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> SceneComponent;

// Spawn Section
protected:
	virtual class AGameProp* BeginSpawnGameProp();

	virtual class AGameProp* EndSpawnGameProp(class AGameProp* OnGoingGamePeop);

	UFUNCTION(BlueprintCallable)
	void StartSpawning();

	UFUNCTION(BlueprintCallable)
	void StopSpawning();

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UBoxComponent> SpawnBoxComponent;

	FTimerHandle SpawnTimerHandle;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class AGameProp>> GamePropClasses;

	UPROPERTY(EditAnywhere)
	uint8 bDoSapwn : 1 = true;

	void SpawnAndScheduleNext();

	TSubclassOf<class AGameProp> GetRandomClass(TArray<TSubclassOf<class AGameProp>> Classes) const;

	FVector GetRandomPointInSpawnBox() const;

	UPROPERTY(EditAnywhere)
	FVector SpawnScale = FVector(1.0f, 1.0f, 1.0f);

	void ClearTimerNextTick();

// Game Stage Section
private:
	class ABattleGameState* GetBattleGameState();

	UPROPERTY(EditAnywhere)
	TMap<EGameStage, float> GameStageToSpawnInterval;
};
