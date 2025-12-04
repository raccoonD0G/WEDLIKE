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

// Effect Section
private:
	UFUNCTION()
	void PlayDamagedEffect(int32 NewCurrentHealth, int32 NewMaxHealth);
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AStarFountainEffect> StarFountainEffectClass;

	UFUNCTION()
	void OnDamaged(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnWallHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditAnywhere)
	float ImpulseStrength = 50000.0f;

	UPROPERTY(VisibleAnywhere)
	uint8 bAttacked : 1 = false;
};
