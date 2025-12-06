// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/ScoreSubsystem.h"
#include "EmailScoreSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class WEDLIKE_API UEmailScoreSubsystem : public UScoreSubsystem
{
	GENERATED_BODY()
	
protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void CreateScoreSave() override;

	void CommitDummyData();
};
