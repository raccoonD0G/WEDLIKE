// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/RankingBoardWidget.h"
#include <Widgets/RankingRowWidget.h>
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

void URankingBoardWidget::NativePreConstruct()
{
    Super::NativePreConstruct();

	if (!RankingRowWidgetClass) return;

    Rows.Empty();

    for (int32 Index = 0; Index < RowCount; Index++)
    {
		AddRankingRowDirect(TEXT("--"), 0);
    }
}

void URankingBoardWidget::NativeConstruct()
{
	Super::NativeConstruct();

	check(RankingRowWidgetClass);

    Rows.Empty();

    for (int32 Index = 0; Index < RowCount; Index++)
    {
        AddRankingRowDirect(TEXT("--"), 0);
    }
}

URankingRowWidget* URankingBoardWidget::AddRankingRowAnimated(const FString& InPlayerName, int32 InScore, float ScoreDuration)
{
    if (!RankingCanvas || !RankingRowWidgetClass) return nullptr;

    int32 InsertIdx = 0;
    for (; InsertIdx < Rows.Num(); ++InsertIdx)
    {
        if (InScore > Rows[InsertIdx]->GetScore())
        {
            break;
        }
    }

    URankingRowWidget* NewRow = CreateWidget<URankingRowWidget>(GetWorld(), RankingRowWidgetClass);
    if (!NewRow) return nullptr;

    UCanvasPanelSlot* NewSlot = RankingCanvas->AddChildToCanvas(NewRow);
    if (!NewSlot) return nullptr;

    NewRow->Init(InPlayerName, InScore, ScoreDuration);
    FMargin StartOffset = NewSlot->GetOffsets();
    FMargin IndexOffset = StartOffset;

    IndexOffset.Top = InsertIdx * RowMargin + StartOffset.Top;
    IndexOffset.Bottom = -(InsertIdx * RowMargin) + StartOffset.Bottom;

    FMargin SlideOffset = IndexOffset;
    SlideOffset.Left += SlideInStartX;
    SlideOffset.Right -= SlideInStartX;
    NewSlot->SetOffsets(SlideOffset);

    Rows.Insert(NewRow, InsertIdx);

    for (int32 Index = InsertIdx + 1; Index < Rows.Num(); ++Index)
    {
        URankingRowWidget* Row = Rows[Index];
        if (!Row) continue;

        if (UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(Row->Slot))
        {
            FMargin NewOffset = StartOffset;
            NewOffset.Top = Index * RowMargin + StartOffset.Top;
            NewOffset.Bottom = -(Index * RowMargin) + StartOffset.Bottom;
            Row->StartMoveTo(NewOffset, MoveDuration);
        }
    }

    NewRow->StartMoveTo(IndexOffset, SlideDuration);

    SetRankNumTexts();

    return NewRow;
}

URankingRowWidget* URankingBoardWidget::AddRankingRowDirect(const FString& InPlayerName, int32 InScore, float ScoreDuration)
{
    if (!RankingCanvas || !RankingRowWidgetClass) return nullptr;

    int32 InsertIdx = 0;
    for (; InsertIdx < Rows.Num(); ++InsertIdx)
    {
        if (InScore > Rows[InsertIdx]->GetScore())
        {
            break;
        }
    }

    URankingRowWidget* NewRow = CreateWidget<URankingRowWidget>(GetWorld(), RankingRowWidgetClass);
    if (!NewRow) return nullptr;

    UCanvasPanelSlot* NewSlot = RankingCanvas->AddChildToCanvas(NewRow);
    if (!NewSlot) return nullptr;

    NewRow->Init(InPlayerName, InScore, ScoreDuration);

    FMargin StartOffset = NewSlot->GetOffsets();
	FMargin IndexOffset = StartOffset;

	IndexOffset.Top = InsertIdx * RowMargin + StartOffset.Top;
	IndexOffset.Bottom = -(InsertIdx * RowMargin) + StartOffset.Bottom;

    NewSlot->SetOffsets(IndexOffset);
    Rows.Insert(NewRow, InsertIdx);

    for (int32 Index = InsertIdx + 1; Index < Rows.Num(); ++Index)
    {
        URankingRowWidget* Row = Rows[Index];
        if (!Row) continue;
        if (UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(Row->Slot))
        {
            FMargin NewOffset = StartOffset;
			NewOffset.Top = Index * RowMargin + StartOffset.Top;
			NewOffset.Bottom = -(Index * RowMargin) + StartOffset.Bottom;
            CanvasPanelSlot->SetOffsets(NewOffset);
        }
    }

    SetRankNumTexts();

	return NewRow;
}

void URankingBoardWidget::HighlightRow(int32 Index)
{
	Rows[Index]->Highlight();
}

bool URankingBoardWidget::GetIndexOfRow(URankingRowWidget* RowWidget, int32& OutIndex) const
{
    for (int32 i = 0; i < Rows.Num(); ++i)
    {
        if (Rows[i] == RowWidget)
        {
            OutIndex = i;
            return true;
        }
    }
    return false;
}

void URankingBoardWidget::SetRankIcon(URankingRowWidget* InRow)
{
    if (!InRow) return;

    const int32 Index = Rows.IndexOfByKey(InRow);
    if (Index == INDEX_NONE) return;

    if (RankIcons.IsValidIndex(Index))
    {
        InRow->SetShowRankIcon(true);
        InRow->SetRankIconImage(RankIcons[Index]);
    }
    else
    {
		InRow->SetShowRankIcon(false);
    }
}

void URankingBoardWidget::SetRankFrame(URankingRowWidget* InRow)
{
    if (!InRow) return;

    const int32 Index = Rows.IndexOfByKey(InRow);
    if (Index == INDEX_NONE) return;

    if (Index < ShowFrameRawCount)
    {
        InRow->SetShowFrame(true);
    }
    else
    {
        InRow->SetShowFrame(false);
	}
}

void URankingBoardWidget::SetRankNumTexts()
{
    for (int32 Index = 0; Index < Rows.Num(); ++Index)
    {
        Rows[Index]->SetRankNumText(Index + 1);
        SetRankIcon(Rows[Index]);
        SetRankFrame(Rows[Index]);
    }
}