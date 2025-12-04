// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "ChromSave.generated.h"

/**
 * 
 */



UCLASS()
class VISIONMOTIONKIT_API UChromSave : public USaveGame
{
	GENERATED_BODY()
	
public:
	static const FString ChromSlot;

public:
	UPROPERTY(BlueprintReadWrite)
	FLinearColor SetKeyColor = FLinearColor::Green;

	UPROPERTY(BlueprintReadWrite)
	float LumaMask = 0.0f;

	UPROPERTY(BlueprintReadWrite)
	float AlphaMin = 0.0f;

	UPROPERTY(BlueprintReadWrite)
	float AlphaDivisor = 0.15f;

	UPROPERTY(BlueprintReadWrite)
	float IntensityFloor = 0.0f;

	UPROPERTY(BlueprintReadWrite)
	float Brightness = 1.0f;

	UPROPERTY(BlueprintReadWrite)
	float Contrast = 1.0f;

	UPROPERTY(BlueprintReadWrite)
	float Saturation = 0.0f;

	UPROPERTY(BlueprintReadWrite)
	float HueShift = 0.0f;
};
