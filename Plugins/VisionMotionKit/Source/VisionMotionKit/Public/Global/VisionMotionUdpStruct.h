#pragma once

#include "CoreMinimal.h"
#include "VisionMotionUdpStruct.generated.h"

USTRUCT(BlueprintType)
struct FPersonPose
{
    GENERATED_BODY()

public:
    uint16 PersonId;
    TArray<FVector2f> XY;   // 17
    TArray<float> Conf;     // 17
};

USTRUCT(BlueprintType)
struct FHandPose
{
    GENERATED_BODY()

public:
    uint16 PersonId = 0xFFFF; // 0xFFFF = unknown
    uint8  Which = 2;      // 0=left, 1=right, 2=unknown
    FVector2f Center = FVector2f(NAN, NAN);
    TArray<FVector2f> XY;     // 21
    TArray<float>     Conf;   // 21
};