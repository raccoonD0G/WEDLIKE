// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerControllers/BattlePlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "EnhancedInputComponent.h"
#include <Subsystems/RecordingSubsystem.h>
#include <GameStates/BattleGameState.h>

void ABattlePlayerController::BeginPlay()
{
    if (ULocalPlayer* LP = GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LP))
        {
            Subsystem->AddMappingContext(IMC_Battle, /*Priority*/ 0);
        }
    }

    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
    {
        EIC->BindAction(IA_BattleQ, ETriggerEvent::Started, this, &ABattlePlayerController::OnQPressed);
    }
}

void ABattlePlayerController::OnQPressed()
{
    UWorld* World = GetWorld();
    check(World);

    ABattleGameState* BattleGameState = World->GetGameState<ABattleGameState>();
    check(BattleGameState);

    BattleGameState->OpenResultLevel();
}
