// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/GameProp.h"
#include "Components/IntHealthComponent.h"
#include "Components/BoxComponent.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h"
#include "HUDs/LevelHUD.h"
#include "Widgets/BattleWidget.h"
#include "Kismet/GameplayStatics.h"
#include <NiagaraFunctionLibrary.h>
#include <NiagaraComponent.h>
#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"
#include "Components/StaticMeshComponent.h"

AGameProp::AGameProp()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	HitBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBoxComponent"));
	HitBoxComponent->SetupAttachment(RootComponent);
	HitBoxComponent->SetCollisionProfileName(TEXT("GetDamageBox"));

	IntHealthComponent = CreateDefaultSubobject<UIntHealthComponent>(TEXT("IntHealthComponent"));
	IntHealthComponent->OnHealthZeroDelegate.AddDynamic(this, &AGameProp::OnHealthZero);
	IntHealthComponent->OnHealthChangeDelegate.AddDynamic(this, &AGameProp::OnHealthChange);
}

void AGameProp::DestroySelf()
{
	GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this]()
		{
			Destroy();
		})
	);
}

void AGameProp::OnHealthChange(int32 NewCurrentHealth, int32 NewMaxHealth)
{
	if (HitEffect)
	{
		UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			HitEffect,
			GetActorLocation(),
			FRotator::ZeroRotator,
			HitEffectScale,
			true,
			true,
			ENCPoolMethod::AutoRelease,
			true);
	}
}

void AGameProp::OnHealthZero()
{
	if (DeathEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			DeathEffect,
			GetActorLocation(),
			FRotator::ZeroRotator,
			DeathEffectScale,
			true,
			true,
			ENCPoolMethod::AutoRelease,
			true
		);
	}

	if (DeathSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			DeathSound,
			GetActorLocation()
		);
	}

	DestroySelf();
}
