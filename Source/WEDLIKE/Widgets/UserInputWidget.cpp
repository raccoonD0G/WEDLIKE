// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/UserInputWidget.h"
#include <WEDLikeGameInstance.h>
#include "Components/EditableText.h"
#include "Components/TextBlock.h"
#include "Internationalization/Regex.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerInput.h"
#include "Components/CheckBox.h"

void UUserInputWidget::NativeConstruct()
{
	Super::NativeConstruct();

	bIsFocusable = true;

	EmailErrorText->SetVisibility(ESlateVisibility::Hidden);
	EmailErrorImage->SetVisibility(ESlateVisibility::Hidden);

	AgreeErrorText->SetVisibility(ESlateVisibility::Hidden);
	AgreeErrorImage->SetVisibility(ESlateVisibility::Hidden);

	if (EmailInputEditableText)
	{
		EmailInputEditableText->OnTextCommitted.AddDynamic(this, &UUserInputWidget::OnEmailCommitted);
	}

	if (SaveButton)
	{
		SaveButton->OnClicked.AddDynamic(this, &UUserInputWidget::OnSaveButtonClicked);
	}

	if (DelButton)
	{
		DelButton->OnClicked.AddDynamic(this, &UUserInputWidget::OnDelButtonClicked);
	}

	if (AgreeCheckBox)
	{
		AgreeCheckBox->OnCheckStateChanged.AddDynamic(this, &UUserInputWidget::OnCheckBoxToggled);
	}

	UWEDLikeGameInstance* WEDLikeGameInstance = Cast<UWEDLikeGameInstance>(GetGameInstance());
	check(WEDLikeGameInstance);

	WEDLikeGameInstance->OnNextPlayerEmailChangedDelegate.AddDynamic(this, &UUserInputWidget::SetNextPlayerText);
	WEDLikeGameInstance->OnGameStartNotReadyDelegate.AddDynamic(this, &UUserInputWidget::PlayErrorAnimation);
	WEDLikeGameInstance->OnIsAgreedChangedDelegate.AddDynamic(this, &UUserInputWidget::SetAgreeCheckBox);
}

FReply UUserInputWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::SpaceBar)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PC)
		{
			FInputKeyParams KeyParams(InKeyEvent.GetKey(), EInputEvent::IE_Pressed, 1.0);
			PC->InputKey(KeyParams);
			return FReply::Handled();
		}
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

FReply UUserInputWidget::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::SpaceBar)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PC)
		{
			FInputKeyParams KeyParams(InKeyEvent.GetKey(), EInputEvent::IE_Released, 0.0);
			PC->InputKey(KeyParams);
			return FReply::Handled();
		}
	}

	return Super::NativeOnKeyUp(InGeometry, InKeyEvent);
}

bool UUserInputWidget::IsValidEmailFormat(const FString& EmailString)
{
	const FString EmailRegex = TEXT("^.+@.+\\..{1,}$");

	FRegexPattern Pattern(EmailRegex);
	FRegexMatcher Matcher(Pattern, EmailString);

	return Matcher.FindNext();
}

void UUserInputWidget::OnEmailCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	FString InputString = Text.ToString();

	if (IsValidEmailFormat(InputString))
	{
		EmailErrorText->SetVisibility(ESlateVisibility::Hidden);
		EmailErrorImage->SetVisibility(ESlateVisibility::Hidden);

		SaveEmail(Text);
	}
	else
	{
		PlayEmailErrorAnimation();
	}

	if (CommitMethod == ETextCommit::OnEnter)
	{
		EmailInputEditableText->SetKeyboardFocus();
	}
}

void UUserInputWidget::OnSaveButtonClicked()
{
	SaveEmail(EmailInputEditableText->GetText());
}

void UUserInputWidget::OnDelButtonClicked()
{
	ClearEmail();
}

void UUserInputWidget::SaveEmail(const FText& InText)
{
	UWEDLikeGameInstance* WEDLikeGameInstance = Cast<UWEDLikeGameInstance>(GetGameInstance());
	check(WEDLikeGameInstance);

	WEDLikeGameInstance->SetNextPlayerEmail(InText.ToString());
}

void UUserInputWidget::ClearEmail()
{
	UWEDLikeGameInstance* WEDLikeGameInstance = Cast<UWEDLikeGameInstance>(GetGameInstance());
	check(WEDLikeGameInstance);

	WEDLikeGameInstance->ClearNextPlayerEmail();
}

void UUserInputWidget::SetNextPlayerText(const FString& InString)
{
	NextPlayerText->SetText(FText::FromString(InString));
}

void UUserInputWidget::OnCheckBoxToggled(bool InIsAgreed)
{
	UWEDLikeGameInstance* WEDLikeGameInstance = Cast<UWEDLikeGameInstance>(GetGameInstance());
	check(WEDLikeGameInstance);

	WEDLikeGameInstance->SetIsAgreed(InIsAgreed);

	if (InIsAgreed)
	{
		AgreeErrorText->SetVisibility(ESlateVisibility::Hidden);
		AgreeErrorImage->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UUserInputWidget::PlayAgreeErrorAnimation()
{
	AgreeErrorText->SetVisibility(ESlateVisibility::Visible);
	AgreeErrorImage->SetVisibility(ESlateVisibility::Visible);

	PlayAnimation(Anim_AgreeError);
}

void UUserInputWidget::PlayEmailErrorAnimation()
{
	EmailErrorText->SetVisibility(ESlateVisibility::Visible);
	EmailErrorImage->SetVisibility(ESlateVisibility::Visible);

	PlayAnimation(Anim_EmailError);
}

void UUserInputWidget::PlayErrorAnimation()
{
	UWEDLikeGameInstance* WEDLikeGameInstance = Cast<UWEDLikeGameInstance>(GetGameInstance());
	check(WEDLikeGameInstance);

	if (!(WEDLikeGameInstance->IsValidEmailFormat(WEDLikeGameInstance->GetNextPlayerEmail())))
	{
		PlayEmailErrorAnimation();
		return;
	}

	if (!(WEDLikeGameInstance->GetIsAgreed()))
	{
		PlayAgreeErrorAnimation();
		return;
	}
}

void UUserInputWidget::SetAgreeCheckBox(bool InIsAgree)
{
	if (InIsAgree)
	{
		AgreeCheckBox->SetCheckedState(ECheckBoxState::Checked);
	}
	else
	{
		AgreeCheckBox->SetCheckedState(ECheckBoxState::Unchecked);
	}
}