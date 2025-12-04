// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "PlayerCountSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class EASYGAMEKIT_API UPlayerCountSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	int32 PlayerCount;
};
