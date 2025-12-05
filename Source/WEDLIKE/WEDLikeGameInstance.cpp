// Fill out your copyright notice in the Description page of Project Settings.


#include "WEDLikeGameInstance.h"
#include "Utilities/WindowHelper.h"
#include "Objects/WindowHandle.h"
#include "Widgets/UserInputWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

void UWEDLikeGameInstance::OnStart()
{
	Super::OnStart();

	WindowHandle = UWindowHelper::OpenNewWindow(TEXT("UserInputWindow"), 1920.0f / 2.0f, 1080.0f / 2.0f);
	check(WindowHandle);

	check(UserInputWidgetClass);
	UserInputWidget = CreateWidget<UUserInputWidget>(this, UserInputWidgetClass);

	WindowHandle->SetWindowContent(UserInputWidget);

	PlayBGM();
}

void UWEDLikeGameInstance::SetNextPlayerEmail(const FString& InEmail)
{
	NextPlayerEmail = InEmail;
	OnNextPlayerEmailChangedDelegate.Broadcast(InEmail);
}

void UWEDLikeGameInstance::ClearNextPlayerEmail()
{
	SetNextPlayerEmail(TEXT(""));
}

void UWEDLikeGameInstance::SetOnBattleEmail()
{
	OnBattleEmail = NextPlayerEmail;
	ClearNextPlayerEmail();
}

void UWEDLikeGameInstance::OnBattleMapEntered()
{
	SetIsAgreed(false);
}

bool UWEDLikeGameInstance::CheckGameStartReady()
{
	bool IsEmailReady = IsValidEmailFormat(NextPlayerEmail);
	bool IsAgreeReady = bIsAgreed;

	if (!IsEmailReady || !IsAgreeReady)
	{
		OnGameStartNotReadyDelegate.Broadcast();
	}

	return IsEmailReady && IsAgreeReady;
}

FString UWEDLikeGameInstance::GetUserID() const
{
	FString LeftPart;
	FString RightPart;

	if (OnBattleEmail.Split(TEXT("@"), &LeftPart, &RightPart))
	{
		return LeftPart;
	}
	
	return FString(TEXT(""));
}

FString UWEDLikeGameInstance::GetFileName() const
{
	return OnBattleEmail;
}

bool UWEDLikeGameInstance::IsValidEmailFormat(const FString& EmailString)
{
	const FString EmailRegex = TEXT("^.+@.+\\..{1,}$");

	FRegexPattern Pattern(EmailRegex);
	FRegexMatcher Matcher(Pattern, EmailString);

	return Matcher.FindNext();
}

void UWEDLikeGameInstance::SetIsAgreed(bool InIsAgreed)
{
	bIsAgreed = InIsAgreed;
	OnIsAgreedChangedDelegate.Broadcast(bIsAgreed);
}

void UWEDLikeGameInstance::PlayBGM()
{
    if (!BGMAsset) return;

    if (BGMComponent)
    {
        if (BGMComponent->Sound == BGMAsset && BGMComponent->IsPlaying())
        {
            return;
        }

        BGMComponent->Stop();
    }

    BGMComponent = UGameplayStatics::CreateSound2D(this, BGMAsset, 1.f, 1.f, 0.f, nullptr, true, false);

    if (BGMComponent)
    {
        BGMComponent->bAutoDestroy = false;

		BGMComponent->Play();
    }
}

void UWEDLikeGameInstance::StopBGM()
{
    if (BGMComponent && BGMComponent->IsPlaying())
    {
        BGMComponent->Stop();
    }
}