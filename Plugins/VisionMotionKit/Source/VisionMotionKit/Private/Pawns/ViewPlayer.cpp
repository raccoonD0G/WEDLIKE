// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawns/ViewPlayer.h"
#include "Actors/MirroredPlayer.h"
#include "Components/SingleSwingClassifierComponent.h"
#include "Actors/SingleSwingFollower.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CameraBoundsClampComponent.h"
#include "Actors/PlayerCam.h"
#include <NiagaraFunctionLibrary.h>

AViewPlayer::AViewPlayer()
{
    PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	CameraBoundsClampComponent = CreateDefaultSubobject<UCameraBoundsClampComponent>(TEXT("CameraBoundsClampComponent"));

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
}

void AViewPlayer::BeginPlay()
{
    Super::BeginPlay();

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    check(MirroredPlayerClass);
    check(SingleSwingFollowerClass);
    check(PlayerCamClass)

    // --- WebcamReceiver 스폰 ---
    {
        const FRotator MirroredPlayerRot(0.0f, 0.0f, 0.0f);
        const FVector MirroredPlayerLoc(0, 0, -100);
        
        FTransform MirroredPlayerTF(MirroredPlayerRot, MirroredPlayerLoc, FVector(ViewScale, ViewScale, ViewScale));

        MirroredPlayer = GetWorld()->SpawnActorDeferred<AMirroredPlayer>(MirroredPlayerClass, MirroredPlayerTF);

        IClampVolumeInterface* ClampVolumeInterface = Cast<IClampVolumeInterface>(MirroredPlayer);
        check(ClampVolumeInterface);

        CameraBoundsClampComponent->Init(ClampVolumeInterface->GetClampVolume(), SpringArmComponent);

        MirroredPlayer->FinishSpawning(MirroredPlayerTF);
        MirroredPlayer->SetActorTransform(MirroredPlayerTF, false, nullptr, ETeleportType::ResetPhysics);
    }

    {
        FTransform FollowerTF;
        SingleSwingFollowerLeft = GetWorld()->SpawnActorDeferred<ASingleSwingFollower>(SingleSwingFollowerClass, FollowerTF);
        SingleSwingFollowerLeft->SetOwningPlayer(MirroredPlayer);
        SingleSwingFollowerLeft->SetHandPoseClassifierComponent(MirroredPlayer->GetLeftSingleSwingClassifierComponent());
        SingleSwingFollowerLeft->FinishSpawning(FollowerTF);

        SingleSwingFollowerRight = GetWorld()->SpawnActorDeferred<ASingleSwingFollower>(SingleSwingFollowerClass, FollowerTF);
        SingleSwingFollowerRight->SetOwningPlayer(MirroredPlayer);
        SingleSwingFollowerRight->SetHandPoseClassifierComponent(MirroredPlayer->GetRightSingleSwingClassifierComponent());
        SingleSwingFollowerRight->FinishSpawning(FollowerTF);
    }

    {
        const FTransform SpawnTransform = GetActorTransform();

        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // AlwaysSpawn
        SpawnParams.Owner = this;

        if (UWorld* World = GetWorld())
        {
            PlayerCam = World->SpawnActor<APlayerCam>(PlayerCamClass, SpawnTransform, SpawnParams);
            FAttachmentTransformRules AttachRules(EAttachmentRule::KeepRelative, true);
            PlayerCam->AttachToComponent(SpringArmComponent, AttachRules, TEXT("SpringEndpoint"));
        }
    }
}

void AViewPlayer::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (!MirroredPlayer) return;

    UPoseUdpReceiverComponent* Receiver = MirroredPlayer->GetPoseReceiver();

    if (!Receiver) return;

    FVector ShoulderMid;
    if (!Receiver->GetShoulderMidWorld(ShoulderMid)) return;

    const FVector TargetLoc = ShoulderMid + FollowWorldOffset;

    if (FollowLagSpeed <= KINDA_SMALL_NUMBER)
    {
        SetActorLocation(TargetLoc);
    }
    else
    {
        SetActorLocation(FMath::VInterpTo(GetActorLocation(), TargetLoc, DeltaSeconds, FollowLagSpeed));
    }
}

void AViewPlayer::PlayNiagaraOverCamera(class UNiagaraSystem* NiagaraSystem, const FRotator& NiagaraRotation, const FVector& NiagaraScale)
{
    FVector CamLocation = PlayerCam->GetActorLocation();
    FVector SpawnLocation = PlayerCam->GetActorForwardVector() * 100.f + CamLocation;

	UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
        GetWorld(),
        NiagaraSystem,
        SpawnLocation,
        NiagaraRotation,
        NiagaraScale,
        true,
        true,
        ENCPoolMethod::AutoRelease,
        true);
}
