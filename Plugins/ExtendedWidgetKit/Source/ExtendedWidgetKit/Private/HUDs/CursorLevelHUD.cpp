// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDs/CursorLevelHUD.h"
#include "Blueprint/UserWidget.h"


void ACursorLevelHUD::BeginPlay()
{
	Super::BeginPlay();

    APlayerController* PC = PlayerOwner;
    if (!PC) return;

    FInputModeGameAndUI InputMode;
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    InputMode.SetHideCursorDuringCapture(true);

    if (GetLevelWidget())
    {
        InputMode.SetWidgetToFocus(GetLevelWidget()->TakeWidget());
    }

    PC->SetInputMode(InputMode);
    EnableInput(PC);
    PC->bShowMouseCursor = true;
}