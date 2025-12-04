// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/ScoreSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGames/ScoreSaveGame.h"
#include "Interfaces/UserIDInterface.h"

void UScoreSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    if (!UGameplayStatics::DoesSaveGameExist(ScoreSaveSlot, UserIndex))
    {
        USaveGame* SaveGameObject = UGameplayStatics::CreateSaveGameObject(UScoreSaveGame::StaticClass());
        UGameplayStatics::SaveGameToSlot(SaveGameObject, ScoreSaveSlot, UserIndex);
		ScoreSaveGame = Cast<UScoreSaveGame>(SaveGameObject);
    }
    else
    {
        ScoreSaveGame = Cast<UScoreSaveGame>(UGameplayStatics::LoadGameFromSlot(ScoreSaveSlot, UserIndex));
    }

    HighScore = ScoreSaveGame->GetHighScore().Score;
}


void UScoreSubsystem::ResetLastScore()
{
    LastScore = 0;
    OnLastScoreChanged.Broadcast(LastScore);
}

void UScoreSubsystem::AddLastScore(int32 Delta)
{
    LastScore += Delta;
	OnLastScoreChanged.Broadcast(LastScore);

    if (LastScore > HighScore)
    {
        HighScore = LastScore;
		OnHighScoreChanged.Broadcast(HighScore);
    }
}

void UScoreSubsystem::SubtractLastScore(int32 Delta)
{
    LastScore -= Delta;
    if (LastScore < 0)
    {
        LastScore = 0;
    }

    if (ScoreSaveGame->GetHighScore().Score > LastScore)
    {
        HighScore = ScoreSaveGame->GetHighScore().Score;
    }

    OnLastScoreChanged.Broadcast(LastScore);
}

void UScoreSubsystem::CommitScoreAndSave()
{
    if (!UserIDInterface) return;

    FScoreData NewScore;
    NewScore.Score = LastScore;
    NewScore.UserID = UserIDInterface->GetUserID();

    ScoreSaveGame->AddScore(NewScore);

    UGameplayStatics::SaveGameToSlot(ScoreSaveGame, ScoreSaveSlot, UserIndex);
}

const TArray<FScoreData>& UScoreSubsystem::GetScores()
{
    return ScoreSaveGame->GetScores();
}

int32 UScoreSubsystem::GetHighScore()
{
    return HighScore;
}

