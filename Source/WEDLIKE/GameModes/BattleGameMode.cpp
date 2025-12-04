// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/BattleGameMode.h"
#include <Subsystems/EmailScoreSubsystem.h>
#include <WEDLikeGameInstance.h>
#include <GameStates/BattleGameState.h>
#include <Widgets/BattleWidget.h>
#include <HUDs/LevelHUD.h>
#include "Subsystems/RecordingSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TimerComponent.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

void ABattleGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	BindGameStageEvents();

	UWEDLikeGameInstance* WEDLikeGameInstance = Cast<UWEDLikeGameInstance>(GetGameInstance());
	check(WEDLikeGameInstance);

	WEDLikeGameInstance->SetOnBattleEmail();
	CurrentEmail = WEDLikeGameInstance->GetOnBattleEmail();
	
	WEDLikeGameInstance->OnBattleMapEntered();

	UEmailScoreSubsystem* EmailScoreSubsystem = GetGameInstance()->GetSubsystem<UEmailScoreSubsystem>();
	check(EmailScoreSubsystem);
}


void ABattleGameMode::StartGame()
{
	UEmailScoreSubsystem* EmailScoreSubsystem = GetGameInstance()->GetSubsystem<UEmailScoreSubsystem>();
	EmailScoreSubsystem->ResetLastScore();

	URecordingSubsystem* RecordingSubsystem = GetGameInstance()->GetSubsystem<URecordingSubsystem>();
	RecordingSubsystem->StartRecording();

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	check(PC);

	ALevelHUD* LevelHUD = Cast<ALevelHUD>(PC->GetHUD());
	check(LevelHUD);

	UBattleWidget* BattleWidget = Cast<UBattleWidget>(LevelHUD->GetLevelWidget());
	check(BattleWidget);

	BattleWidget->PlayStartWidget();
}

void ABattleGameMode::EndGame()
{
	URecordingSubsystem* RecordingSubsystem = GetGameInstance()->GetSubsystem<URecordingSubsystem>();
	RecordingSubsystem->StopRecording();

	SaveEmailCSV();

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	check(PC);

	ALevelHUD* LevelHUD = Cast<ALevelHUD>(PC->GetHUD());
	check(LevelHUD);

	UBattleWidget* BattleWidget = Cast<UBattleWidget>(LevelHUD->GetLevelWidget());
	check(BattleWidget);

	BattleWidget->PlayGameOverWidget();
}

void ABattleGameMode::BindGameStageEvents()
{
	ABattleGameState* BattleGameState = GetWorld()->GetGameState<ABattleGameState>();

	if (!BattleGameState)
	{
		GetWorld()->GetTimerManager().SetTimer(
			BindGameStageTimerHandle,
			this,
			&ABattleGameMode::BindGameStageEvents,
			0.1f,
			false
		);
		return;
	}

	BattleGameState->GetWaitingTimerComponent()->OnTimerEndDelegate.AddDynamic(this, &ABattleGameMode::StartGame);
	BattleGameState->GetBattleTimerComponent()->OnTimerEndDelegate.AddDynamic(this, &ABattleGameMode::EndGame);
}

void ABattleGameMode::SaveEmailCSV()
{
	UWEDLikeGameInstance* WEDLikeGameInstance = Cast<UWEDLikeGameInstance>(GetGameInstance());
	check(WEDLikeGameInstance);

	FString Email = WEDLikeGameInstance->GetOnBattleEmail();

	if (Email.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Email is empty. Skip saving to CSV."));
		return;
	}

	FString SaveDirectory = FPaths::ProjectSavedDir();
	FString FileName = TEXT("WEDLikeEmails.csv");
	FString AbsoluteFilePath = FPaths::ConvertRelativePathToFull(FPaths::Combine(SaveDirectory, FileName));

	FString CsvRow = Email + LINE_TERMINATOR;

	bool bSuccess = FFileHelper::SaveStringToFile(
		CsvRow,
		*AbsoluteFilePath,
		FFileHelper::EEncodingOptions::ForceUTF8,
		&IFileManager::Get(),
		EFileWrite::FILEWRITE_Append 
	);

	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("Saved Email to CSV: %s"), *AbsoluteFilePath);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to save CSV!"));
	}
}
