// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/TextureRenderTarget2D.h"
#include "ComponentWidget.generated.h"

/**
 * 
 */
UCLASS()
class EXTENDEDWIDGETKIT_API UComponentWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void Init();

protected:
	virtual void NativeConstruct() override;

public:
	FORCEINLINE AActor* GetOwningActor() const { return OwningActor; }
	FORCEINLINE void SetOwningActor(AActor* InActor) { OwningActor = InActor; }

	// ===== Render-to-Texture API =====
	/** RT를 생성/초기화 (필요 시 다시 생성). */
	UFUNCTION(BlueprintCallable, Category = "RenderToTexture")
	void InitWidgetRenderTarget(int32 Width = 1024, int32 Height = 512,
		bool bInUseGammaSpace = true, TEnumAsByte<ETextureRenderTargetFormat> InFormat = RTF_RGBA8);

	/** 외부 RT를 지정해서 여기에 그리도록 설정. */
	UFUNCTION(BlueprintCallable, Category = "RenderToTexture")
	void SetWidgetRenderTarget(UTextureRenderTarget2D* InRT);

	/** 현재 설정된 RT로 즉시 그리기(한 프레임 갱신). */
	UFUNCTION(BlueprintCallable, Category = "RenderToTexture")
	void DrawWidgetToRenderTarget();

	/** 자동 갱신 On/Off 및 주기(초). 0이면 매 틱. */
	UFUNCTION(BlueprintCallable, Category = "RenderToTexture")
	void SetAutoRedraw(bool bEnable, float InIntervalSeconds = 0.f);

	/** 현재 RT 가져오기 (머티리얼 파라미터로 꽂아 사용). */
	UFUNCTION(BlueprintCallable, Category = "RenderToTexture")
	UTextureRenderTarget2D* GetWidgetRenderTarget() const { return WidgetRenderTarget; }

protected:
	virtual void NativeDestruct() override;

private:
	UPROPERTY()
	TObjectPtr<AActor> OwningActor = nullptr;

	// ===== 내부 상태 =====
	UPROPERTY(Transient)
	TObjectPtr<UTextureRenderTarget2D> WidgetRenderTarget = nullptr;

	UPROPERTY(EditAnywhere, Category = "RenderToTexture")
	FIntPoint DesiredRTSize = FIntPoint(1024, 512);

	UPROPERTY(EditAnywhere, Category = "RenderToTexture")
	bool bUseGammaSpace = true;

	UPROPERTY(EditAnywhere, Category = "RenderToTexture")
	TEnumAsByte<ETextureRenderTargetFormat> RTFormat = RTF_RGBA8;

	/** 자동 갱신 설정 */
	UPROPERTY(EditAnywhere, Category = "RenderToTexture")
	bool bAutoRedraw = false;

	UPROPERTY(EditAnywhere, Category = "RenderToTexture", meta = (EditCondition = "bAutoRedraw"))
	float RedrawIntervalSeconds = 0.f; // 0 = 매 틱

	float RedrawAccum = 0.f;

	/** Slate 위젯을 RT에 그리는 렌더러 */
	TSharedPtr<class FWidgetRenderer> WidgetRenderer;

private:
	/** 자동 갱신: Tick 대신 월드 타이머 사용 */
	FTimerHandle AutoRedrawTimer;

	/** 같은 프레임 중복 그리기 방지 */
	uint64 LastDrawFrame = MAX_uint64;
};
