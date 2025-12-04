// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SwingComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class EASYGAMEKIT_API USwingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USwingComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
// Swing Section
public:
	void Init(class USceneComponent* NewUpdatedComponent);

	FORCEINLINE void SetDoSwing(bool bInDoSwing) { bDoSwing = bInDoSwing; }

	void SetUpdatedComponent(class USceneComponent* NewUpdatedComponent);

	void StartSwing();

	void StopSwing();

protected:
	void SwingTick(float DeltaTime);

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USceneComponent> UpdatedComponent;

	UPROPERTY(EditAnywhere)
	uint8 bDoSwing : 1 = false;

	UPROPERTY(EditAnywhere)
	float SwingAmplitudeDeg = 30.f;

	UPROPERTY(EditAnywhere)
	float SwingSpeed = 1.5f;

	float ElapsedTime = 0.f;

	float StartSide;
	
};
