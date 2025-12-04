// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/HandPoseClassifierComponent.h"
#include "SingleSwingClassifierComponent.generated.h"


// 맨 위 쪽
DECLARE_LOG_CATEGORY_EXTERN(LogSingleSwing, Log, All);

UENUM()
enum class EExitCause : uint8
{
    LowSpeed,
    DirectionBreak,
    Unknown
};

USTRUCT()
struct FSingleSwingTrace
{
    GENERATED_BODY()

    // 타임라인/윈도우
    double UseSec = 0.0;
    int32 NumSnaps = 0;
    int32 NumU = 0;

    // 손/인물
    bool bRight = false;
    int32 PersonEnter = -1;
    int32 PersonExit = -1;

    // 1) 변위(왕복 대비) 통과 여부(내부 판단만 알 수 있으면 bool만)
    bool bPassMinDisp = false;
    double MaxDeviationPx = 0.0;   // 가능하면 PassesMinDisplacementPx 내부에서 받아오기
    double PathLenPx = 0.0;        // 가능하면 Calc 시점에 채우기
    double MinDispThresholdPx = 0.0;

    // 2) 속도 메트릭
    double AvgSpeedPx = 0.0;
    double PeakSpeedPx = 0.0;
    double MinAvgSpeedPx = 0.0;
    double MinPeakSpeedPx = 0.0;

    // 3) 속도 시퀀스 & 구간
    double EnterSpeedPx = 0.0;
    int32  HoldFast = 0;
    double ExitSpeedPx = 0.0;
    int32  HoldStill = 0;

    int32 IEnter = -1;
    int32 IExitLow = -1;
    int32 IBreak = -1;
    int32 IExit = -1;
    EExitCause ExitCause = EExitCause::Unknown;

    // 매핑/시간
    int32 EnterSnapIdx = -1;
    int32 ExitSnapIdx = -1;
    uint64 LocalNowMs = 0;
    uint64 SenderNowMs = 0;

    // 디버그 스위치
    bool bEnabled = true;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnSingleSwingDetected, TArray<FTimedPoseSnapshot>, Snaps, TArray<int32>, PersonIdxOf, EHandSide, InHandSide, int32, EnterIdx, int32, ExitIdx);

/**
 * 
 */
UCLASS()
class VISIONMOTIONKIT_API USingleSwingClassifierComponent : public UHandPoseClassifierComponent
{
	GENERATED_BODY()

public:
	FOnSingleSwingDetected OnSingleSwingDetected;

protected:
    virtual void Detect() override;

// Single Swing Section
private:
    uint64 LastSingleSwingLoggedMs = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pose|Swing", meta = (AllowPrivateAccess = "true"))
    float SingleSwingCooldownSeconds = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pose|Swing", meta = (AllowPrivateAccess = "true"))
    float SingleSwingRecentSeconds = 3.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pose|Swing", meta = (AllowPrivateAccess = "true"))
    double HoldFast = 0.05;     // 시작 속도가 유지돼야 하는 최소 시간 (초)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pose|Swing", meta = (AllowPrivateAccess = "true"))
    double HoldStill = 0.1;    // 종료 속도가 유지돼야 하는 최소 시간 (초)

    UPROPERTY(EditAnywhere, Category = "Pose|Swing")
    float DirectionBreakAngleDeg = 45.f;          // 방향 급변으로 볼 최소 각도(도)

    UPROPERTY(EditAnywhere, Category = "Pose|Swing")
    float DirectionBreakHoldSec = 0.0f;           // 급변 상태가 유지되어야 하는 시간(초) ≈ 2프레임@30Hz
    
    // 스윙 최소 이동 거리(픽셀). 왕복 스윙 대비를 위해 "최대 편차" 기준을 우선 사용 권장.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pose|Swing", meta = (AllowPrivateAccess = "true"))
    double MinDisplacementPx = 80.0; // FHD 근접 샷 기준 시작값

    // 스윙 판정용 평균/피크 속도(px/sec)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pose|Swing", meta = (AllowPrivateAccess = "true"))
    double MinAvgSpeedPx = 0.0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pose|Swing", meta = (AllowPrivateAccess = "true"))
    double MinPeakSpeedPx = 0.0;

    // 구간 탐색용 진입/종료 속도 임계(px/sec)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pose|Swing", meta = (AllowPrivateAccess = "true"))
    double EnterSpeedPx = 700.0;  // 고속 구간 진입

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pose|Swing", meta = (AllowPrivateAccess = "true"))
    double ExitSpeedPx = 200.0;  // 저속 구간 종료

    // 방향 급변 판정 시 최소 속도(px/sec)
    UPROPERTY(EditAnywhere, Category = "Pose|Swing")
    double DirectionBreakMinSpeedPx = 0.0;

    UPROPERTY(EditAnywhere, Category = "Debug")
    bool bLogSingleSwingReasons = true;

private:
    /// <brief>쿨다운 타임아웃이 남아있는지 확인합니다.</brief>
    /// <param name="LocalNow">로컬 시각(ms).</param>
    /// <return>쿨다운 진행 중이면 true.</return>
    /// <details>마지막 감지 시각과 비교해 SingleSwingCooldownSeconds를 적용합니다.</details>
    bool IsCooldownActive(uint64 LocalNow) const;

    /// <brief>최근 구간(UseSec) 조건을 만족하는 스냅샷만 수집합니다.</brief>
    /// <param name="SenderNow">송신자 기준 현재 시각(ms).</param>
    /// <param name="UseSec">사용할 최근 구간(초).</param>
    /// <param name="OutSnaps">필터링된 스냅샷 포인터 배열.</param>
    /// <return>수집 성공 여부.</return>
    /// <details>WindowBuffer에서 타임스탬프와 포즈 개수 조건으로 필터링합니다.</details>
    bool CollectRecentSnaps(uint64 SenderNow, double UseSec, TArray<const FTimedPoseSnapshot*>& OutSnaps) const;

    /// <brief>스냅샷들로부터 FHandSample 시퀀스를 생성합니다.</brief>
    /// <param name="Snaps">입력 스냅샷 포인터 배열.</param>
    /// <param name="OutSamples">출력 샘플 배열.</param>
    /// <return>샘플 수가 3개 이상이면 true.</return>
    /// <details>대표 인물 선택, 어깨중앙/손목 좌표 산출, ShoulderW 확보 및 핸드 보정(Hands)까지 수행합니다.</details>
    bool BuildSamples(const TArray<const FTimedPoseSnapshot*>& Snaps, TArray<FHandSample>& OutSamples);

    /// <brief>샘플을 30Hz 균일 시계열로 리샘플하고 EMA 스무딩을 적용합니다.</brief>
    /// <param name="InSamples">원본 샘플.</param>
    /// <param name="OutU">균일 시계열 출력.</param>
    /// <return>U 길이가 3 이상이면 true.</return>
    /// <details>ResampleUniform 후 ApplyEMA를 호출합니다.</details>
    bool ResampleAndSmooth(const TArray<FHandSample>& InSamples, TArray<FHandSample>& OutU) const;

    /// <brief>평균 어깨 폭(MeanSW)을 계산합니다.</brief>
    /// <param name="U">균일 시계열.</param>
    /// <param name="OutMeanSW">평균 어깨폭.</param>
    /// <return>유효한 평균이면 true.</return>
    /// <details>0 방지를 위해 KINDA_SMALL_NUMBER로 클램프 후 평균을 취합니다.</details>
    bool ComputeMeanShoulderWidth(const TArray<FHandSample>& U, double& OutMeanSW) const;

    /// <brief>U 인덱스(iEnterAfter/iExitAfter)를 원본 스냅샷 인덱스로 매핑합니다.</brief>
    /// <param name="Snaps">스냅샷 포인터 배열.</param>
    /// <param name="U">균일 시계열.</param>
    /// <param name="iEnterAfter">U 기준 진입 이후 인덱스.</param>
    /// <param name="iExitAfter">U 기준 종료 이후 인덱스.</param>
    /// <param name="OutEnterSnapIdx">원본 진입 인덱스.</param>
    /// <param name="OutExitSnapIdx">원본 종료 인덱스.</param>
    /// <return>성공 시 true.</return>
    /// <details>투 포인터 매핑(MapUToSnapIndex)으로 최근접 스냅을 선택합니다.</details>
    bool MapWindowToSnaps(const TArray<const FTimedPoseSnapshot*>& Snaps, const TArray<FHandSample>& U, int iEnterAfter, int iExitAfter, int& OutEnterSnapIdx, int& OutExitSnapIdx) const;

    /// <brief>각 스냅에 대한 대표 PersonIdx를 고정합니다.</brief>
    /// <param name="Snaps">스냅샷 포인터 배열.</param>
    /// <param name="OutPersonIdxOf">스냅별 대표 인덱스 배열.</param>
    /// <return>항상 true.</return>
    /// <details>PickBestPerson으로 스냅마다 대표 인물을 선택합니다.</details>
    bool BuildPersonIndexTimeline(const TArray<const FTimedPoseSnapshot*>& Snaps, TArray<int32>& OutPersonIdxOf);

    /// <brief>이벤트 브로드캐스트와 정리를 수행합니다.</brief>
    /// <param name="Snaps">스냅샷 포인터 배열.</param>
    /// <param name="PersonIdxOf">스냅별 대표 인덱스.</param>
    /// <param name="InHandSide">사용 손.</param>
    /// <param name="EnterSnapIdx">원본 진입 인덱스.</param>
    /// <param name="ExitSnapIdx">원본 종료 인덱스.</param>
    /// <return>없음.</return>
    /// <details>값 복사 후 OnSingleSwingDetected 브로드캐스트, 쿨다운 갱신 및 버퍼 리셋.</details>
    void BroadcastAndFinalize(const TArray<const FTimedPoseSnapshot*>& Snaps, const TArray<int32>& PersonIdxOf, int EnterSnapIdx, int ExitSnapIdx);

    bool PassesMinDisplacementPx(const TArray<FHandSample>& U) const;

    bool BuildSpeedPx(const TArray<FHandSample>& U, TArray<double>& OutSpeedPx) const;

    FHandMetrics CalcMetricsPx(const TArray<FHandSample>& U, bool bLeft) const;

    bool FindDirectionBreakIndexPx(const TArray<FHandSample>& U, int StartIdx, int& OutBreakIdx) const;

};
