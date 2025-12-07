// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/PoseFollower.h"


// Sets default values
APoseFollower::APoseFollower()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APoseFollower::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APoseFollower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

