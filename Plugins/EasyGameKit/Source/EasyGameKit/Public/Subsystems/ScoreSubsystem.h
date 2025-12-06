// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include <SaveGames/ScoreSaveGame.h>
#include "ScoreSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLastScoreChangedDelegate, int32, NewLastScore);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHighScoreChangedDelegate, int32, NewHighScore);

/**
 * 
 */
UCLASS(Abstract)
class EASYGAMEKIT_API UScoreSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

protected:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

// Score Section
public:
    FOnLastScoreChangedDelegate OnLastScoreChanged;
	FOnHighScoreChangedDelegate OnHighScoreChanged;

    void ResetLastScore();

    void AddLastScore(int32 Delta);
	void SubtractLastScore(int32 Delta);

    FORCEINLINE int32 GetLastScore() const { return LastScore; }

    FORCEINLINE int32 GetHighScore() const { return HighScore; }

    void CommitScoreAndSave();

    const TArray<FScoreData>& GetScores();

	FScoreData EmptyScore;

    int32 GetHighScore();

protected:
    class IUserIDInterface* UserIDInterface = nullptr;

    virtual void CreateScoreSave();

    UPROPERTY()
    class UScoreSaveGame* ScoreSaveGame;

    const FString ScoreSaveSlot = TEXT("ScoreSaveSlot");
    const uint32 UserIndex = 0;

private:
    int32 LastScore = 0;
    int32 HighScore = 0;

    
};
