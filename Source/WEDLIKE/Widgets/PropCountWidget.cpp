// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/PropCountWidget.h"
#include "Components/TextBlock.h"
#include <Kismet/GameplayStatics.h>
#include <PlayerStates/BattlePlayerState.h>
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"

void UPropCountWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	RefreshWidget();
}

void UPropCountWidget::NativeConstruct()
{
	Super::NativeConstruct();

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	check(PC);

	ABattlePlayerState* BattlePlayerState = PC->GetPlayerState<ABattlePlayerState>();
	check(BattlePlayerState);

	BattlePlayerState->OnPropCountChangeDelegate.AddDynamic(this, &UPropCountWidget::OnPropCountChange);

	SetCountText(BattlePlayerState->GetPropCount(CountPropType));

	RefreshWidget();
}

void UPropCountWidget::RefreshWidget()
{
	if (ItemImage)
	{
		ItemImage->SetBrushFromTexture(ItemTexture);
	}

	if (CountText)
	{
		FSlateFontInfo FontInfo = CountText->GetFont();
		FontInfo.Size = CountFontSize;
		CountText->SetFont(FontInfo);
	}
}

void UPropCountWidget::SetCountText(int32 InCount)
{
	CountText->SetText(FText::AsNumber(InCount));
}

void UPropCountWidget::OnPropCountChange(EPropType PropType, int32 NewPropCount)
{
	if (PropType == CountPropType)
	{
		SetCountText(NewPropCount);

		PlayPropCountChangeAnimation();
	}
}