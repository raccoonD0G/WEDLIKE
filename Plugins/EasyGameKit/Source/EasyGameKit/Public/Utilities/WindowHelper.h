// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WindowHelper.generated.h"

/**
 * 
 */
UCLASS()
class EASYGAMEKIT_API UWindowHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    static class UWindowHandle* OpenNewWindow(FString WindowTitle, int32 Width = 800, int32 Height = 600);
};
