// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CameraBoundsClampComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VISIONMOTIONKIT_API UCameraBoundsClampComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCameraBoundsClampComponent();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;

    /** 외부에서 초기화 (스프링암과 박스 바인딩) */
	UFUNCTION(BlueprintCallable, Category = "Clamp")
    void Init(UBoxComponent* InBoundsBox, USpringArmComponent* InSpringArm);

    /** 매 프레임 클램프 실행 */
    void ClampNow();

protected:
    virtual void BeginPlay() override;

private:
    /** 클램프 기준 박스 */
    UPROPERTY(EditAnywhere, Category = "Clamp")
    TObjectPtr<UBoxComponent> BoundsBox = nullptr;

    /** 타겟 스프링암 */
    UPROPERTY(EditAnywhere, Category = "Clamp")
    TObjectPtr<USpringArmComponent> SpringArm = nullptr;
		
};
