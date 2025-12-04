// Fill out your copyright notice in the Description page of Project Settings.


#include "Utilities/VisionMotionMathLibraries.h"

bool UVisionMotionMathLibraries::IsFinite2D(const FVector2f& V)
{
	return FMath::IsFinite(V.X) && FMath::IsFinite(V.Y);
}

uint64 UVisionMotionMathLibraries::LocalNowMs()
{
	if (const UWorld* W = GWorld) return (uint64)(W->GetTimeSeconds() * 1000.0);
	return (uint64)(FPlatformTime::Seconds() * 1000.0);
}

float UVisionMotionMathLibraries::SuperlinearScale(float L, float L0, float alpha = 1.6f, float blend = 1.0f)
{
	if (L0 <= KINDA_SMALL_NUMBER) return 1.0f;
	const float n = FMath::Max(0.f, L / L0);                // 정규화 길이
	const float linear = n;                                 // 선형
	const float power = FMath::Pow(n, alpha);              // 초선형
	const float mixed = FMath::Lerp(linear, power, blend); // 선형↔초선형 혼합
	return mixed;
}