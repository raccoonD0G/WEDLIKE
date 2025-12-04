// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "WindowHandle.generated.h"

/**
 * 
 */
UCLASS()
class EASYGAMEKIT_API UWindowHandle : public UObject
{
	GENERATED_BODY()
	
protected:
	virtual void BeginDestroy() override;

public:
	UFUNCTION(BlueprintCallable)
	void SetWindowContent(UUserWidget* WidgetToDisplay);

	TWeakPtr<SWindow> WindowRef;

};