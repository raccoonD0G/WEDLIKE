// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProjectileFallingComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class EASYGAMEKIT_API UProjectileFallingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UProjectileFallingComponent();
		
public:
	void Init(float InSpeed);

private:
	UPROPERTY()
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditAnywhere)
	float SpeedMultiplier = 1.0f;
};
