// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/RankingRowWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Slate/SlateBrushAsset.h"
#include "Components/CanvasPanelSlot.h"


void URankingRowWidget::NativePreConstruct()
{
    Super::NativePreConstruct();

    RefreshWidget();
}

void URankingRowWidget::NativeConstruct()
{
    Super::NativeConstruct();

    RefreshWidget();
}

void URankingRowWidget::Init(const FString& InPlayerName, int32 InScore, float Duration)
{
    SetPlayerName(InPlayerName);

    if (Duration < KINDA_SMALL_NUMBER)
    {
        SetScore(InScore);
    }
    else
    {
        SetScoreDelayed(InScore, Duration);
    }

    if (UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(Slot))
    {
        CanvasPanelSlot->SetAlignment(FVector2D(0.0f, 0.0f));
		CanvasPanelSlot->SetAnchors(RowAchors);
		CanvasPanelSlot->SetOffsets(FMargin(OffsetLeft, OffsetTop, OffsetRight, OffsetBottom));
	}
}

void URankingRowWidget::SetRankNumText(int32 InNum)
{
    if (RankNumText)
    {
        RankNumText->SetText(FText::AsNumber(InNum));
    }
}

void URankingRowWidget::SetRankIconImage(const FSlateBrush& Brush)
{
    if (RankIconImage)
    {
        RankIconImage->SetBrush(Brush);
    }
}

void URankingRowWidget::SetRankFrame(const FSlateBrush& Brush)
{
    if (RankFrameImage)
    {
        RankFrameImage->SetBrush(Brush);
    }
}

void URankingRowWidget::SetPlayerName(const FString& PlayerName)
{
	if (PlayerNameText)
	{
		PlayerNameText->SetText(FText::FromString(PlayerName));
	}
}

void URankingRowWidget::SetScore(int32 Score)
{
    CurrentScore = Score;

	if (ScoreText)
	{
		ScoreText->SetText(FText::AsNumber(Score));
	}
}

void URankingRowWidget::SetScoreDelayed(int32 Score, float Duration)
{
    CurrentScore = Score;

    if (GetWorld()->GetTimerManager().IsTimerActive(ScoreTimerHandle))
    {
        GetWorld()->GetTimerManager().ClearTimer(ScoreTimerHandle);
    }

    TargetScore = Score;
    AnimationDuration = Duration;
    AnimationTimeElapsed = 0.0f;

    if (ScoreText)
    {
        ScoreText->SetText(FText::AsNumber(0));
    }

    GetWorld()->GetTimerManager().SetTimer(
        ScoreTimerHandle,
        this,
        &URankingRowWidget::UpdateScoreAnimation,
        TimerInterval,
        true
    );
}

void URankingRowWidget::Highlight_Implementation()
{

}

void URankingRowWidget::RefreshWidget()
{
    if (bShowFrame)
    {
        if (RankFrameImage)
        {
            RankFrameImage->SetVisibility(ESlateVisibility::Visible);
        }
    }
    else
    {
        if (RankFrameImage)
        {
            RankFrameImage->SetVisibility(ESlateVisibility::Hidden);
        }
    }

    if (bShowRankIcon)
    {
        if (RankIconImage)
        {
            RankIconImage->SetVisibility(ESlateVisibility::Visible);
        }
    }
    else
    {
        if (RankIconImage)
        {
            RankIconImage->SetVisibility(ESlateVisibility::Hidden);
        }
    }

    if (RankIconImage)
    {
        RankIconImage->SetBrush(RankIcon);
    }
}

void URankingRowWidget::StartMoveTo(const FMargin& InOffset, float InDuration)
{
    MoveDuration = FMath::Max(InDuration, 0.01f);
    MoveElapsed = 0.f;
    MoveTargetOffset = InOffset;

    if (UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(Slot))
    {
        MoveStartOffset = CanvasPanelSlot->GetOffsets();
    }
    else
    {
        MoveStartOffset = FMargin();
    }

    GetWorld()->GetTimerManager().ClearTimer(MoveTimer);
    GetWorld()->GetTimerManager().SetTimer(
        MoveTimer,
        this,
        &URankingRowWidget::UpdateMove,
		0.033f,
        true
    );
}

void URankingRowWidget::UpdateMove()
{
    MoveElapsed += 0.033f;

    float Alpha = FMath::Clamp(MoveElapsed / MoveDuration, 0.f, 1.f);
    float SmoothAlpha = FMath::InterpEaseOut(0.f, 1.f, Alpha, 2.f);

    FMargin NewOffset;
    NewOffset.Bottom = FMath::Lerp(MoveStartOffset.Bottom, MoveTargetOffset.Bottom, SmoothAlpha);
	NewOffset.Top = FMath::Lerp(MoveStartOffset.Top, MoveTargetOffset.Top, SmoothAlpha);
	NewOffset.Left = FMath::Lerp(MoveStartOffset.Left, MoveTargetOffset.Left, SmoothAlpha);
	NewOffset.Right = FMath::Lerp(MoveStartOffset.Right, MoveTargetOffset.Right, SmoothAlpha);

    if (UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(Slot))
    {
        CanvasPanelSlot->SetOffsets(NewOffset);
    }

    if (Alpha >= 1.f)
    {
        if (UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(Slot))
        {
            CanvasPanelSlot->SetOffsets(MoveTargetOffset);
        }
        GetWorld()->GetTimerManager().ClearTimer(MoveTimer);
    }
}

void URankingRowWidget::SetShowRankIcon(bool bInShowRankIcon)
{
    bShowRankIcon = bInShowRankIcon;

    if (bShowRankIcon)
    {
        if (RankIconImage)
        {
            RankIconImage->SetVisibility(ESlateVisibility::Visible);
        }
    }
    else
    {
        if (RankIconImage)
        {
            RankIconImage->SetVisibility(ESlateVisibility::Hidden);
        }
    }
}

void URankingRowWidget::SetShowFrame(bool bInShowFrame)
{
    bShowFrame = bInShowFrame;

    if (bShowFrame)
    {
        if (RankFrameImage)
        {
            RankFrameImage->SetVisibility(ESlateVisibility::Visible);
        }
    }
    else
    {
        if (RankFrameImage)
        {
            RankFrameImage->SetVisibility(ESlateVisibility::Hidden);
        }
	}
}

void URankingRowWidget::UpdateScoreAnimation()
{
    AnimationTimeElapsed += TimerInterval;

    float Alpha = AnimationTimeElapsed / AnimationDuration;
    Alpha = FMath::Clamp(Alpha, 0.0f, 1.0f);

    int32 CurrentDisplayScore = (int32)FMath::Lerp(0.0f, (float)TargetScore, Alpha);

    if (ScoreText)
    {
        ScoreText->SetText(FText::AsNumber(CurrentDisplayScore));
    }

    if (Alpha >= 1.0f)
    {
        GetWorld()->GetTimerManager().ClearTimer(ScoreTimerHandle);

        if (ScoreText)
        {
            ScoreText->SetText(FText::AsNumber(TargetScore));
        }
    }
}