// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/HandFollower.h"
#include "SingleSwingFollower.generated.h"

/**
 * 
 */
UCLASS()
class VISIONMOTIONKIT_API ASingleSwingFollower : public AHandFollower
{
	GENERATED_BODY()

public:
	ASingleSwingFollower();
    virtual void SetHandPoseClassifierComponent(class UHandPoseClassifierComponent* InHandPoseClassifierComponent) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<class USingleSwingClassifierComponent> SingleSwingClassifierComponent;

// Attack Section
private:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    TSubclassOf<class ASingleSwingEffect> SingleSwingEffectClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    uint8 bDoDebug : 1 = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    float SingleSwingDamageAmount = 2.f;

    UPROPERTY(EditAnywhere)
    float AttackLifeSeconds = 1.f;

    UFUNCTION()
    void HandleSingleSwingDetected(TArray<FTimedPoseSnapshot> Snaps, TArray<int32> PersonIdxOf, EHandSide InHandSide, int32 EnterIdx, int32 ExitIdx);

};
