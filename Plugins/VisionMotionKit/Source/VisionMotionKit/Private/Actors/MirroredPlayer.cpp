// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/MirroredPlayer.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PoseUdpReceiverComponent.h"
#include "Components/SingleSwingClassifierComponent.h"

#include "Engine/Engine.h"          // GEngine
#include "Engine/World.h"           // GetWorld()
#include "Misc/OutputDeviceNull.h"  // 로그용
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInstance.h"
#include "Materials/MaterialInterface.h"
#include "SaveGames/ChromSave.h"
#include "Components/StaticMeshComponent.h"

AMirroredPlayer::AMirroredPlayer()
{
	PrimaryActorTick.bCanEverTick = false;

	ClampVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("ClampVolume"));
	ClampVolume->SetupAttachment(RootComponent);
	ClampVolume->SetBoxExtent(FVector(92.0f, 100.0f, 540.0f));
	ClampVolume->SetCollisionProfileName(TEXT("NoCollision"));

    PoseReceiver = CreateDefaultSubobject<UPoseUdpReceiverComponent>(TEXT("PoseReceiver"));

    LeftSingleSwingClassifierComponent = CreateDefaultSubobject<USingleSwingClassifierComponent>(TEXT("LeftSingleSwingClassifierComponent"));
    LeftSingleSwingClassifierComponent->SetCurrentHandSide(EHandSide::Left);

    RightSingleSwingClassifierComponent = CreateDefaultSubobject<USingleSwingClassifierComponent>(TEXT("RightSingleSwingClassifierComponent"));
    RightSingleSwingClassifierComponent->SetCurrentHandSide(EHandSide::Right);

	VideoComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VideoComponent"));
	VideoComponent->SetupAttachment(RootComponent);

	// 항상 시각화 되도록 설정
	// ClampVolume->SetHiddenInGame(false);     // 게임 중에도 보이게
	// ClampVolume->ShapeColor = FColor::Green; // 에디터 뷰에서 와이어프레임 색상
}

UBoxComponent* AMirroredPlayer::GetClampVolume() const
{
	return ClampVolume;
}

void AMirroredPlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();

    const FVector2D MirroredPlayerScale(10.8f * 100.0f, 19.2f * 100.0f);

    SetFrameSize(MirroredPlayerScale);
}

void AMirroredPlayer::BeginPlay()
{
    Super::BeginPlay();

    if (!IsValid(VideoMeshComponent))
    {
        UE_LOG(LogTemp, Error, TEXT("[WebcamReceiver] VideoMeshComponent is null"));
        return;
    }

    const int32 MatIndex = 0;

    // 1) 현재 슬롯에 꽂혀있는 머티리얼에서 MID를 '가져오거나' 없으면 '만들기'
    VideoMaterialInstance = Cast<UMaterialInstanceDynamic>(VideoMeshComponent->GetMaterial(MatIndex));
    if (!IsValid(VideoMaterialInstance))
    {
        if (!IsValid(VideoMaterial))
        {
            UE_LOG(LogTemp, Error, TEXT("[WebcamReceiver] No MID on mesh and VideoMaterial(base) is null"));
            return;
        }
        VideoMaterialInstance =
            VideoMeshComponent->CreateAndSetMaterialInstanceDynamicFromMaterial(MatIndex, VideoMaterial);
        UE_LOG(LogTemp, Log, TEXT("[WebcamReceiver] Created new MID and set on slot %d"), MatIndex);
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("[WebcamReceiver] Reusing existing MID from slot %d"), MatIndex);
    }

    // 2) NDI 재생/알파를 보이게 하는 토글(머티리얼에 실제 존재하는 이름을 그대로 사용)
    VideoMaterialInstance->SetScalarParameterValue(TEXT("Enable Video Texture"), 1.0f);
    VideoMaterialInstance->SetScalarParameterValue(TEXT("Enable Video Alpha"), 1.0f);
    // 필요 시 알파 미리보기 끄기
    VideoMaterialInstance->SetScalarParameterValue(TEXT("Alpha Preview"), 0.0f);

    // 3) 세이브 로드
    UChromSave* LoadedSave = nullptr;
    if (UGameplayStatics::DoesSaveGameExist(UChromSave::ChromSlot, 0))
    {
        LoadedSave = Cast<UChromSave>(UGameplayStatics::LoadGameFromSlot(UChromSave::ChromSlot, 0));
    }
    if (!LoadedSave)
    {
        UE_LOG(LogTemp, Warning, TEXT("[WebcamReceiver] Failed to load BattleSave; using defaults"));
        return; // 디폴트로 진행하려면 여기서 return 빼세요.
    }

    // 4) 파라미터 적용 (로그에서 확인된 정확한 이름 사용)
    VideoMaterialInstance->SetVectorParameterValue(TEXT("Key Color"), LoadedSave->SetKeyColor);
    VideoMaterialInstance->SetScalarParameterValue(TEXT("LumaMask"), LoadedSave->LumaMask);
    VideoMaterialInstance->SetScalarParameterValue(TEXT("Alpha Min"), LoadedSave->AlphaMin);
    VideoMaterialInstance->SetScalarParameterValue(TEXT("Alpha Divisor"), LoadedSave->AlphaDivisor);
    VideoMaterialInstance->SetScalarParameterValue(TEXT("IntensityFloor"), LoadedSave->IntensityFloor);

    VideoMaterialInstance->SetScalarParameterValue(TEXT("Brightness"), LoadedSave->Brightness);
    VideoMaterialInstance->SetScalarParameterValue(TEXT("Contrast"), LoadedSave->Contrast);
    VideoMaterialInstance->SetScalarParameterValue(TEXT("Saturation"), LoadedSave->Saturation);
    VideoMaterialInstance->SetScalarParameterValue(TEXT("Hue Shift"), LoadedSave->HueShift);

    // 5) 디버그 출력
    const FString Msg = FString::Printf(
        TEXT("[WebcamReceiver] Applied | Key=(%.3f,%.3f,%.3f,%.3f) LumaMask=%.3f AlphaMin=%.3f AlphaDiv=%.3f IntFloor=%.3f"),
        LoadedSave->SetKeyColor.R, LoadedSave->SetKeyColor.G, LoadedSave->SetKeyColor.B, LoadedSave->SetKeyColor.A,
        LoadedSave->LumaMask, LoadedSave->AlphaMin, LoadedSave->AlphaDivisor, LoadedSave->IntensityFloor
    );
    UE_LOG(LogTemp, Log, TEXT("%s"), *Msg);
    if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Yellow, Msg);
    
}
