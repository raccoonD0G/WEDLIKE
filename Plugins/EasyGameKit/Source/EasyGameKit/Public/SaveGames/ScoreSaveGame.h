// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "ScoreSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FScoreData
{
	GENERATED_BODY()

public:
	UPROPERTY()
	int32 Score = 0;

	UPROPERTY()
	FString UserID;
};
/**
 * 
 */
UCLASS()
class EASYGAMEKIT_API UScoreSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	FORCEINLINE void AddScore(const FScoreData& NewScore) { Scores.Add(NewScore); }

	const TArray<FScoreData>& GetScores();

	FORCEINLINE const FScoreData& GetHighScore() { return Scores.Num() > 0 ? Scores[0] : EmptyScore; }

private:
	UPROPERTY()
	TArray<FScoreData> Scores;

	FScoreData EmptyScore;

	void SortScoresDescending();
};
