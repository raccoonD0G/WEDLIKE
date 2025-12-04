// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/PlayerCam.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"

APlayerCam::APlayerCam()
{
	GetCameraComponent()->AspectRatio = 9.f / 16.f;
}

void APlayerCam::BeginPlay()
{
	Super::BeginPlay();
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		PC->SetViewTargetWithBlend(this, /*BlendTime=*/0.f, VTBlend_Linear, /*BlendExp=*/0.f, /*bLockOutgoing=*/false);
	}
}
