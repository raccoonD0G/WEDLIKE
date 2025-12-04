// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/GameProp.h"
#include "Enums/GameStage.h"
#include "PenaltyProp.generated.h"

/**
 * 
 */
UCLASS()
class WEDLIKE_API APenaltyProp : public AGameProp
{
	GENERATED_BODY()
	
public:
	APenaltyProp();

public:
	void Init(float InSpeed);

// Score Decrease Section
private:
	UPROPERTY(EditAnywhere)
	int32 ScoreDecreaseAmount = 700;

	UFUNCTION()
	void DecreaseScore();

// Movement Section
public:
	float GetFallingSpeed(EGameStage CurrentGameStage);

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UProjectileFallingComponent> ProjectileFallingComponent;

	UPROPERTY(EditAnywhere)
	TMap<EGameStage, float> GameStageToPropFallSpeed;
};
