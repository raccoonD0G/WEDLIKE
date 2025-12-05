// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/MeshProp.h"
#include "PhysicsProp.generated.h"

/**
 * 
 */
UCLASS()
class WEDLIKE_API APhysicsProp : public AMeshProp
{
	GENERATED_BODY()
	
public:
	APhysicsProp();

// Physics Section
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UBoxComponent> PhysicsBoxComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USceneComponent> PhysicsRootComponent;

private:
	UFUNCTION()
	void OnWallHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnDamaged(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere)
	float ImpulseStrength = 50000.0f;

	UPROPERTY(VisibleAnywhere)
	uint8 bAttacked : 1 = false;
};
