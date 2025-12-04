// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ResultWidget.generated.h"

/**
 * 
 */
UCLASS()
class WEDLIKE_API UResultWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	class URankingBoardWidget* RankingBoardWidget;

	UPROPERTY(meta = (BindWidget))
	class UDelayedTextBlock* MyScoreText;

	const FString ScoreSaveSlot = TEXT("ScoreSaveSlot");
	const uint32 UserIndex = 0;

	UPROPERTY(EditAnywhere)
	float MaxScoreDuration = 2.0f;
};
