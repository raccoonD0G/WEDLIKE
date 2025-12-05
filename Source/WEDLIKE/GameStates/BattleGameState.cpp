// Fill out your copyright notice in the Description page of Project Settings.


#include "GameStates/BattleGameState.h"
#include "Components/TimerComponent.h"
#include <Kismet/GameplayStatics.h>
#include "Components/AudioComponent.h"

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
	BattleTimerComponent->OnTimerEndDelegate.AddDynamic(this, &ABattleGameState::EndBattle);

	WaitingTimerComponent->OnTimerUpdateDelegate.AddDynamic(this, &ABattleGameState::PlayCountSound);
	PlayCountSound(WaitingTimerComponent->GetCurrentTime());
}

void ABattleGameState::StartBattle()
{
	BattleTimerComponent->StartTimer();
	SetGameLevelTimer();

	if (GameStartSound)
	{
		UAudioComponent* AudioComponent = UGameplayStatics::CreateSound2D(this, GameStartSound);
		if (AudioComponent)
		{
			AudioComponent->Play();
		}
	}
}

void ABattleGameState::EndBattle()
{
	if(GameOverSound)
	{
		UAudioComponent* AudioComponent = UGameplayStatics::CreateSound2D(this, GameOverSound);
		if (AudioComponent)
		{
			AudioComponent->Play();
		}
	}

	OpenResultLevelDelayed();
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

void ABattleGameState::PlayCountSound(int32 CurrentCount)
{
	if (CountSounds.Contains(CurrentCount))
	{
		UAudioComponent* AudioComponent = UGameplayStatics::CreateSound2D(this, CountSounds[CurrentCount]);
		if (AudioComponent)
		{
			AudioComponent->Play();
		}
	}
}
