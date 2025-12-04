// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Enums/GameStage.h"
#include "BattleGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameStageChangedDelegate, EGameStage, NewGameStage);

/**
 * 
 */
UCLASS()
class WEDLIKE_API ABattleGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	ABattleGameState();

protected:
	virtual void PostInitializeComponents() override;

// Waiting Timer Section
public:
	class UTimerComponent* GetWaitingTimerComponent() const { return WaitingTimerComponent; }

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UTimerComponent> WaitingTimerComponent;

// Battle Timer Section
public:
	class UTimerComponent* GetBattleTimerComponent() const { return BattleTimerComponent; }

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UTimerComponent> BattleTimerComponent;

	UFUNCTION()
	void StartBattle();

	void OpenResultLevel();

	UFUNCTION()
	void OpenResultLevelDelayed();

	FTimerHandle ResultDelayHandle;

// Game Level Section
public:
	FORCEINLINE EGameStage GetCurrentGameStage() const { return CurrentGameStage; }

	FOnGameStageChangedDelegate OnGameStageChangedDelegate;

	void SetCurrentGameStage(EGameStage NewGameStage);

private:
	EGameStage CurrentGameStage = EGameStage::WarmUp;

	void SetGameStageEasy();
	void SetGameStageHard();
	void SetGameStageFeverTime();

	void SetGameLevelTimer();

	FTimerHandle EasyTimerHandle;
	FTimerHandle HardTimerHandle;
	FTimerHandle FeverTimeTimerHandle;
};
