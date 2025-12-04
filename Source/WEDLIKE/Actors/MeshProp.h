// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/GameProp.h"
#include "MeshProp.generated.h"

/**
 * 
 */
UCLASS()
class WEDLIKE_API AMeshProp : public AGameProp
{
	GENERATED_BODY()
	
public:
	AMeshProp();

// Mesh Section
protected:
	FORCEINLINE class UStaticMeshComponent* GetStaticMeshComponent() const { return StaticMeshComponent; }

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UStaticMeshComponent> StaticMeshComponent;
	
	
};
