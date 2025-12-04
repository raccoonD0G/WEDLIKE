// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/HealthBarWidget.h"
#include "Components/IntHealthComponent.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ProgressBar.h"

void UHealthBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC) return;

	APlayerState* PS = PC->GetPlayerState<APlayerState>();
	if (!PS) return;

	UIntHealthComponent* HealthComponent = PS->FindComponentByClass<UIntHealthComponent>();
	if (!HealthComponent) return;

	HealthComponent->OnHealthChangeDelegate.AddDynamic(this, &UHealthBarWidget::SetHealthProgressBarInt);
	SetHealthProgressBar(HealthComponent->GetCurrentHealth(), HealthComponent->GetMaxHealth());
}

void UHealthBarWidget::SetHealthProgressBar(float CurrentHealth, float MaxHealth)
{
	const float Ratio = (MaxHealth > 0.f) ? CurrentHealth / MaxHealth : 0.f;
	HealthProgressBar->SetPercent(Ratio);
}

void UHealthBarWidget::SetHealthProgressBarInt(int32 CurrentHealth, int32 MaxHealth)
{
	SetHealthProgressBar(static_cast<float>(CurrentHealth), static_cast<float>(MaxHealth));
}
