// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ResultPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class WEDLIKE_API AResultPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_ResultSpace = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext* IMC_Result = nullptr;
	
	void OnSpacePressed();
};
