// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/DelayedTextBlock.h"


void UDelayedTextBlock::SetNumberDelayed(int32 InNumber, float Duration)
{
    CurrentNumber = InNumber;

    if (GetWorld()->GetTimerManager().IsTimerActive(NumberTimerHandle))
    {
        GetWorld()->GetTimerManager().ClearTimer(NumberTimerHandle);
    }

    TargetNumber = InNumber;
    AnimationDuration = Duration;
    AnimationTimeElapsed = 0.0f;

    SetText(FText::AsNumber(0));

    GetWorld()->GetTimerManager().SetTimer(
        NumberTimerHandle,
        this,
        &UDelayedTextBlock::UpdateNumberAnimation,
        TimerInterval,
        true
    );
}

void UDelayedTextBlock::UpdateNumberAnimation()
{
    AnimationTimeElapsed += TimerInterval;

    float Alpha = AnimationTimeElapsed / AnimationDuration;
    Alpha = FMath::Clamp(Alpha, 0.0f, 1.0f);

    int32 CurrentDisplayScore = (int32)FMath::Lerp(0.0f, (float)TargetNumber, Alpha);

    SetText(FText::AsNumber(CurrentDisplayScore));

    if (Alpha >= 1.0f)
    {
        GetWorld()->GetTimerManager().ClearTimer(NumberTimerHandle);

        SetText(FText::AsNumber(TargetNumber));
    }
}
