// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Enums/PropType.h"
#include "BattlePlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPropCountChangeDelegate, EPropType, PropType, int32, NewPropCount);

/**
 * 
 */
UCLASS()
class WEDLIKE_API ABattlePlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	ABattlePlayerState();
	
protected:
	virtual void BeginPlay() override;

// Prop Count Section
public:
	int32 GetPropCount(EPropType PropType) const;

	void IncreasePropCount(EPropType PropType);

	FOnPropCountChangeDelegate OnPropCountChangeDelegate;

private:
	UPROPERTY()
	TObjectPtr<class UEmailScoreSubsystem> EmailScoreSubsystem;

	UPROPERTY(VisibleAnywhere)
	TMap<EPropType, int32> PropCounts;

	UPROPERTY(VisibleAnywhere)
	TMap<EPropType, int32> PropScores;

	UPROPERTY(EditAnywhere)
	int32 PlayVideoCount = 10;

// Widget Section
private:
	void PlayVideoByType(EPropType PropType);
};
