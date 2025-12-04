// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/AttackEffect.h"
#include "Components/BoxComponent.h"
#include "Actors/GameProp.h"
#include "Components/IntHealthComponent.h"


AAttackEffect::AAttackEffect()
{
    PlaneBox->OnComponentBeginOverlap.AddDynamic(this, &AAttackEffect::OnPlaneBoxBeginOverlap);
    PlaneBox->SetCollisionProfileName(TEXT("DoDamageBox"));
}

void AAttackEffect::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this]()
            {
                if (!IsValid(PlaneBox)) return;
                PlaneBox->SetCollisionProfileName(TEXT("NoCollision"));
            })
        );
    }
}

void AAttackEffect::OnPlaneBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor || OtherActor == this) return;

    // 같은 액터에 중복 데미지 방지
    if (DamagedActors.Contains(OtherActor)) return;

    AGameProp* FallingProp = Cast<AGameProp>(OtherActor);
    if (!FallingProp) return;

    FallingProp->GetIntHealthComponent()->DoIntHealthDamage(1);

    DamagedActors.Add(OtherActor);
}