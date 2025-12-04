// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ChromWidget.h"
#include <Kismet/KismetRenderingLibrary.h>
#include <Kismet/GameplayStatics.h>
#include <SaveGames/ChromSave.h>
#include "Components/EditableTextBox.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/Slider.h"

void UChromWidget::NativeConstruct()
{
	Super::NativeConstruct();

    // Execution Sequence then_0: CreateDynamicMaterialInstance -> SetBrushFromMaterial
    SetupChromViewColor();

    if (VideoImage && UiMaterial)
    {
        UiMID = UMaterialInstanceDynamic::Create(UiMaterial, this);
        VideoImage->SetBrushFromMaterial(UiMID);
    }

    if (RtMaterial)
    {
        RtMID = UMaterialInstanceDynamic::Create(RtMaterial, this);
    }

    UChromSave* ChromSave = nullptr;
    if (UGameplayStatics::DoesSaveGameExist(UChromSave::ChromSlot, 0))
    {
        ChromSave = Cast<UChromSave>(UGameplayStatics::LoadGameFromSlot(UChromSave::ChromSlot, 0));
    }
    if (!ChromSave)
    {
        ChromSave = Cast<UChromSave>(UGameplayStatics::CreateSaveGameObject(UChromSave::StaticClass()));
    }

    if (!ChromSave) return;

    // 세이브 데이터 불러오기
    PickedColor = ChromSave->SetKeyColor;
    SaveLumaMask = ChromSave->LumaMask;
    SaveAlphaMin = ChromSave->AlphaMin;
    SaveAlphaDivisor = ChromSave->AlphaDivisor;
    SaveIntensityFloor = ChromSave->IntensityFloor;
    SaveBrightness = ChromSave->Brightness;
    SaveContrast = ChromSave->Contrast;
    SaveSaturation = ChromSave->Saturation;
    SaveHueShift = ChromSave->HueShift;

    // 세이브 데이터 적용
    ChangeKeyColor();
    ChangeLumaMask(SaveLumaMask);
    ChangeAlphaMin(SaveAlphaMin);
    ChangeAlphaDivisor(SaveAlphaDivisor);
    ChangeIntensityFloor(SaveIntensityFloor);
    ChangeBrightness(SaveBrightness);
    ChangeContrast(SaveContrast);
    ChangeSaturation(SaveSaturation);
    ChangeHueShift(SaveHueShift);
    UpdateViewColor(PickedColor);

    if (SaveButton)
    {
        SaveButton->OnClicked.AddDynamic(this, &UChromWidget::OnSaveButtonClicked);
    }

    if (ResetButton)
    {
        ResetButton->OnClicked.AddDynamic(this, &UChromWidget::OnResetButtonClicked);
    }

    // 실시간 데이터 적용
    if (LumaMask)
    {
        LumaMask->OnTextCommitted.AddDynamic(this, &UChromWidget::OnValueCommittedLumaMask);
    }
    if (AlphaMin)
    {
        AlphaMin->OnTextCommitted.AddDynamic(this, &UChromWidget::OnValueCommittedAlphaMin);
    }
    if (AlphaDivisor)
    {
        AlphaDivisor->OnTextCommitted.AddDynamic(this, &UChromWidget::OnValueCommittedAlphaDivisor);
    }
    if (IntensityFloor)
    {
        IntensityFloor->OnTextCommitted.AddDynamic(this, &UChromWidget::OnValueCommittedIntensityFloor);
    }
    if (Brightness)
    {
        Brightness->OnTextCommitted.AddDynamic(this, &UChromWidget::OnValueCommittedBrightness);
    }
    if (Contrast)
    {
        Contrast->OnTextCommitted.AddDynamic(this, &UChromWidget::OnValueCommittedContrast);
    }
    if (Saturation)
    {
        Saturation->OnTextCommitted.AddDynamic(this, &UChromWidget::OnValueCommittedSaturation);
    }
    if (HueShift)
    {
        HueShift->OnTextCommitted.AddDynamic(this, &UChromWidget::OnValueCommittedHueShift);
    }

    if (LumaMaskSlider)
    {
        LumaMaskSlider->OnValueChanged.AddDynamic(this, &UChromWidget::OnSliderValueChangedLumaMask);
    }
    if (AlphaMinSlider)
    {
        AlphaMinSlider->OnValueChanged.AddDynamic(this, &UChromWidget::OnSliderValueChangedAlphaMin);
    }
    if (AlphaDivisorSlider)
    {
        AlphaDivisorSlider->OnValueChanged.AddDynamic(this, &UChromWidget::OnSliderValueChangedAlphaDivisor);
    }
    if (IntensityFloorSlider)
    {
        IntensityFloorSlider->OnValueChanged.AddDynamic(this, &UChromWidget::OnSliderValueChangedIntensityFloor);
    }
    if (BrightnessSlider)
    {
        BrightnessSlider->OnValueChanged.AddDynamic(this, &UChromWidget::OnSliderValueChangedBrightness);
    }
    if (ContrastSlider)
    {
        ContrastSlider->OnValueChanged.AddDynamic(this, &UChromWidget::OnSliderValueChangedContrast);
    }
    if (SaturationSlider)
    {
        SaturationSlider->OnValueChanged.AddDynamic(this, &UChromWidget::OnSliderValueChangedSaturation);
    }
    if (HueShiftSlider)
    {
        HueShiftSlider->OnValueChanged.AddDynamic(this, &UChromWidget::OnSliderValueChangedHueShift);
    }
}

FReply UChromWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    FVector2D UV;

    if (!ComputeUVOnImage(InMouseEvent, UV))
    {
        return FReply::Unhandled();
    }

    if (!RenderTarget || !RtMID)
    {
        return FReply::Handled();
    }

    // 1) 클릭 시점의 현재 프레임을 RT로 복사(그리기)
    UKismetRenderingLibrary::DrawMaterialToRenderTarget(this, RenderTarget, RtMID);

    // 2) UV -> 픽셀 좌표
    const int32 RtW = RenderTarget->SizeX;
    const int32 RtH = RenderTarget->SizeY;

    int32 Px = FMath::Clamp(FMath::FloorToInt(UV.X * RtW), 0, RtW - 1);
    int32 Py = FMath::Clamp(FMath::FloorToInt(UV.Y * RtH), 0, RtH - 1);

    // 3) 픽셀 읽기
    PickedColor = UKismetRenderingLibrary::ReadRenderTargetPixel(this, RenderTarget, Px, Py);

    PickedColor.A = 1.0f;

    // 디버그
    UE_LOG(LogTemp, Display, TEXT("PickedColor UV(%.3f, %.3f) -> Pixel(%d, %d) = %s"),
        UV.X, UV.Y, Px, Py, *PickedColor.ToString());

    UpdateViewColor(PickedColor);

    ChangeKeyColor();

    return FReply::Handled();
}

void UChromWidget::UpdateViewColor(FLinearColor NewColor)
{
    if (!MID_ViewColor)
    {
        UE_LOG(LogTemp, Warning, TEXT("UpdateViewColor called but MID_ViewColor is null on %s"), *GetName());
        return;
    }

    // 블루프린트의 SetVectorParameterValue("color", Value) 대응
    MID_ViewColor->SetVectorParameterValue(TEXT("color"), NewColor);

    UE_LOG(LogTemp, Verbose, TEXT("Updated ViewColor to R=%.2f G=%.2f B=%.2f A=%.2f"), NewColor.R, NewColor.G, NewColor.B, NewColor.A);
}

void UChromWidget::OnValueCommittedLumaMask(const FText& Text, ETextCommit::Type CommitMethod)
{
    const FString StringVal = Text.ToString();
    const float IntVal = FCString::Atof(*StringVal);

    UE_LOG(LogTemp, Log, TEXT("Entered Number: %f"), IntVal);

    ChangeLumaMask(IntVal);
}

void UChromWidget::OnValueCommittedAlphaMin(const FText& Text, ETextCommit::Type CommitMethod)
{
    const FString StringVal = Text.ToString();
    const float IntVal = FCString::Atof(*StringVal);

    UE_LOG(LogTemp, Log, TEXT("Entered Number: %f"), IntVal);

    ChangeAlphaMin(IntVal);
}

void UChromWidget::OnValueCommittedAlphaDivisor(const FText& Text, ETextCommit::Type CommitMethod)
{
    const FString StringVal = Text.ToString();
    const float IntVal = FCString::Atof(*StringVal);

    UE_LOG(LogTemp, Log, TEXT("Entered Number: %f"), IntVal);

    ChangeAlphaDivisor(IntVal);
}

void UChromWidget::OnValueCommittedIntensityFloor(const FText& Text, ETextCommit::Type CommitMethod)
{
    const FString StringVal = Text.ToString();
    const float IntVal = FCString::Atof(*StringVal);

    UE_LOG(LogTemp, Log, TEXT("Entered Number: %f"), IntVal);

    ChangeIntensityFloor(IntVal);
}

void UChromWidget::OnValueCommittedBrightness(const FText& Text, ETextCommit::Type CommitMethod)
{
    const FString StringVal = Text.ToString();
    const float IntVal = FCString::Atof(*StringVal);

    UE_LOG(LogTemp, Log, TEXT("Entered Number: %f"), IntVal);

    ChangeBrightness(IntVal);
}

void UChromWidget::OnValueCommittedContrast(const FText& Text, ETextCommit::Type CommitMethod)
{
    const FString StringVal = Text.ToString();
    const float IntVal = FCString::Atof(*StringVal);

    UE_LOG(LogTemp, Log, TEXT("Entered Number: %f"), IntVal);

    ChangeContrast(IntVal);
}

void UChromWidget::OnValueCommittedSaturation(const FText& Text, ETextCommit::Type CommitMethod)
{
    const FString StringVal = Text.ToString();
    const float IntVal = FCString::Atof(*StringVal);

    UE_LOG(LogTemp, Log, TEXT("Entered Number: %f"), IntVal);

    ChangeSaturation(IntVal);
}

void UChromWidget::OnValueCommittedHueShift(const FText& Text, ETextCommit::Type CommitMethod)
{
    const FString StringVal = Text.ToString();
    const float IntVal = FCString::Atof(*StringVal);

    UE_LOG(LogTemp, Log, TEXT("Entered Number: %f"), IntVal);

    ChangeHueShift(IntVal);
}

void UChromWidget::OnSliderValueChangedLumaMask(const float Value)
{
    ChangeLumaMask(Value);
}

void UChromWidget::OnSliderValueChangedAlphaMin(const float Value)
{
    ChangeAlphaMin(Value);
}

void UChromWidget::OnSliderValueChangedAlphaDivisor(const float Value)
{
    ChangeAlphaDivisor(Value);
}

void UChromWidget::OnSliderValueChangedIntensityFloor(const float Value)
{
    ChangeIntensityFloor(Value);
}

void UChromWidget::OnSliderValueChangedBrightness(const float Value)
{
    ChangeBrightness(Value);
}

void UChromWidget::OnSliderValueChangedContrast(const float Value)
{
    ChangeContrast(Value);
}

void UChromWidget::OnSliderValueChangedSaturation(const float Value)
{
    ChangeSaturation(Value);
}

void UChromWidget::OnSliderValueChangedHueShift(const float Value)
{
    ChangeHueShift(Value);
}

void UChromWidget::ChangeKeyColor()
{
    UiMID->SetVectorParameterValue("Key Color", PickedColor);
}

void UChromWidget::ChangeLumaMask(float f)
{
    UiMID->SetScalarParameterValue("LumaMask", f);

    LumaMask->SetText(FText::AsNumber(f));

    SaveLumaMask = f;
}

void UChromWidget::ChangeAlphaMin(float f)
{
    UiMID->SetScalarParameterValue("Alpha Min", f);

    AlphaMin->SetText(FText::AsNumber(f));

    SaveAlphaMin = f;
}

void UChromWidget::ChangeAlphaDivisor(float f)
{
    UiMID->SetScalarParameterValue("Alpha Divisor", f);

    AlphaDivisor->SetText(FText::AsNumber(f));

    SaveAlphaDivisor = f;
}

void UChromWidget::ChangeIntensityFloor(float f)
{
    UiMID->SetScalarParameterValue("IntensityFloor", f);

    IntensityFloor->SetText(FText::AsNumber(f));

    SaveIntensityFloor = f;
}

void UChromWidget::ChangeBrightness(float f)
{
    UiMID->SetScalarParameterValue("Brightness", f);

    Brightness->SetText(FText::AsNumber(f));

    SaveBrightness = f;
}

void UChromWidget::ChangeContrast(float f)
{
    UiMID->SetScalarParameterValue("Contrast", f);

    Contrast->SetText(FText::AsNumber(f));

    SaveContrast = f;
}

void UChromWidget::ChangeSaturation(float f)
{
    UiMID->SetScalarParameterValue("Saturation", f);

    Saturation->SetText(FText::AsNumber(f));

    SaveSaturation = f;
}

void UChromWidget::ChangeHueShift(float f)
{
    UiMID->SetScalarParameterValue("Hue Shift", f);

    HueShift->SetText(FText::AsNumber(f));

    SaveHueShift = f;
}

void UChromWidget::SaveParam()
{
    UChromSave* ChromSave = nullptr;
    if (UGameplayStatics::DoesSaveGameExist(UChromSave::ChromSlot, 0))
    {
        ChromSave = Cast<UChromSave>(UGameplayStatics::LoadGameFromSlot(UChromSave::ChromSlot, 0));
    }
    if (!ChromSave)
    {
        ChromSave = Cast<UChromSave>(UGameplayStatics::CreateSaveGameObject(UChromSave::StaticClass()));
    }

    if (!ChromSave) return;

    ChromSave->SetKeyColor = PickedColor;
    ChromSave->LumaMask = SaveLumaMask;
    ChromSave->AlphaMin = SaveAlphaMin;
    ChromSave->AlphaDivisor = SaveAlphaDivisor;
    ChromSave->IntensityFloor = SaveIntensityFloor;
    ChromSave->Brightness = SaveBrightness;
    ChromSave->Contrast = SaveContrast;
    ChromSave->Saturation = SaveSaturation;
    ChromSave->HueShift = SaveHueShift;

    UGameplayStatics::SaveGameToSlot(ChromSave, UChromSave::ChromSlot, 0);
}

void UChromWidget::ResetParam()
{
    UChromSave* ChromSave = nullptr;
    if (UGameplayStatics::DoesSaveGameExist(UChromSave::ChromSlot, 0))
    {
        ChromSave = Cast<UChromSave>(UGameplayStatics::LoadGameFromSlot(UChromSave::ChromSlot, 0));
    }
    if (!ChromSave)
    {
        ChromSave = Cast<UChromSave>(UGameplayStatics::CreateSaveGameObject(UChromSave::StaticClass()));
    }

    if (!ChromSave) return;

    ChromSave->SetKeyColor = FLinearColor::Green;
    ChromSave->LumaMask = 0.0f;
    ChromSave->AlphaMin = 0.0f;
    ChromSave->AlphaDivisor = 0.15f;
    ChromSave->IntensityFloor = 0.0f;
    ChromSave->Brightness = 1.0f;
    ChromSave->Contrast = 1.0f;
    ChromSave->Saturation = 0.0f;
    ChromSave->HueShift = 0.0f;

    UGameplayStatics::SaveGameToSlot(ChromSave, UChromSave::ChromSlot, 0);

    ResetMap();
}
void UChromWidget::SetupChromViewColor()
{
    UMaterialInterface* Parent = ViewColorParent.LoadSynchronous();
    if (!Parent)
    {
        UE_LOG(LogTemp, Warning, TEXT("UChromWidget: ViewColorParent not found."));
        return;
    }

    MID_ViewColor = UMaterialInstanceDynamic::Create(Parent, this);
    if (!MID_ViewColor)
    {
        UE_LOG(LogTemp, Warning, TEXT("UChromWidget: Failed to create MID_ViewColor."));
        return;
    }

    if (ChromColorImage)
    {
        ChromColorImage->SetBrushFromMaterial(MID_ViewColor);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("UChromWidget: ChromColorImage is null."));
    }
}

void UChromWidget::OnResetButtonClicked()
{
    ResetParam();
}

void UChromWidget::OnSaveButtonClicked()
{
    SaveParam();

    ResetMap();
}

void UChromWidget::ResetMap()
{
    UGameplayStatics::OpenLevel(this, FName(TEXT("LobbyMap")));
}

bool UChromWidget::ComputeUVOnImage(const FPointerEvent& MouseEvent, FVector2D& OutUV) const
{
    if (!VideoImage)
    {
        return false;
    }

    const FGeometry& ImgGeo = VideoImage->GetCachedGeometry();

    // 스크린 좌표 -> 이미지 로컬 좌표
    const FVector2D ScreenPos = MouseEvent.GetScreenSpacePosition();
    const FVector2D LocalPos = ImgGeo.AbsoluteToLocal(ScreenPos);
    const FVector2D ImgSize = ImgGeo.GetLocalSize();

    if (ImgSize.X <= 0.f || ImgSize.Y <= 0.f)
    {
        return false;
    }

    FVector2D UV(LocalPos.X / ImgSize.X, LocalPos.Y / ImgSize.Y);

    // 범위 체크 (이미지 영역 밖 클릭시 무시)
    if (UV.X < 0.f || UV.X > 1.f || UV.Y < 0.f || UV.Y > 1.f)
    {
        return false;
    }

    OutUV = UV;
    return true;
}

bool UChromWidget::ComputeUV_ScaleToFit(const FPointerEvent& MouseEvent, FVector2D& OutUV) const
{
    if (!VideoImage || !RenderTarget)
    {
        return false;
    }

    const FGeometry& ImgGeo = VideoImage->GetCachedGeometry();
    const FVector2D ScreenPos = MouseEvent.GetScreenSpacePosition();
    const FVector2D LocalPos = ImgGeo.AbsoluteToLocal(ScreenPos);
    const FVector2D WwHw = ImgGeo.GetLocalSize();

    const float RtW = RenderTarget->SizeX;
    const float RtH = RenderTarget->SizeY;

    if (WwHw.X <= 0.f || WwHw.Y <= 0.f || RtW <= 0.f || RtH <= 0.f)
    {
        return false;
    }

    const float Scale = FMath::Min(WwHw.X / RtW, WwHw.Y / RtH);
    const FVector2D Ds(RtW * Scale, RtH * Scale);
    const FVector2D Offset((WwHw.X - Ds.X) * 0.5f, (WwHw.Y - Ds.Y) * 0.5f);

    const FVector2D PPrime = LocalPos - Offset;

    if (PPrime.X < 0.f || PPrime.Y < 0.f || PPrime.X > Ds.X || PPrime.Y > Ds.Y)
    {
        return false;
    }

    OutUV = FVector2D(PPrime.X / Ds.X, PPrime.Y / Ds.Y);
    return true;
}