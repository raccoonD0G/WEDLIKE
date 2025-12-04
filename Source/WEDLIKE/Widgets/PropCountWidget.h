// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Enums/PropType.h"
#include "PropCountWidget.generated.h"

/**
 * 
 */
UCLASS()
class WEDLIKE_API UPropCountWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintImplementableEvent)
	void PlayPropCountChangeAnimation();

private:
	UPROPERTY(EditAnywhere)
	EPropType CountPropType;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UTexture2D> ItemTexture;

	UPROPERTY(EditAnywhere)
	int32 CountFontSize;

	void RefreshWidget();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> ItemImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> CountText;

	UFUNCTION()
	void SetCountText(int32 InCount);

	UFUNCTION()
	void OnPropCountChange(EPropType PropType, int32 NewPropCount);
};
