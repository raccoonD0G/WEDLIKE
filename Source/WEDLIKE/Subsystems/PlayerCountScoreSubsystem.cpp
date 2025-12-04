// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/PlayerCountScoreSubsystem.h"
#include "Interfaces/UserIDInterface.h"
#include "Subsystems/PlayerCountSubsystem.h"

void UPlayerCountScoreSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	UserIDInterface = GetGameInstance()->GetSubsystem<UPlayerCountSubsystem>();

	check(UserIDInterface);
}