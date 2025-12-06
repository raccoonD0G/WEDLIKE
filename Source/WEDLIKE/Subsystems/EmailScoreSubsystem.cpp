// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/EmailScoreSubsystem.h"
#include "Interfaces/UserIDInterface.h"
#include <Kismet/GameplayStatics.h>

void UEmailScoreSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UserIDInterface = Cast<IUserIDInterface>(GetGameInstance());

	check(UserIDInterface);
}

void UEmailScoreSubsystem::CreateScoreSave()
{
	Super::CreateScoreSave();
    CommitDummyData();
}

void UEmailScoreSubsystem::CommitDummyData()
{
    for (int32 Index = 0; Index < 9; Index++)
    {
        FScoreData NewScore;
        NewScore.Score = 5000 * Index;
        NewScore.UserID = TEXT("---");

        ScoreSaveGame->AddScore(NewScore);
    }

    UGameplayStatics::SaveGameToSlot(ScoreSaveGame, ScoreSaveSlot, UserIndex);
}