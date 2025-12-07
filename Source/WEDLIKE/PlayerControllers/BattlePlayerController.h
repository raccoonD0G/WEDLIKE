// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BattlePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class WEDLIKE_API ABattlePlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_BattleQ = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext* IMC_Battle = nullptr;

	void OnQPressed();
	
};
