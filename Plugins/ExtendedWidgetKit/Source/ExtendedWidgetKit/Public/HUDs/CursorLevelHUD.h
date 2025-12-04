// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HUDs/LevelHUD.h"
#include "CursorLevelHUD.generated.h"

/**
 * 
 */
UCLASS()
class EXTENDEDWIDGETKIT_API ACursorLevelHUD : public ALevelHUD
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	
};
