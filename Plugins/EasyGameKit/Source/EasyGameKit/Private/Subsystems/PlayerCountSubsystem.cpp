// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/PlayerCountSubsystem.h"
#include "SaveGames/PlayerCountSaveGame.h"
#include "Kismet/GameplayStatics.h"

void UPlayerCountSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

    if (!UGameplayStatics::DoesSaveGameExist(PlayerCountSaveSlot, UserIndex))
    {
        USaveGame* SaveGameObject = UGameplayStatics::CreateSaveGameObject(UPlayerCountSaveGame::StaticClass());
		UGameplayStatics::SaveGameToSlot(SaveGameObject, PlayerCountSaveSlot, UserIndex);
        PlayerCountSaveGame = Cast<UPlayerCountSaveGame>(SaveGameObject);
		PlayerCount = 0;
	}
    else
    {
        PlayerCountSaveGame = Cast<UPlayerCountSaveGame>(UGameplayStatics::LoadGameFromSlot(PlayerCountSaveSlot, UserIndex));

        if (PlayerCountSaveGame)
        {
            PlayerCount = PlayerCountSaveGame->PlayerCount;
        }
    }
}

void UPlayerCountSubsystem::IncrementPlayerCount()
{
    PlayerCount++;
    SavePlayerCount();
}

FString UPlayerCountSubsystem::GetUserID() const
{
    return FString::FromInt(PlayerCount);
}

void UPlayerCountSubsystem::SavePlayerCount()
{
    PlayerCountSaveGame->PlayerCount = PlayerCount;
    UGameplayStatics::SaveGameToSlot(PlayerCountSaveGame, PlayerCountSaveSlot, UserIndex);
}