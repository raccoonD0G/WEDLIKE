// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UserInputWidget.generated.h"

/**
 * 
 */
UCLASS()
class WEDLIKE_API UUserInputWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UEditableText> EmailInputEditableText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> EmailErrorText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> EmailErrorImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> AgreeErrorText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> AgreeErrorImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> NextPlayerText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCheckBox> AgreeCheckBox;

	UFUNCTION()
	void SetNextPlayerText(const FString& InString);

	bool IsValidEmailFormat(const FString& EmailString);

	UFUNCTION()
	void OnEmailCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* Anim_EmailError;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* Anim_AgreeError;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> SaveButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> DelButton;

	UFUNCTION()
	void OnSaveButtonClicked();

	UFUNCTION()
	void OnDelButtonClicked();

	void SaveEmail(const FText& InText);

	void ClearEmail();

	UFUNCTION()
	void OnCheckBoxToggled(bool InIsAgreed);

	void PlayAgreeErrorAnimation();

	void PlayEmailErrorAnimation();

	UFUNCTION()
	void PlayErrorAnimation();

	UFUNCTION()
	void SetAgreeCheckBox(bool InIsAgree);
};
