// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TimerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimerUpdateDelegate, int32, CurrentTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTimerEndDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class EASYGAMEKIT_API UTimerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTimerComponent();

// Timer Section
public:
	FOnTimerUpdateDelegate OnTimerUpdateDelegate;
	FOnTimerEndDelegate OnTimerEndDelegate;

	FORCEINLINE int32 GetCurrentTime() const { return CurrentTime; }
	FORCEINLINE void SetStartTime(int32 NewStartTime) { StartTime = NewStartTime; }

	void StartTimer();

private:
	UPROPERTY(EditAnywhere)
	int32 StartTime = 60;

	UPROPERTY(EditAnywhere)
	int32 CurrentTime;

	void DecreaseCurrentTime();
	
	FTimerHandle TimerHandle;
};
