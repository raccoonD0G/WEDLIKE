// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class EASYGAMEKIT_API UHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	
// Healt Bar Section
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UProgressBar> HealthProgressBar;

	void SetHealthProgressBar(float CurrentHealth, float MaxHealth);
	
	UFUNCTION()
	void SetHealthProgressBarInt(int32 CurrentHealth, int32 MaxHealth);
};
