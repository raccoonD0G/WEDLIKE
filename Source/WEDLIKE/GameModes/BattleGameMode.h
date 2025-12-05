// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BattleGameMode.generated.h"

/**
 * 
 */
UCLASS()
class WEDLIKE_API ABattleGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UFUNCTION()
	void StartGame();

	UFUNCTION()
	void EndGame();

	void BindGameStageEvents();
	
	FTimerHandle BindGameStageTimerHandle;

	UPROPERTY(VisibleAnywhere)
	FString CurrentEmail;

private:
	void SaveEmailCSV();
};
