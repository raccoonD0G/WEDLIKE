// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/JointPoseClassifierComponent.h"
#include "SwingClassifierComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnSwingDetectedDelegate, TArray<FTimedPoseSnapshot>, Snaps, TArray<int32>, PersonIdxOf, int32, EnterIdx, int32, ExitIdx);

/**
 * 
 */
UCLASS()
class VISIONMOTIONKIT_API USwingClassifierComponent : public UJointPoseClassifierComponent
{
	GENERATED_BODY()
	
protected:
	virtual void Detect() override;
    virtual FVector2f GetJointPos(const FTimedPoseSnapshot* Snap) override;

private:
    UPROPERTY(EditAnywhere)
    ECOCOKeypoint TargetJoint;

    bool IsCooldownActive(uint64 LocalNow) const;
    bool CollectRecentSnaps(uint64 SenderNow, double UseSec, TArray<const FTimedPoseSnapshot*>& OutSnaps) const;
    bool PassesMinDisplacementPx(const TArray<FJointSample>& U) const;
    bool BuildPersonIndexTimeline(const TArray<const FTimedPoseSnapshot*>& Snaps, TArray<int32>& OutPersonIdxOf);
    bool BuildSpeedPx(const TArray<FJointSample>& U, TArray<double>& OutSpeedPx) const;
    bool FindDirectionBreakIndexPx(const TArray<FJointSample>& U, int StartIdx, int& OutBreakIdx) const;
    bool MapWindowToSnaps(const TArray<const FTimedPoseSnapshot*>& Snaps, const TArray<FJointSample>& U, int iEnterAfter, int iExitAfter, int& OutEnterSnapIdx, int& OutExitSnapIdx) const;
    void BroadcastAndFinalize(const TArray<const FTimedPoseSnapshot*>& Snaps, const TArray<int32>& PersonIdxOf, int EnterSnapIdx, int ExitSnapIdx);

    FOnSwingDetectedDelegate OnSwingDetectedDelegate;
private:
    uint64 LastSingleSwingLoggedMs = 0;

    UPROPERTY(EditAnywhere)
    float SingleSwingCooldownSeconds = 0.0f;

    UPROPERTY(EditAnywhere)
    float SingleSwingRecentSeconds = 3.0f;

    UPROPERTY(EditAnywhere)
    double HoldFast = 0.05;     // 시작 속도가 유지돼야 하는 최소 시간 (초)

    UPROPERTY(EditAnywhere)
    double HoldStill = 0.1;    // 종료 속도가 유지돼야 하는 최소 시간 (초)

    UPROPERTY(EditAnywhere)
    float DirectionBreakAngleDeg = 45.f;          // 방향 급변으로 볼 최소 각도(도)

    UPROPERTY(EditAnywhere)
    float DirectionBreakHoldSec = 0.0f;           // 급변 상태가 유지되어야 하는 시간(초) ≈ 2프레임@30Hz

    // 스윙 최소 이동 거리(픽셀). 왕복 스윙 대비를 위해 "최대 편차" 기준을 우선 사용 권장.
    UPROPERTY(EditAnywhere)
    double MinDisplacementPx = 80.0; // FHD 근접 샷 기준 시작값

    // 스윙 판정용 평균/피크 속도(px/sec)
    UPROPERTY(EditAnywhere)
    double MinAvgSpeedPx = 0.0;

    UPROPERTY(EditAnywhere)
    double MinPeakSpeedPx = 0.0;

    // 구간 탐색용 진입/종료 속도 임계(px/sec)
    UPROPERTY(EditAnywhere)
    double EnterSpeedPx = 700.0;  // 고속 구간 진입

    UPROPERTY(EditAnywhere)
    double ExitSpeedPx = 200.0;  // 저속 구간 종료

    // 방향 급변 판정 시 최소 속도(px/sec)
    UPROPERTY(EditAnywhere)
    double DirectionBreakMinSpeedPx = 0.0;
};
