// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/IntHealthComponent.h"


UIntHealthComponent::UIntHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UIntHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
}

void UIntHealthComponent::SetMaxHealth(int32 NewMaxHealth)
{
	MaxHealth = NewMaxHealth;

}

void UIntHealthComponent::SetFullHealth()
{
	SetCurrentHealth(MaxHealth);
}

void UIntHealthComponent::DoIntHealthDamage(int32 DamageAmount)
{
	if (CurrentHealth <= 0) return;

	SetCurrentHealth(CurrentHealth - DamageAmount);

	if (CurrentHealth <= 0)
	{
		OnHealthZeroDelegate.Broadcast();
	}

}

void UIntHealthComponent::SetCurrentHealth(int32 NewCurrentHealth)
{
	CurrentHealth = NewCurrentHealth;
	OnHealthChangeDelegate.Broadcast(CurrentHealth, MaxHealth);
}
