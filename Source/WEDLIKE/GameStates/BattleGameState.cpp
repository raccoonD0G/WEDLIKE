// Fill out your copyright notice in the Description page of Project Settings.


#include "GameStates/BattleGameState.h"
#include "Components/TimerComponent.h"
#include <Kismet/GameplayStatics.h>

ABattleGameState::ABattleGameState()
{
	BattleTimerComponent = CreateDefaultSubobject<UTimerComponent>(TEXT("BattleTimerComponent"));
	WaitingTimerComponent = CreateDefaultSubobject<UTimerComponent>(TEXT("WaitingTimerComponent"));
}

void ABattleGameState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	WaitingTimerComponent->StartTimer();

	WaitingTimerComponent->OnTimerEndDelegate.AddDynamic(this, &ABattleGameState::StartBattle);
	BattleTimerComponent->OnTimerEndDelegate.AddDynamic(this, &ABattleGameState::OpenResultLevelDelayed);
}

void ABattleGameState::StartBattle()
{
	BattleTimerComponent->StartTimer();
	SetGameLevelTimer();
}

void ABattleGameState::OpenResultLevel()
{
	UWorld* World = GetWorld();
	if (!World) return;

	UGameplayStatics::OpenLevel(World, TEXT("ResultMap"));
}

void ABattleGameState::OpenResultLevelDelayed()
{
	UWorld* World = GetWorld();
	if (!World) return;

	World->GetTimerManager().SetTimer(
		ResultDelayHandle,
		this,
		&ABattleGameState::OpenResultLevel,
		3.0f,
		false
	);
}

void ABattleGameState::SetCurrentGameStage(EGameStage NewGameStage)
{
	CurrentGameStage = NewGameStage;

	OnGameStageChangedDelegate.Broadcast(NewGameStage);
}

void ABattleGameState::SetGameStageEasy()
{
	SetCurrentGameStage(EGameStage::Easy);
}

void ABattleGameState::SetGameStageHard()
{
	SetCurrentGameStage(EGameStage::Hard);
}

void ABattleGameState::SetGameStageFeverTime()
{
	SetCurrentGameStage(EGameStage::FeverTime);
}

void ABattleGameState::SetGameLevelTimer()
{
	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().SetTimer(
			EasyTimerHandle,
			this,
			&ABattleGameState::SetGameStageEasy,
			5.0f,
			false);

		World->GetTimerManager().SetTimer(
			HardTimerHandle,
			this,
			&ABattleGameState::SetGameStageHard,
			15.0f,
			false);

		World->GetTimerManager().SetTimer(
			FeverTimeTimerHandle,
			this,
			&ABattleGameState::SetGameStageFeverTime,
			25.0f,
			false);

	}
}