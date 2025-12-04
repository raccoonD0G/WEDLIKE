// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Enums/PropType.h"
#include "PropIconWidget.generated.h"

/**
 * 
 */
UCLASS()
class WEDLIKE_API UPropIconWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void Init(EPropType InPropType, FVector2D InStartPos);

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> ItemImage;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UTexture2D> ItemTexture;

	UFUNCTION(BlueprintCallable)
	void TestFunction();

// Scale Section
private:
	UPROPERTY(EditAnywhere)
	float StartScale = 1.0f;

	UPROPERTY(EditAnywhere)
	float EndScale = 2.0f;

	UPROPERTY(VisibleAnywhere)
	float ScaleAnimElapsed = 0.0f;

	UPROPERTY(EditAnywhere)
	float ScaleAnimDuration = 1.0f;

	FTimerHandle ScaleTimerHandle;

	float OvershootScale;

	void StartScaleAnimation();

	void UpdateScaleAnimation();

	void SetImageScale(float Scale);

// Pos Secton
private:
	UPROPERTY(VisibleAnywhere)
	FVector2D StartPos;

	FVector2D LastPos;

	UPROPERTY(VisibleAnywhere)
	FVector2D EndPos;

	UPROPERTY(VisibleAnywhere)
	float PosAnimElapsed = 0.f;

	UPROPERTY(EditAnywhere)
	float PosAnimDuration = 1.5f;

	FTimerHandle PosTimerHandle;

	UPROPERTY()
	TObjectPtr<class UPropCountWidget> PropCountWidget;

	void StartPosAnimation();

	void UpdatePosAnimation();
};
