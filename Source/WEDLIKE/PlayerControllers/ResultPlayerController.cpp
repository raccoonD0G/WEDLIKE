// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerControllers/ResultPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "EnhancedInputComponent.h"
#include <Kismet/GameplayStatics.h>

void AResultPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (ULocalPlayer* LP = GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LP))
        {
            Subsystem->AddMappingContext(IMC_Result, /*Priority*/ 0);
        }
    }

    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
    {
        EIC->BindAction(IA_ResultSpace, ETriggerEvent::Started, this, &AResultPlayerController::OnSpacePressed);
        EIC->BindAction(IA_ResultS, ETriggerEvent::Started, this, &AResultPlayerController::OnSPressed);
    }
}

void AResultPlayerController::OnSpacePressed()
{
    UGameplayStatics::OpenLevel(this, FName(TEXT("LobbyMap")));
}

void AResultPlayerController::OnSPressed()
{
    UGameplayStatics::OpenLevel(this, FName(TEXT("BattleMap")));
}