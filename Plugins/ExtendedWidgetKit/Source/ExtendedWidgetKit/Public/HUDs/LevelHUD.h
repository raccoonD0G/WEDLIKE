// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LevelHUD.generated.h"

/**
 * 
 */
UCLASS()
class EXTENDEDWIDGETKIT_API ALevelHUD : public AHUD
{
	GENERATED_BODY()

public:
    // 레벨(전투) UI 루트 위젯을 반환. 없으면 생성해서 뷰포트에 추가.
    UFUNCTION(BlueprintCallable, Category = "UI")
    class UUserWidget* GetLevelWidget();

	UFUNCTION(BlueprintCallable)
    void FreeMouse();

protected:
    virtual void BeginPlay() override;

    // 에디터에서 지정: UBattleWidget 블루프린트 클래스
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UUserWidget> LevelWidgetClass;

    // 실제 인스턴스(소유는 HUD가 가짐)
    UPROPERTY(Transient)
    class UUserWidget* LevelWidgetInstance = nullptr;

    // 뷰포트 ZOrder (필요하면 조절)
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    int32 LevelWidgetZOrder = 0;

private:
    void EnsureLevelWidgetCreated();
};
