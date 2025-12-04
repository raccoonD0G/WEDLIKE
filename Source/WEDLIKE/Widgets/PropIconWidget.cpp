// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/PropIconWidget.h"
#include <Kismet/GameplayStatics.h>
#include "Components/Image.h"
#include "Widgets/BattleWidget.h"
#include "HUDs/LevelHUD.h"
#include "Enums/PropType.h"
#include "Components/CanvasPanel.h"
#include "Widgets/PropCountWidget.h"

void UPropIconWidget::Init(EPropType InPropType, FVector2D InStartPos)
{
    StartPos = InStartPos;
    SetRenderTranslation(StartPos);

    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    check(PC);

    ALevelHUD* LevelHUD = Cast<ALevelHUD>(PC->GetHUD());
    check(LevelHUD);

    UBattleWidget* BattleWidget = Cast<UBattleWidget>(LevelHUD->GetLevelWidget());
    check(BattleWidget);

    switch (InPropType)
    {
    case EPropType::OriginalRamyun:
        PropCountWidget = BattleWidget->GetOriginalRamyunCountWidget();
		break;
    case EPropType::ShrimpRamyun:
		PropCountWidget = BattleWidget->GetShrimpRamyunCountWidget();
        break;
	case EPropType::SpicyRamyun:
        PropCountWidget = BattleWidget->GetSpicyRamyunCountWidget();
		break;
    case EPropType::Tangerine:
		PropCountWidget = BattleWidget->GetTangerineCountWidget();
        break;
    case EPropType::Topokki:
        PropCountWidget = BattleWidget->GetTopokkiCountWidget();
		break;
    default:
        break;
    }

    UCanvasPanel* Canvas = BattleWidget->GetCanvasPannel();
    const FGeometry& CanvasGeo = Canvas->GetCachedGeometry();

    FVector2D EndAbs = PropCountWidget->GetCachedGeometry().GetAbsolutePosition();
    EndPos = CanvasGeo.AbsoluteToLocal(EndAbs);

    const FGeometry& CountGeo = PropCountWidget->GetCachedGeometry();
    FVector2D CountSize = CountGeo.GetLocalSize();

    EndPos.X += CountSize.X * 0.5f;
    EndPos.Y += CountSize.Y * 0.5f;
}

void UPropIconWidget::NativePreConstruct()
{
    Super::NativePreConstruct();

    if (ItemImage)
    {
        ItemImage->SetBrushFromTexture(ItemTexture);
    }
}

void UPropIconWidget::NativeConstruct()
{
	Super::NativeConstruct();

    if (ItemImage)
    {
        ItemImage->SetBrushFromTexture(ItemTexture);
    }

    StartScaleAnimation();
}

void UPropIconWidget::TestFunction()
{
    if (ItemImage)
    {
        ItemImage->SetBrushFromTexture(ItemTexture);
    }

    StartScaleAnimation();
}

void UPropIconWidget::StartScaleAnimation()
{
    OvershootScale = EndScale * 1.1f;

    ScaleAnimElapsed = 0.f;

    SetImageScale(StartScale);

    GetWorld()->GetTimerManager().SetTimer(
        ScaleTimerHandle,
        this,
        &UPropIconWidget::UpdateScaleAnimation,
        0.033f,
        true
    );
}

void UPropIconWidget::UpdateScaleAnimation()
{
    ScaleAnimElapsed += 0.033f;

    float Alpha = ScaleAnimElapsed / ScaleAnimDuration;

    if (Alpha < 0.7f)
    {
        float T = Alpha / 0.7f;
        float Scale = FMath::InterpEaseOut(StartScale, OvershootScale, T, 2.0f);

        SetImageScale(Scale);
    }
    else if (Alpha < 1.0f)
    {
        float T = (Alpha - 0.7f) / 0.3f;
        float Scale = FMath::InterpEaseIn(OvershootScale, EndScale, T, 4.0f);

        SetImageScale(Scale);
    }
    else
    {
        SetImageScale(EndScale);
        GetWorld()->GetTimerManager().ClearTimer(ScaleTimerHandle);
        StartPosAnimation();
    }
}

void UPropIconWidget::SetImageScale(float Scale)
{
    if (!ItemImage) return;

    FVector2D NewScale(Scale, Scale);
    ItemImage->SetRenderScale(NewScale);
}

void UPropIconWidget::StartPosAnimation()
{
    PosAnimElapsed = 0.f;

    LastPos = StartPos;

    GetWorld()->GetTimerManager().SetTimer(
        PosTimerHandle,
        this,
        &UPropIconWidget::UpdatePosAnimation,
        0.033f,
        true
    );
}

void UPropIconWidget::UpdatePosAnimation()
{
    PosAnimElapsed += 0.033f;

    float Alpha = FMath::Clamp(PosAnimElapsed / PosAnimDuration, 0.f, 1.f);
    float SmoothAlpha = FMath::InterpEaseOut(0.f, 1.f, Alpha, 2.0f);

    LastPos = FMath::Lerp(LastPos, EndPos, SmoothAlpha);

    SetRenderTranslation(LastPos);

    if (Alpha >= 1.f)
    {
        SetRenderTranslation(EndPos);
        GetWorld()->GetTimerManager().ClearTimer(PosTimerHandle);
        RemoveFromParent();
    }
}
