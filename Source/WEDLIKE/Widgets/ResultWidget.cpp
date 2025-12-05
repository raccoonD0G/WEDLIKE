// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ResultWidget.h"
#include <SaveGames/ScoreSaveGame.h>
#include <Subsystems/EmailScoreSubsystem.h>
#include "Widgets/RankingBoardWidget.h"
#include "Widgets/DelayedTextBlock.h"
#include <WEDLikeGameInstance.h>
#include <Kismet/GameplayStatics.h>
#include "Components/AudioComponent.h"

void UResultWidget::NativeConstruct()
{
    Super::NativeConstruct();

    UEmailScoreSubsystem* EmailScoreSubsystem = GetGameInstance()->GetSubsystem<UEmailScoreSubsystem>();
	check(EmailScoreSubsystem);

    const int32 LastScore = EmailScoreSubsystem->GetLastScore();
    const TArray<FScoreData>& Scores = EmailScoreSubsystem->GetScores();

    int32 Count = FMath::Min(9, Scores.Num());
    bool bNewRecord = Scores.Num() < 9 || Scores[8].Score < LastScore;

    if (bNewRecord)
    {
        if (NewRecordSound)
        {
            UAudioComponent* AudioComponent = UGameplayStatics::CreateSound2D(this, NewRecordSound);
            if (AudioComponent)
            {
                AudioComponent->Play();
            }
        }
    }

    int32 HighScore = 0;
    if (Scores.IsEmpty())
    {
        HighScore = LastScore;
    }
    else
    {
        HighScore = LastScore > Scores[0].Score ? LastScore : Scores[0].Score;
    }

    if (bNewRecord && Scores.Num() >= 9 && Count != 0)
    {
        --Count;
    }

    for (int32 Index = 0; Index < Count; ++Index)
    {
        float ScoreDuration = 0.0f;
        if (HighScore != 0)
        {
            ScoreDuration = MaxScoreDuration * (float)(Scores[Index].Score) / (float)HighScore;
        }
        RankingBoardWidget->AddRankingRowDirect(Scores[Index].UserID, Scores[Index].Score, ScoreDuration);
    }

    UWEDLikeGameInstance* WEDLikeGameInstance = Cast<UWEDLikeGameInstance>(GetGameInstance());
    check(WEDLikeGameInstance);

    if (bNewRecord)
    {
        float ScoreDuration = 0.0f;
        if (HighScore != 0)
        {
            ScoreDuration = MaxScoreDuration * (float)LastScore / (float)HighScore;
        }
        FString PlayerName = WEDLikeGameInstance->GetUserID();
        URankingRowWidget* RankingRowWidget = RankingBoardWidget->AddRankingRowAnimated(PlayerName, LastScore, ScoreDuration);
		int32 NewRecordIndex = -1;
		RankingBoardWidget->GetIndexOfRow(RankingRowWidget, NewRecordIndex);
		RankingBoardWidget->HighlightRow(NewRecordIndex);
    }

    EmailScoreSubsystem->CommitScoreAndSave();

    MyScoreText->SetNumberDelayed(LastScore, 1.0f);
}