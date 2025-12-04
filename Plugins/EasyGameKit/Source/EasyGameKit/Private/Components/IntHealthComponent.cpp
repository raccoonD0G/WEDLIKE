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

void UIntHealthComponent::DoIntHealthDamage(int32 DamageAmount)
{
	if (CurrentHealth <= 0) return;

	CurrentHealth -= DamageAmount;
	OnHealthChangeDelegate.Broadcast(CurrentHealth, MaxHealth);

	if (CurrentHealth <= 0)
	{
		OnHealthZeroDelegate.Broadcast();
	}

}