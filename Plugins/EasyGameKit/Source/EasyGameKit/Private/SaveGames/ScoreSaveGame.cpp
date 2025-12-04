// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveGames/ScoreSaveGame.h"

const TArray<FScoreData>& UScoreSaveGame::GetScores()
{
	SortScoresDescending();
	return Scores;
}

void UScoreSaveGame::SortScoresDescending()
{
	Scores.Sort([](const FScoreData& A, const FScoreData& B)
		{
			return A.Score > B.Score;
		}
	);
}
