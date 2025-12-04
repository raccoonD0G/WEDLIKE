// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/UserIDInterface.h"
#include "PlayerCountSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class EASYGAMEKIT_API UPlayerCountSubsystem : public UGameInstanceSubsystem, public IUserIDInterface
{
	GENERATED_BODY()
	
protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

// Player Count Section
public:
	FORCEINLINE int32 GetPlayerCount() const { return PlayerCount; }

	UFUNCTION(BlueprintCallable)
	void IncrementPlayerCount();

	void SavePlayerCount();

protected:
	virtual FString GetUserID() const override;

private:
	const FString PlayerCountSaveSlot = TEXT("PlayerCountSaveSlot");
	const uint32 UserIndex = 0;

	int32 PlayerCount = 0;

	UPROPERTY()
	class UPlayerCountSaveGame* PlayerCountSaveGame;
};
