#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EGameStage : uint8
{
	WarmUp,
	Easy,
	Hard,
	FeverTime
};