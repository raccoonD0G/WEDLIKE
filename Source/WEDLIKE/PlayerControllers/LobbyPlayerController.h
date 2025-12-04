// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class WEDLIKE_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_LobbySpace = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_LobbyOne = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_LobbyTwo = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_LobbyThree = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext* IMC_Lobby = nullptr;

private:
	void OnSpacePressed();
	void OnOnePressed();
	void OnTwoPressed();
	void OnThreePressed();

	void SetWindowed(int32 Width, int32 Height);

	void SetFullscreen(bool bTrueFullscreen);

	void OpenSetting();

};
