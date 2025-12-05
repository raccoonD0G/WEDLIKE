// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/ScoreProp.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerStates/BattlePlayerState.h"
#include "Components/IntHealthComponent.h"
#include "PaperFlipbookComponent.h"
#include "Widgets/PropIconWidget.h"
#include <Widgets/BattleWidget.h>
#include <HUDs/LevelHUD.h>
#include "Components/CanvasPanel.h"
#include "Utilities/WidgetHelper.h"
#include <NiagaraFunctionLibrary.h>
#include "Actors/StarFountainEffect.h"
#include <Subsystems/EmailScoreSubsystem.h>

AScoreProp::AScoreProp()
{
	GetIntHealthComponent()->OnHealthChangeDelegate.AddDynamic(this, &AScoreProp::PlayDamagedEffect);

	PropScores.Add(EPropType::OriginalRamyun, 1000);
	PropScores.Add(EPropType::ShrimpRamyun, 1000);
	PropScores.Add(EPropType::SpicyRamyun, 1000);
	PropScores.Add(EPropType::Tangerine, 1000);
	PropScores.Add(EPropType::Topokki, 1000);
}

void AScoreProp::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AScoreProp::IncreasePropCount()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	check(PC)

	ABattlePlayerState* BattlePlayerState = PC->GetPlayerState<ABattlePlayerState>();
	check(BattlePlayerState)

	BattlePlayerState->IncreasePropCount(GetPropType());
}

void AScoreProp::CreatePropIconWidget()
{
	if (!PropIconWidgetClass) return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	check(PC);

	ALevelHUD* LevelHUD = Cast<ALevelHUD>(PC->GetHUD());
	check(LevelHUD);

	UBattleWidget* BattleWidget = Cast<UBattleWidget>(LevelHUD->GetLevelWidget());
	check(BattleWidget);

	PropIconWidget = CreateWidget<UPropIconWidget>(GetWorld(), PropIconWidgetClass);
	check(PropIconWidget);

	FVector2D PropIconPos;

	if (bool bSuccess = UWidgetHelper::GetActorCanvasLocalPosition(GetWorld(), this, BattleWidget->GetCanvasPannel(), PropIconPos))
	{
		PropIconWidget->Init(GetPropType(), PropIconPos);
	}

	BattleWidget->GetCanvasPannel()->AddChild(PropIconWidget);
}

void AScoreProp::OnHealthZero()
{
	Super::OnHealthZero();

	UEmailScoreSubsystem* EmailScoreSubsystem = GetGameInstance()->GetSubsystem<UEmailScoreSubsystem>();
	check(EmailScoreSubsystem);

	EmailScoreSubsystem->AddLastScore(PropScores[GetPropType()]);

	IncreasePropCount();
	CreatePropIconWidget();
}

void AScoreProp::PlayDamagedEffect(int32 NewCurrentHealth, int32 NewMaxHealth)
{
	if (!StarFountainEffectClass) return;

	UWorld* World = GetWorld();
	if (!World) return;

	FTransform EffectTransform = GetActorTransform();
	FVector EffectLocation = EffectTransform.GetLocation();
	EffectTransform.SetLocation(FVector(EffectLocation.X - 30.0f, EffectLocation.Y, EffectLocation.Z));

	AStarFountainEffect* StarFountainEffect = World->SpawnActor<AStarFountainEffect>(StarFountainEffectClass, EffectTransform);
	check(StarFountainEffect);

	StarFountainEffect->SetActorRotation(FRotator());
}