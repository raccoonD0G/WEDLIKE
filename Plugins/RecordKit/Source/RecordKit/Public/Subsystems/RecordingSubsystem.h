// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "RecordingSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class RECORDKIT_API URecordingSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable)
    void StartRecording();

    UFUNCTION(BlueprintCallable)
    void StopRecording();

private:
    bool LaunchFF(bool bUseGdi, bool bGotMon, int32 MonX, int32 MonY, int32 MonW, int32 MonH);

    bool IsRecording() const { return bRecording; }

    UPROPERTY()
    FString FFmpegExe = TEXT("FFmpeg/bin/ffmpeg.exe");

    UPROPERTY()
    FString OutputDir;

    UPROPERTY()
    int32 Framerate = 30;

    UPROPERTY()
    int32 CRF = 23;

    UPROPERTY()
    uint8 bHideWindow : 1 = true;

private:
    uint8 bRecording : 1 = false;

    FString OutputPath;
    FProcHandle FfmpegHandle;

    void* ChildStdOut_Read = nullptr;  // 자식 STDOUT/ERR 읽기 (부모가 ReadPipe)
    void* ChildStdOut_Write = nullptr; // 자식 STDOUT/ERR 쓰기 (CreateProc에 전달)
    void* ChildStdIn_Read = nullptr;  // 자식 STDIN 읽기 (CreateProc에 전달)
    void* ChildStdIn_Write = nullptr;  // 자식 STDIN 쓰기 (부모가 WritePipe)

    double StartTimeSec = 0.0;
    uint8 bUsedGdiGrab : 1 = false;

    void KillIfRunning();

// Sound Section
private:
    UPROPERTY()
    uint8 bCaptureSystemAudio : 1 = true;

    UPROPERTY()
    int32 AudioSampleRate = 48000;

    UPROPERTY()
    int32 AudioBitrateKbps = 192;
};
