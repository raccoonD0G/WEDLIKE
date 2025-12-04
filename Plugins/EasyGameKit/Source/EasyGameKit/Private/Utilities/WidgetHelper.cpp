// Fill out your copyright notice in the Description page of Project Settings.


#include "Utilities/WidgetHelper.h"
#include <Kismet/GameplayStatics.h>
#include <Blueprint/WidgetLayoutLibrary.h>
#include "Components/CanvasPanel.h"

bool UWidgetHelper::GetActorCanvasLocalPosition(
    const UObject* WorldContextObject,
    AActor* TargetActor,
    UCanvasPanel* ReferenceCanvas,
    FVector2D& OutLocalPos)
{
    OutLocalPos = FVector2D::ZeroVector;
    if (!WorldContextObject || !TargetActor || !ReferenceCanvas)
        return false;

    APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0);
    if (!PC) return false;

    // 1) 월드 → 뷰포트 로컬 좌표 (DPI 보정된 UMG 좌표)
    FVector2D ViewportLocalPos;
    if (!UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(
        PC, TargetActor->GetActorLocation(), ViewportLocalPos, true))
        return false;

    // 2) 뷰포트 로컬 → 절대 좌표로 변환
    const FGeometry ViewportGeo =
        UWidgetLayoutLibrary::GetViewportWidgetGeometry(WorldContextObject);
    const FVector2D AbsolutePos = ViewportGeo.LocalToAbsolute(ViewportLocalPos);

    // 3) 절대 → 캔버스 로컬
    const FGeometry& CanvasGeo = ReferenceCanvas->GetCachedGeometry();
    if (CanvasGeo.GetLocalSize().IsNearlyZero())
        return false;

    OutLocalPos = CanvasGeo.AbsoluteToLocal(AbsolutePos);
    return true;
}
