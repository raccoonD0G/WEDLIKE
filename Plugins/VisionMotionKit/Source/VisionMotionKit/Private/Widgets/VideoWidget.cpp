// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/VideoWidget.h"
#include "Components/Image.h"
#include "MediaPlayer.h"
#include "MediaSource.h"
#include "Components/Border.h"

void UVideoWidget::NativeConstruct()
{
	Super::NativeConstruct();

    if (CoverImage && CoverImageTexture)
    {
		CoverImage->SetBrushFromTexture(CoverImageTexture);
    }
    else
    {
        CoverImage->SetOpacity(0.0f);
    }

    if (VideoMaterial)
    {
        VideoPanel->SetBrushFromMaterial(VideoMaterial);
    }

	OpenVideoDelayed();
}

void UVideoWidget::OpenVideoDelayed()
{
    if (!GetWorld())
    {
        UE_LOG(LogTemp, Warning, TEXT("UVideoWidget: No World for timer."));
        return;
    }

    // BP KismetSystemLibrary::Delay(0.1) 대응: 타이머로 대체
    GetWorld()->GetTimerManager().SetTimer(
        VideoDelayHandle, this, &UVideoWidget::OpenVideo, 0.1f, /*bLoop*/false);
}

void UVideoWidget::OpenVideo()
{
    if (!Video)
    {
        UE_LOG(LogTemp, Warning, TEXT("UVideoWidget: Video(MediaPlayer) is null."));
        return;
    }

    UMediaSource* Source = VideoSource.LoadSynchronous();
    if (!Source)
    {
        UE_LOG(LogTemp, Warning, TEXT("UVideoWidget: VideoSource not found."));
        return;
    }

    const bool bWillOpen = Video->OpenSource(Source);
    if (!bWillOpen)
    {
        UE_LOG(LogTemp, Warning, TEXT("UVideoWidget: MediaPlayer->OpenSource() returned false."));
    }
}