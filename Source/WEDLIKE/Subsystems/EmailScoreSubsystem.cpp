// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/EmailScoreSubsystem.h"
#include "Interfaces/UserIDInterface.h"

void UEmailScoreSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UserIDInterface = Cast<IUserIDInterface>(GetGameInstance());

	check(UserIDInterface);
}