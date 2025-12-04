// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/MeshProp.h"
#include "Components/StaticMeshComponent.h"

AMeshProp::AMeshProp()
{
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(RootComponent);
	StaticMeshComponent->SetCollisionProfileName(TEXT("NoCollision"));
}