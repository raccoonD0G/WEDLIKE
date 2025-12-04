// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VideoWidget.generated.h"

/**
 * 
 */
UCLASS()
class VISIONMOTIONKIT_API UVideoWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(EditAnywhere, Category = "Video")
	TObjectPtr<class UMaterial> VideoMaterial;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UBorder> VideoPanel;

	UPROPERTY(EditAnywhere, Category = "Video")
	TObjectPtr<class UMediaPlayer> Video = nullptr;

	UPROPERTY(EditAnywhere, Category = "Video")
	TSoftObjectPtr<class UMediaSource> VideoSource = TSoftObjectPtr<class UMediaSource>(FSoftObjectPath(TEXT("/Game/Assets/Widget/Media/FMS_Intro.FMS_Intro")));

	UPROPERTY(EditAnywhere, Category = "Video")
	class UTexture2D* CoverImageTexture = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UImage* CoverImage = nullptr;

	/** 분기 B: 0.1초 후 인트로 열기 */
	void OpenVideoDelayed();
	void OpenVideo();

	FTimerHandle VideoDelayHandle;
};
