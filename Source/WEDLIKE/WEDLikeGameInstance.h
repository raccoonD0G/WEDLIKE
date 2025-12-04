// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/UserIDInterface.h"
#include "Interfaces/FileNameInterface.h"
#include "WEDLikeGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNextPlayerEmailChangedDelegate, const FString&, InEmail);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameStartNotReadyDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIsAgreedChangedDelegate, bool, InIsAgree);

/**
 * 
 */
UCLASS()
class WEDLIKE_API UWEDLikeGameInstance : public UGameInstance, public IUserIDInterface, public IFileNameInterface
{
	GENERATED_BODY()


protected:
	virtual void OnStart() override;
	
public:
	bool CheckGameStartReady();

	FOnGameStartNotReadyDelegate OnGameStartNotReadyDelegate;

// Widget Section
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UUserInputWidget> UserInputWidget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserInputWidget> UserInputWidgetClass;

// Window Section
public:
	FORCEINLINE class UWindowHandle* GetWindowHandle() { return WindowHandle; };

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UWindowHandle> WindowHandle;

public:
	virtual FString GetUserID() const override;
	virtual FString GetFileName() const override;

// Email Section
public:
	FORCEINLINE const FString& GetNextPlayerEmail() { return NextPlayerEmail; }
	FORCEINLINE const FString& GetOnBattleEmail() { return OnBattleEmail; }

	bool IsValidEmailFormat(const FString& EmailString);

	void SetNextPlayerEmail(const FString& InEmail);

	void SetOnBattleEmail();

	void ClearNextPlayerEmail();

	void OnBattleMapEntered();

	FOnNextPlayerEmailChangedDelegate OnNextPlayerEmailChangedDelegate;
	FOnIsAgreedChangedDelegate OnIsAgreedChangedDelegate;

private:
	FString NextPlayerEmail;

	FString OnBattleEmail;

// Toggle Section
public:
	FORCEINLINE bool GetIsAgreed() { return bIsAgreed; }
	void SetIsAgreed(bool InIsAgreed);

private:
	uint8 bIsAgreed : 1 = false;
};
