#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VisionMotionMathLibraries.generated.h"


UCLASS()
class VISIONMOTIONKIT_API UVisionMotionMathLibraries : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static bool IsFinite2D(const FVector2f& V);

	static uint64 LocalNowMs();

	static float SuperlinearScale(float L, float L0, float alpha, float blend);
};