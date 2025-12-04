// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WidgetHelper.generated.h"

/**
 * 
 */
UCLASS()
class EASYGAMEKIT_API UWidgetHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static bool GetActorCanvasLocalPosition(const UObject* WorldContextObject, AActor* TargetActor, class UCanvasPanel* ReferenceCanvas, FVector2D& OutLocalPos);
};
