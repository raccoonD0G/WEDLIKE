// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStates/BattlePlayerState.h"
#include "Components/IntHealthComponent.h"
#include <Widgets/BattleWidget.h>
#include <HUDs/LevelHUD.h>
#include <Kismet/GameplayStatics.h>
#include <Subsystems/EmailScoreSubsystem.h>

ABattlePlayerState::ABattlePlayerState()
{
	PropCounts.Add(EPropType::OriginalRamyun, 0);
	PropCounts.Add(EPropType::ShrimpRamyun, 0);
	PropCounts.Add(EPropType::SpicyRamyun, 0);
	PropCounts.Add(EPropType::Tangerine, 0);
	PropCounts.Add(EPropType::Topokki, 0);

	PropActionScores.Add(EPropType::OriginalRamyun, 10000);
	PropActionScores.Add(EPropType::ShrimpRamyun, 10000);
	PropActionScores.Add(EPropType::Tangerine, 10000);
	PropActionScores.Add(EPropType::Topokki, 10000);
	PropActionScores.Add(EPropType::SpicyRamyun, 10000);
}

void ABattlePlayerState::BeginPlay()
{
	Super::BeginPlay();

	EmailScoreSubsystem = GetGameInstance()->GetSubsystem<UEmailScoreSubsystem>();
}

int32 ABattlePlayerState::GetPropCount(EPropType PropType) const
{
	const int32* CountPtr = PropCounts.Find(PropType);
	return CountPtr ? *CountPtr : 0;
}

void ABattlePlayerState::IncreasePropCount(EPropType PropType)
{
	PropCounts[PropType]++;
	
	if (PropCounts[PropType] % PlayVideoCount == 0)
	{
		EmailScoreSubsystem->AddLastScore(PropActionScores[PropType]);
		PlayVideoByType(PropType);
	}

	OnPropCountChangeDelegate.Broadcast(PropType, PropCounts[PropType]);
}

void ABattlePlayerState::PlayVideoByType(EPropType PropType)
{
	UWorld* World = GetWorld();
	if (!World) return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
	if (!PC) return;

	ALevelHUD* LevelHUD = Cast<ALevelHUD>(PC->GetHUD());
	if (!LevelHUD) return;

	UBattleWidget* BattleWidget = Cast<UBattleWidget>(LevelHUD->GetLevelWidget());
	if (!BattleWidget) return;

	BattleWidget->PlayVideoByType(PropType);
}