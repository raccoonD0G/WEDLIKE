// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/SingleSwingEffect.h"
#include "AttackEffect.generated.h"

/**
 * 
 */
UCLASS()
class WEDLIKE_API AAttackEffect : public ASingleSwingEffect
{
	GENERATED_BODY()
	
public:
	AAttackEffect();

protected:
	virtual void PostInitializeComponents() override;

private:
	UPROPERTY(Transient)
	TSet<TWeakObjectPtr<AActor>> DamagedActors;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY(EditAnywhere)
	float DamageAmount = 25.f;

	UFUNCTION()
	void OnPlaneBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
