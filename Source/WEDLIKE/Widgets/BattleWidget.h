// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Enums/PropType.h"
#include "BattleWidget.generated.h"



/**
 * 
 */
UCLASS()
class WEDLIKE_API UBattleWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

// Canvas Section
public:
	class UCanvasPanel* GetCanvasPannel() { return CanvasPanel; }

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> CanvasPanel;

// Video Section
public:
	void PlayVideoByType(EPropType VideoType);

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> VideoImage;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UMediaPlayer> VideoPlayer;

	UPROPERTY(EditAnywhere)
	TMap<EPropType, TSoftObjectPtr<class UMediaSource>> VideoSources;

	UFUNCTION()
	void HandleVideoFinished();

// Score Section
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> LastScoreText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> HighScoreText;

	UFUNCTION()
	void UpdateLastScoreTexts(int32 NewLastScore);

	UFUNCTION()
	void UpdateHighScoreText(int32 NewHighScore);

// Waiting Timer Section
private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> WaitingTimerText;

	UFUNCTION()
	void UpdateWaitingTimerText(int32 NewTime);

	UFUNCTION()
	void HideWaitingTimerText();

// Battle Timer Section
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> BattleTimerText;

	UFUNCTION()
	void UpdateBattleTimerText(int32 NewTime);

// Prop Score Item Section
public:
	class UPropCountWidget* GetOriginalRamyunCountWidget() const { return OriginalRamyunCountWidget; }
	class UPropCountWidget* GetShrimpRamyunCountWidget() const { return ShrimpRamyunCountWidget; }
	class UPropCountWidget* GetSpicyRamyunCountWidget() const { return SpicyRamyunCountWidget; }
	class UPropCountWidget* GetTangerineCountWidget() const { return TangerineCountWidget; }
	class UPropCountWidget* GetTopokkiCountWidget() const { return TopokkiCountWidget; }

protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UPropCountWidget> OriginalRamyunCountWidget;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UPropCountWidget> ShrimpRamyunCountWidget;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UPropCountWidget> SpicyRamyunCountWidget;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UPropCountWidget> TangerineCountWidget;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UPropCountWidget> TopokkiCountWidget;

// Start&GameOver Section
public:
	void PlayStartWidget();
	void PlayGameOverWidget();

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UUserWidget> StartWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UUserWidget> GameOverWidget;

	FTimerHandle StartWidgetTimerHandle;
	FTimerHandle GameOverWidgetTimerHandle;

	void ShowStartWidget();
	void HideStartWidget();

	void ShowGameOverWidget();
	void HideGameOverWidget();

// Sound Section
private:
	UPROPERTY(EditAnywhere)
	TArray<TObjectPtr<class USoundBase>> CheerSounds;
};
