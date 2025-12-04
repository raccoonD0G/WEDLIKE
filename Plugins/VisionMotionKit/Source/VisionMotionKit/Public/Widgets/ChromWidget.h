// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChromWidget.generated.h"

/**
 * 
 */
UCLASS()
class VISIONMOTIONKIT_API UChromWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;


protected:
	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* LumaMask;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* AlphaMin;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* AlphaDivisor;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* IntensityFloor;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* Brightness;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* Contrast;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* Saturation;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* HueShift;

	UPROPERTY(meta = (BindWidget))
	class USlider* LumaMaskSlider;

	UPROPERTY(meta = (BindWidget))
	USlider* AlphaMinSlider;

	UPROPERTY(meta = (BindWidget))
	USlider* AlphaDivisorSlider;

	UPROPERTY(meta = (BindWidget))
	USlider* IntensityFloorSlider;

	UPROPERTY(meta = (BindWidget))
	USlider* BrightnessSlider;

	UPROPERTY(meta = (BindWidget))
	USlider* ContrastSlider;

	UPROPERTY(meta = (BindWidget))
	USlider* SaturationSlider;

	UPROPERTY(meta = (BindWidget))
	USlider* HueShiftSlider;

	UPROPERTY(meta = (BindWidget))
	class UButton* SaveButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* ResetButton;

	UPROPERTY(meta = (BindWidget))
	class UImage* VideoImage = nullptr;

	// UI용 머테리얼 (Domain=User Interface)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ColorPicker")
	UMaterialInterface* UiMaterial = nullptr;

	// RenderTarget에 그릴 머테리얼 (Domain=Surface, Unlit)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ColorPicker")
	UMaterialInterface* RtMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ColorPicker")
	UTextureRenderTarget2D* RenderTarget = nullptr;

	// 클릭 시 선택된 색 (마지막 값 저장)
	UPROPERTY(BlueprintReadOnly, Category = "ColorPicker")
	FLinearColor PickedColor = FLinearColor::Black;

	UPROPERTY()
	float SaveLumaMask = 0.0f;

	UPROPERTY()
	float SaveAlphaMin = 0.0f;

	UPROPERTY()
	float SaveAlphaDivisor = 0.0f;

	UPROPERTY()
	float SaveIntensityFloor = 0.0f;

	UPROPERTY()
	float SaveBrightness = 0.0f;

	UPROPERTY()
	float SaveContrast = 0.0f;

	UPROPERTY()
	float SaveSaturation = 0.0f;

	UPROPERTY()
	float SaveHueShift = 0.0f;

	void UpdateViewColor(FLinearColor NewColor);

	UFUNCTION()
	void OnValueCommittedLumaMask(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnValueCommittedAlphaMin(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnValueCommittedAlphaDivisor(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnValueCommittedIntensityFloor(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnValueCommittedBrightness(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnValueCommittedContrast(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnValueCommittedSaturation(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnValueCommittedHueShift(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnSliderValueChangedLumaMask(const float Value);

	UFUNCTION()
	void OnSliderValueChangedAlphaMin(const float Value);

	UFUNCTION()
	void OnSliderValueChangedAlphaDivisor(const float Value);

	UFUNCTION()
	void OnSliderValueChangedIntensityFloor(const float Value);

	UFUNCTION()
	void OnSliderValueChangedBrightness(const float Value);

	UFUNCTION()
	void OnSliderValueChangedContrast(const float Value);

	UFUNCTION()
	void OnSliderValueChangedSaturation(const float Value);

	UFUNCTION()
	void OnSliderValueChangedHueShift(const float Value);

	UFUNCTION()
	void ChangeKeyColor();

	UFUNCTION()
	void ChangeLumaMask(float f);

	UFUNCTION()
	void ChangeAlphaMin(float f);

	UFUNCTION()
	void ChangeAlphaDivisor(float f);

	UFUNCTION()
	void ChangeIntensityFloor(float f);

	UFUNCTION()
	void ChangeBrightness(float f);

	UFUNCTION()
	void ChangeContrast(float f);

	UFUNCTION()
	void ChangeSaturation(float f);

	UFUNCTION()
	void ChangeHueShift(float f);

	void SaveParam();

	void ResetParam();

	UFUNCTION()
	void OnSaveButtonClicked();



private:
	UPROPERTY()
	UMaterialInstanceDynamic* UiMID = nullptr;

	UPROPERTY()
	UMaterialInstanceDynamic* RtMID = nullptr;

	// Image 위젯의 Geometry 기준으로 UV 계산 (Stretch=Fill 기본, 필요 시 StretchToFit 보정 코드 참고)
	bool ComputeUVOnImage(const FPointerEvent& MouseEvent, FVector2D& OutUV) const;

	// 필요 시: ScaleToFit 보정 버전
	bool ComputeUV_ScaleToFit(const FPointerEvent& MouseEvent, FVector2D& OutUV) const;

	/** 분기 A: MID 생성 + 브러시 적용 */
	void SetupChromViewColor();

	UFUNCTION()
	void OnResetButtonClicked();

	void ResetMap();

private:
	/** BP 위젯: ChromColorImage */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> ChromColorImage = nullptr;

	/** BP 변수: MID_ViewColor */
	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> MID_ViewColor = nullptr;

	/** BP에서 CreateDynamicMaterialInstance Parent로 쓰던 머티리얼 (M_ViewColor) */
	UPROPERTY(EditDefaultsOnly, Category = "Chrom")
	TSoftObjectPtr<UMaterialInterface> ViewColorParent = TSoftObjectPtr<UMaterialInterface>(FSoftObjectPath(TEXT("/Game/Assets/Widget/Chrom/M_ViewColor.M_ViewColor")));
};
