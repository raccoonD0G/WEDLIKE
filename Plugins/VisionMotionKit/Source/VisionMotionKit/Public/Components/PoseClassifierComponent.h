// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PoseUdpReceiverComponent.h"
#include "PoseClassifierComponent.generated.h"

// 타임스탬프가 붙은 스냅샷(한 번의 수신에서의 모든 사람 포즈)
USTRUCT(BlueprintType)
struct FTimedPoseSnapshot
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	uint64 TimestampMs = 0; // 송신측 ms

	UPROPERTY(VisibleAnywhere)
	TArray<FPersonPose> Poses;

    UPROPERTY(VisibleAnywhere)
    TArray<FHandPose> Hands;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPoseWindowUpdated);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VISIONMOTIONKIT_API UPoseClassifierComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPoseClassifierComponent();

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    
// Window Section
public:
	FORCEINLINE UPoseUdpReceiverComponent* GetReceiver() const { return Receiver; }

    /** 윈도우에 새로운 스냅샷이 반영될 때마다 브로드캐스트 */
    UPROPERTY(BlueprintAssignable, Category = "Pose")
    FOnPoseWindowUpdated OnPoseWindowUpdated;

    /** 현재 윈도우(최근 2초)의 스냅샷들을 읽기 전용으로 반환 */
    UFUNCTION(BlueprintCallable, Category = "Pose")
    const TArray<FTimedPoseSnapshot> GetWindow() const { return WindowBuffer; }

    /** 현재 윈도우 안의 총 사람 수(중복 포함, 스냅샷마다 합산) */
    UFUNCTION(BlueprintCallable, Category = "Pose|Stats")
    int32 GetWindowTotalPersons() const;

    /** 윈도우 안의 최신 스냅샷에서 첫 번째 사람의 특정 관절 좌표(없으면 false) */
    UFUNCTION(BlueprintCallable, Category = "Pose|Query")
    bool GetLatestJoint(int32 CocoIndex, FVector2D& OutXY) const;

    /** (예시) 윈도우 내에서 가장 자주 등장한 personId (동점 시 임의) — 없으면 -1 */
    UFUNCTION(BlueprintCallable, Category = "Pose|Stats")
    int32 GetDominantPersonId() const;

protected:
    // 내부 버퍼: 최근 수 초의 스냅샷
    TArray<FTimedPoseSnapshot> WindowBuffer;
    /** 리시버 참조 (동일 액터에 붙어 있으면 자동 탐색, 아니면 에디터에서 세팅) */
    UPROPERTY(EditAnywhere, Category = "Pose")
    TObjectPtr<UPoseUdpReceiverComponent> Receiver;

    /** 윈도우 길이(초) */
    UPROPERTY(EditAnywhere, Category = "Pose")
    float WindowSeconds = 10.0f;

private:
    // 중복 삽입 방지용 마지막 타임스탬프 캐시
    uint64 LastIngestedTs = 0;

// Util Section
public:
    bool PickBestPerson(const TArray<FPersonPose>& Poses, int32& OutIdx) const;
    void IngestLatestFromReceiver();
    void PruneOld(uint64 NowMs);
    bool GetShoulderW(const FPersonPose& P, float& OutShoulderW) const;
    // 현재(수신 기준) ms — Receiver의 LatestTimestamp를 Now로 사용
    uint64 NowMsFromReceiver() const;


// Detect Section
public:

    FORCEINLINE float GetPixelToUU() const { return Receiver->GetPixelToUU(); }
    FORCEINLINE float GetDepthOffsetX() const { return Receiver->GetDepthOffsetX(); }
    FORCEINLINE bool GetIsInvertImageYToUp() const { return Receiver->GetInvertImageYToUp(); }

    bool GetCameraView(FVector& OutCamLoc, FVector& OutCamForward) const;

// Classify Section
protected:
    virtual void Detect();

private:
    bool ComputeShoulderWidthPx(const FPersonPose& P, float& OutShoulderW) const;

// Detect Section
protected:
    static constexpr double KFixedHz = 30.0;
    static constexpr double KFixedDt = 1.0 / KFixedHz;

    bool FindRun(const TArray<double>& SpeedNorm, int StartIdx, double Thr, double HoldSec, bool bAbove, int& OutIdx) const;

    
};