// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/PenaltyProp.h"
#include "Components/IntHealthComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include <GameStates/BattleGameState.h>
#include "Components/ProjectileFallingComponent.h"
#include <Subsystems/EmailScoreSubsystem.h>

APenaltyProp::APenaltyProp()
{
	GetIntHealthComponent()->OnHealthZeroDelegate.AddDynamic(this, &APenaltyProp::DecreaseScore);

	GameStageToPropFallSpeed.Add(EGameStage::WarmUp, 100.0f);
	GameStageToPropFallSpeed.Add(EGameStage::Easy, 200.0f);
	GameStageToPropFallSpeed.Add(EGameStage::Hard, 350.0f);
	GameStageToPropFallSpeed.Add(EGameStage::FeverTime, 500.0f);

	ProjectileFallingComponent = CreateDefaultSubobject<UProjectileFallingComponent>(TEXT("ProjectileFallingComponent"));
}

void APenaltyProp::Init(float InSpeed)
{
	ProjectileFallingComponent->Init(InSpeed);
}

void APenaltyProp::DecreaseScore()
{
	UEmailScoreSubsystem* EmailScoreSubsystem = GetGameInstance()->GetSubsystem<UEmailScoreSubsystem>();
	EmailScoreSubsystem->SubtractLastScore(ScoreDecreaseAmount);
}

float APenaltyProp::GetFallingSpeed(EGameStage CurrentGameStage)
{
	return GameStageToPropFallSpeed[CurrentGameStage];
}