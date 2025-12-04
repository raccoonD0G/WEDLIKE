// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enums/PropType.h"
#include "GameProp.generated.h"


UCLASS()
class WEDLIKE_API AGameProp : public AActor
{
	GENERATED_BODY()
	
public:	
	AGameProp();

private:
	UFUNCTION()
	void DestroySelf();

// Type Section
public:
	FORCEINLINE EPropType GetPropType() const { return PropType; }

private:
	UPROPERTY(EditAnywhere)
	EPropType PropType = EPropType::OriginalRamyun;

// Health Section
public:
	FORCEINLINE class UIntHealthComponent* GetIntHealthComponent() const { return IntHealthComponent; }
	FORCEINLINE class UBoxComponent* GetHitBoxComponent() const { return HitBoxComponent; }

protected:
	UFUNCTION()
	virtual void OnHealthZero();

	UFUNCTION()
	void OnHealthChange(int32 NewCurrentHealth, int32 NewMaxHealth);

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UIntHealthComponent> IntHealthComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UBoxComponent> HitBoxComponent;



// Niagara Section
private:
	UPROPERTY(EditAnywhere)
	FVector HitEffectScale = FVector(0.1, 0.1, 0.1);

	UPROPERTY(EditAnywhere)
	FVector DeathEffectScale = FVector(0.5, 0.5, 0.5);

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UNiagaraSystem> HitEffect;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UNiagaraSystem> DeathEffect;

// Sound Section
private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<class USoundBase> DeathSound;


};
