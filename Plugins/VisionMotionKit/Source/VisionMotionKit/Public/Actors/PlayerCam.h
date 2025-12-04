// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "PlayerCam.generated.h"

/**
 * 
 */
UCLASS()
class VISIONMOTIONKIT_API APlayerCam : public ACameraActor
{
	GENERATED_BODY()
	
public:
	APlayerCam();

protected:
	virtual void BeginPlay() override;
};
