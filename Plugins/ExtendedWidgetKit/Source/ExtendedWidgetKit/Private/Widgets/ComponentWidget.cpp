// ComponentWidget.cpp

#include "Widgets/ComponentWidget.h"
#include "Slate/WidgetRenderer.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Engine/TextureRenderTarget2D.h"
#include "TimerManager.h"
#include "Blueprint/WidgetLayoutLibrary.h"

void UComponentWidget::Init()
{
    ;
}

void UComponentWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // InitWidgetRenderTarget(1024, 512, true, RTF_RGBA8);
    // SetAutoRedraw(true, 1.f / 30.f); // 30FPS 갱신
}

void UComponentWidget::InitWidgetRenderTarget(int32 Width, int32 Height, bool bInUseGammaSpace, TEnumAsByte<ETextureRenderTargetFormat> InFormat)
{
    DesiredRTSize.X = FMath::Max(1, Width);
    DesiredRTSize.Y = FMath::Max(1, Height);
    bUseGammaSpace = bInUseGammaSpace;
    RTFormat = InFormat;

    if (!WidgetRenderer.IsValid())
    {
        // bCompositeMode = true 로 변경 (알파 합성/클리어 처리 용이)
        WidgetRenderer = MakeShared<FWidgetRenderer>(bUseGammaSpace, /*bCompositeMode*/ true);
    }

    const bool bNeedCreate =
        !WidgetRenderTarget ||
        WidgetRenderTarget->SizeX != DesiredRTSize.X ||
        WidgetRenderTarget->SizeY != DesiredRTSize.Y ||
        WidgetRenderTarget->RenderTargetFormat != RTFormat;

    if (bNeedCreate)
    {
        WidgetRenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(
            this, DesiredRTSize.X, DesiredRTSize.Y, RTFormat);

        if (WidgetRenderTarget)
        {
            WidgetRenderTarget->bAutoGenerateMips = false;
            WidgetRenderTarget->ClearColor = FLinearColor::Transparent;
            WidgetRenderTarget->TargetGamma = bUseGammaSpace ? 2.2f : 1.0f;
        }
    }
}

void UComponentWidget::SetWidgetRenderTarget(UTextureRenderTarget2D* InRT)
{
    WidgetRenderTarget = InRT;

    if (!WidgetRenderer.IsValid())
    {
        WidgetRenderer = MakeShared<FWidgetRenderer>(bUseGammaSpace, /*bCompositeMode*/ true);
    }
}

void UComponentWidget::DrawWidgetToRenderTarget()
{
    if (!WidgetRenderer.IsValid())
    {
        WidgetRenderer = MakeShared<FWidgetRenderer>(bUseGammaSpace, /*bCompositeMode*/ true);
    }

    if (!WidgetRenderTarget)
    {
        InitWidgetRenderTarget(DesiredRTSize.X, DesiredRTSize.Y, bUseGammaSpace, RTFormat);
        if (!WidgetRenderTarget) return;
    }

    // 동일 프레임 중복 렌더 방지
    if (LastDrawFrame == GFrameCounter) return;
    LastDrawFrame = GFrameCounter;

    // 화면에도 떠 있는 인스턴스를 동시에 RT로 그리면 DrawBuffer가 고갈될 수 있음
    if (IsInViewport())
    {
        UE_LOG(LogTemp, Warning, TEXT("[UComponentWidget] This widget is in viewport AND being drawn to RT. Consider using a separate offscreen instance."));
    }

    // RT 클리어(Composite 모드와 함께 사용 권장)
    UKismetRenderingLibrary::ClearRenderTarget2D(this, WidgetRenderTarget, FLinearColor::Transparent);

    // Slate 위젯 그리기
    TSharedRef<SWidget> SlateWidget = TakeWidget();
    WidgetRenderer->DrawWidget(
        WidgetRenderTarget,
        SlateWidget,
        FVector2D(DesiredRTSize),
        /*DeltaTime*/ 0.f
    );
}

void UComponentWidget::SetAutoRedraw(bool bEnable, float InIntervalSeconds)
{
    bAutoRedraw = bEnable;
    RedrawIntervalSeconds = FMath::Max(0.f, InIntervalSeconds);

    if (UWorld* World = GetWorld())
    {
        FTimerManager& TM = World->GetTimerManager();
        TM.ClearTimer(AutoRedrawTimer);

        if (bAutoRedraw)
        {
            // 0 또는 너무 작은 값이면 기본 1/30초로
            const float Interval = (RedrawIntervalSeconds <= KINDA_SMALL_NUMBER) ? (1.f / 30.f) : RedrawIntervalSeconds;

            TM.SetTimer(
                AutoRedrawTimer,
                FTimerDelegate::CreateUObject(this, &UComponentWidget::DrawWidgetToRenderTarget),
                Interval,
                /*bLoop*/ true
            );
        }
    }
}

void UComponentWidget::NativeDestruct()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(AutoRedrawTimer);
    }
    WidgetRenderer.Reset();
    Super::NativeDestruct();
}