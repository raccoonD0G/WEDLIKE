// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/TimerComponent.h"


UTimerComponent::UTimerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTimerComponent::StartTimer()
{
	CurrentTime = StartTime;

	UWorld* World = GetWorld();
	if (!World) return;

	World->GetTimerManager().SetTimer(TimerHandle, this, &UTimerComponent::DecreaseCurrentTime, 1.0f, true);
}

void UTimerComponent::DecreaseCurrentTime()
{
	CurrentTime--;
	OnTimerUpdateDelegate.Broadcast(CurrentTime);

	if (CurrentTime <= 0)
	{
		UWorld* World = GetWorld();
		if (!World) return;

		World->GetTimerManager().ClearTimer(TimerHandle);

		OnTimerEndDelegate.Broadcast();
	}
}
