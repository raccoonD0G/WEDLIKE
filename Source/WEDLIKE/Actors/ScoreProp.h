// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/PhysicsProp.h"
#include "ScoreProp.generated.h"


UCLASS()
class WEDLIKE_API AScoreProp : public APhysicsProp
{
	GENERATED_BODY()
	
public:
	AScoreProp();

protected:
	virtual void PostInitializeComponents() override;

private:
	UPROPERTY()
	TObjectPtr<class UPropIconWidget> PropIconWidget;

	UFUNCTION()
	void IncreasePropCount();

	UFUNCTION()
	void CreatePropIconWidget();

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UPropIconWidget> PropIconWidgetClass;

// Health Section
protected:
	virtual void OnHealthZero() override;

// Effect Section
private:
	UFUNCTION()
	void PlayDamagedEffect(int32 NewCurrentHealth, int32 NewMaxHealth);

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AStarFountainEffect> StarFountainEffectClass;

// Score Section
private:
	UPROPERTY(EditAnywhere)
	TMap<EPropType, int32> PropScores;
};
