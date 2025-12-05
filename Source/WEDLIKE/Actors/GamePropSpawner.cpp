// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/GamePropSpawner.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Actors/GameProp.h"
#include <GameStates/BattleGameState.h>
#include <Components/TimerComponent.h>

AGamePropSpawner::AGamePropSpawner()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = SceneComponent;

	SpawnBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnBoxComponent"));
	SpawnBoxComponent->SetupAttachment(RootComponent);

	GameStageToSpawnInterval.Add(EGameStage::WarmUp, 2.0f);
	GameStageToSpawnInterval.Add(EGameStage::Easy, 1.0f);
	GameStageToSpawnInterval.Add(EGameStage::Hard, 0.5f);
	GameStageToSpawnInterval.Add(EGameStage::FeverTime, 0.25f);
}

void AGamePropSpawner::BeginPlay()
{
	Super::BeginPlay();
	
	GetBattleGameState()->GetWaitingTimerComponent()->OnTimerEndDelegate.AddDynamic(this, &AGamePropSpawner::StartSpawning);
}

void AGamePropSpawner::StartSpawning()
{
	SpawnAndScheduleNext();
}

void AGamePropSpawner::SpawnAndScheduleNext()
{
	if (!bDoSapwn)
	{
		StopSpawning();
		return;
	}

	AGameProp* GameProp = BeginSpawnGameProp();
	GameProp = EndSpawnGameProp(GameProp);

	float SpawnInterval = GameStageToSpawnInterval[GetBattleGameState()->GetCurrentGameStage()];

	SpawnInterval = FMath::RandRange(SpawnInterval * 0.2f, SpawnInterval * 1.8f);

	GetWorldTimerManager().SetTimer(
		SpawnTimerHandle,
		this,
		&AGamePropSpawner::SpawnAndScheduleNext,
		SpawnInterval,
		false
	);
}

void AGamePropSpawner::StopSpawning()
{
	GetWorldTimerManager().SetTimerForNextTick(this, &AGamePropSpawner::ClearTimerNextTick);
}

TSubclassOf<class AGameProp> AGamePropSpawner::GetRandomClass(TArray<TSubclassOf<class AGameProp>> Classes) const
{
	if (Classes.Num() == 0)
	{
		return nullptr;
	}
	int32 RandomIndex = FMath::RandRange(0, Classes.Num() - 1);
	return Classes[RandomIndex];
}

FVector AGamePropSpawner::GetRandomPointInSpawnBox() const
{
	if (!SpawnBoxComponent)
	{
		return FVector::ZeroVector;
	}

	FVector BoxExtent = SpawnBoxComponent->GetScaledBoxExtent();
	FVector RandomPoint = FVector(
		FMath::FRandRange(-BoxExtent.X, BoxExtent.X),
		FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y),
		FMath::FRandRange(-BoxExtent.Z, BoxExtent.Z)
	);

	return SpawnBoxComponent->GetComponentTransform().TransformPosition(RandomPoint);
}


AGameProp* AGamePropSpawner::BeginSpawnGameProp()
{
	TSubclassOf<AGameProp> PropClass = GetRandomClass(GamePropClasses);

	if (PropClass)
	{
		AGameProp* GameProp = Cast<AGameProp>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, PropClass,
			FTransform(FRotator(), GetRandomPointInSpawnBox(), SpawnScale),
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn));

		return GameProp;
	}

	return nullptr;
}

AGameProp* AGamePropSpawner::EndSpawnGameProp(AGameProp* OnGoingGamePeop)
{
	UGameplayStatics::FinishSpawningActor(OnGoingGamePeop, OnGoingGamePeop->GetActorTransform());

	return OnGoingGamePeop;
}

void AGamePropSpawner::ClearTimerNextTick()
{
	GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
}

class ABattleGameState* AGamePropSpawner::GetBattleGameState()
{
	UWorld* World = GetWorld();
	if (!World) return nullptr;

	return World->GetGameState<ABattleGameState>();
}