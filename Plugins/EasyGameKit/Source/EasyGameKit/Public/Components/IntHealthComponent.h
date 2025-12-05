// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "IntHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealthZeroDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChangeDelegate, int32, NewCurrentHealth, int32, NewMaxHealth);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class EASYGAMEKIT_API UIntHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UIntHealthComponent();

protected:
	virtual void BeginPlay() override;

// Health Section
public:
	FORCEINLINE int32 GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE int32 GetCurrentHealth() const { return CurrentHealth; }

	void SetMaxHealth(int32 NewMaxHealth);

	void SetFullHealth();

	FOnHealthZeroDelegate OnHealthZeroDelegate;
	FOnHealthChangeDelegate OnHealthChangeDelegate;

	void DoIntHealthDamage(int32 DamageAmount);

private:

	void SetCurrentHealth(int32 NewCurrentHealth);

	UPROPERTY(EditAnywhere, Category = "Health")
	int32 MaxHealth = 3;

	UPROPERTY(VisibleAnywhere, Category = "Health")
	int32 CurrentHealth;
};
