// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/BattleWidget.h"
#include "MediaPlayer.h"
#include "MediaSource.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GameStates/BattleGameState.h"
#include "Components/TimerComponent.h"
#include "Components/WidgetSwitcher.h"
#include <Subsystems/EmailScoreSubsystem.h>
#include <Kismet/GameplayStatics.h>
#include "Components/AudioComponent.h"

void UBattleWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (VideoPlayer)
	{
		VideoPlayer->OnEndReached.AddDynamic(this, &UBattleWidget::HandleVideoFinished);
	}

	UEmailScoreSubsystem* EmailScoreSubsystem = GetGameInstance()->GetSubsystem<UEmailScoreSubsystem>();
	if (EmailScoreSubsystem)
	{
		UpdateLastScoreTexts(EmailScoreSubsystem->GetLastScore());
		UpdateHighScoreText(EmailScoreSubsystem->GetHighScore());

		EmailScoreSubsystem->OnLastScoreChanged.AddDynamic(this, &UBattleWidget::UpdateLastScoreTexts);
		EmailScoreSubsystem->OnHighScoreChanged.AddDynamic(this, &UBattleWidget::UpdateHighScoreText);
	}

	UWorld* World = GetWorld();
	if (!World) return;

	ABattleGameState* BattleGameState = World->GetGameState<ABattleGameState>();
	if (!BattleGameState) return;

	UTimerComponent* WaitingTimerComponent = BattleGameState->GetWaitingTimerComponent();
	if (!WaitingTimerComponent) return;

	WaitingTimerComponent->OnTimerUpdateDelegate.AddDynamic(this, &UBattleWidget::UpdateWaitingTimerText);
	UpdateWaitingTimerText(WaitingTimerComponent->GetCurrentTime());

	WaitingTimerComponent->OnTimerEndDelegate.AddDynamic(this, &UBattleWidget::HideWaitingTimerText);

	UTimerComponent* BattleTimerComponent = BattleGameState->GetBattleTimerComponent();
	if (!BattleTimerComponent) return;

	BattleTimerComponent->OnTimerUpdateDelegate.AddDynamic(this, &UBattleWidget::UpdateBattleTimerText);
	UpdateBattleTimerText(BattleTimerComponent->GetCurrentTime());

	HideStartWidget();
	HideGameOverWidget();
}

void UBattleWidget::PlayVideoByType(EPropType VideoType)
{
	if (!VideoPlayer) return;

	if (VideoPlayer->IsPlaying()) return;

	if (!(VideoSources.Find(VideoType))) return;

	UMediaSource* Source = VideoSources.Find(VideoType)->LoadSynchronous();
	if (!Source) return;

	VideoImage->SetVisibility(ESlateVisibility::Visible);

	const bool bWillOpen = VideoPlayer->OpenSource(Source);

	if (!(CheerSounds.IsEmpty()))
	{
		int32 SoundIndex = FMath::RandRange(0, CheerSounds.Num());

		UAudioComponent* AudioComponent = UGameplayStatics::CreateSound2D(this, CheerSounds[SoundIndex]);
		if (AudioComponent)
		{
			AudioComponent->Play();
		}
	}
}

void UBattleWidget::HandleVideoFinished()
{
	VideoPlayer->Close();
	if (VideoImage)
	{
		VideoImage->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UBattleWidget::UpdateLastScoreTexts(int32 NewLastScore)
{
	LastScoreText->SetText(FText::AsNumber(NewLastScore));
}

void UBattleWidget::UpdateHighScoreText(int32 NewHighScore)
{
	HighScoreText->SetText(FText::AsNumber(NewHighScore));
}

void UBattleWidget::UpdateBattleTimerText(int32 NewTime)
{
	BattleTimerText->SetText(FText::FromString(FString::Printf(TEXT("%d"), NewTime)));
}

void UBattleWidget::UpdateWaitingTimerText(int32 NewTime)
{
	WaitingTimerText->SetVisibility(ESlateVisibility::Visible);
	WaitingTimerText->SetText(FText::FromString(FString::FromInt(NewTime)));
}

void UBattleWidget::HideWaitingTimerText()
{
	WaitingTimerText->SetVisibility(ESlateVisibility::Hidden);
}

void UBattleWidget::ShowStartWidget()
{
	StartWidget->SetVisibility(ESlateVisibility::Visible);
}

void UBattleWidget::HideStartWidget()
{
	StartWidget->SetVisibility(ESlateVisibility::Hidden);
}

void UBattleWidget::ShowGameOverWidget()
{
	GameOverWidget->SetVisibility(ESlateVisibility::Visible);
}

void UBattleWidget::HideGameOverWidget()
{
	GameOverWidget->SetVisibility(ESlateVisibility::Hidden);
}

void UBattleWidget::PlayGameOverWidget()
{
	ShowGameOverWidget();
	GetWorld()->GetTimerManager().SetTimer(
		GameOverWidgetTimerHandle,
		this,
		&UBattleWidget::HideGameOverWidget,
		3.0f,
		false
	);
}

void UBattleWidget::PlayStartWidget()
{
	ShowStartWidget();
	GetWorld()->GetTimerManager().SetTimer(
		StartWidgetTimerHandle,
		this,
		&UBattleWidget::HideStartWidget,
		1.0f,
		false
	);
}