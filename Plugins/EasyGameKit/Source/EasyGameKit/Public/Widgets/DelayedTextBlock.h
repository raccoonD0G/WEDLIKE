// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TextBlock.h"
#include "DelayedTextBlock.generated.h"

/**
 * 
 */
UCLASS()
class EASYGAMEKIT_API UDelayedTextBlock : public UTextBlock
{
	GENERATED_BODY()
	
public:
	void SetNumberDelayed(int32 InNumber, float Duration);
	
private:
	UPROPERTY(VisibleAnywhere)
	int32 CurrentNumber;

	FTimerHandle NumberTimerHandle;
	int32 TargetNumber = 0;
	float AnimationDuration = 0.0f;
	float AnimationTimeElapsed = 0.0f;
	const float TimerInterval = 0.033f;

	// 타이머에 의해 주기적으로 호출될 함수
	void UpdateNumberAnimation();
};
