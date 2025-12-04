// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PoseClassifierComponent.h"
#include "HandPoseClassifierComponent.generated.h"

UENUM(BlueprintType)
enum class EHandSide : uint8
{
    Right  UMETA(DisplayName = "Right"),
    Left   UMETA(DisplayName = "Left"),
};

USTRUCT(BlueprintType)
struct FHandSample
{
    GENERATED_BODY()

    double    T = 0.0;
    FVector2D L = FVector2D::ZeroVector;
    FVector2D R = FVector2D::ZeroVector;
    FVector2D C = FVector2D::ZeroVector;
    float     ShoulderW = 0.f;
};

USTRUCT(BlueprintType)
struct FHandMetrics
{
    GENERATED_BODY()

    double Peak = 0.0;
    double Avg = 0.0;
};

/**
 * 
 */
UCLASS()
class VISIONMOTIONKIT_API UHandPoseClassifierComponent : public UPoseClassifierComponent
{
	GENERATED_BODY()

// Hand Section
public:
    FORCEINLINE EHandSide GetCurrentHandSide() const { return CurrentHandSide; }
    FORCEINLINE void SetCurrentHandSide(EHandSide NewHand) { CurrentHandSide = NewHand; }

protected:
    /// <brief>Hands 목록에서 특정 사람/손에 해당하는 손 중심 좌표를 찾고, 실패 시 손목 근처 후보로 폴백하여 가장 가까운 손을 선택합니다.</brief>
    /// <param name="Hands">감지된 손 포즈 리스트.</param>
    /// <param name="PersonId">대상 사람 ID.</param>
    /// <param name="Which">손 구분(0=Left, 1=Right, 2=Unknown 허용).</param>
    /// <param name="FallbackNearTo">폴백 시 근접성 계산에 사용할 기준점(보통 손목 좌표).</param>
    /// <param name="OutCenter">선택된 손의 중심 좌표(성공 시 설정).</param>
    /// <return>유효한 손을 찾으면 true, 아니면 false.</return>
    /// <details>
    /// 선택 규칙(유효 좌표만 대상):
    /// 1) PersonId & Which 완전 일치 항목을 우선 선택.
    /// 2) 없으면 동일 PersonId 내에서 FallbackNearTo에 가장 가까운 손을 선택(Which 무시/Unknown 허용).
    /// 3) 그래도 없으면 전체 Hands 중 FallbackNearTo에 가장 가까운 손을 선택.
    /// 어떤 단계에서도 후보가 없으면 false를 반환합니다.
    /// 거리 계산은 2D 유클리드 거리이며, 좌표가 유한(NaN/Inf 아님)인 경우만 고려됩니다.
    /// </details>
    bool FindHandFor(const TArray<FHandPose>& Hands, uint16 PersonId, uint8 Which, const FVector2f& FallbackNearTo, FVector2D& OutCenter) const;

    /// <brief>균일 시계열에서 선택 손(왼/오)의 정규화 속도 메트릭(최대/평균)을 계산합니다.</brief>
    /// <param name="U">리샘플/스무딩된 균일 시계열 샘플(시간 오름차순).</param>
    /// <param name="MeanSW">정규화 분모로 사용할 평균 어깨폭(>0 권장).</param>
    /// <param name="bLeft">true=왼손 기준, false=오른손 기준.</param>
    /// <return>FHandMetrics { Peak, Avg }.</return>
    /// <details>
    /// v_i = |p_i - p_{i-1}| / KFixedDt / MeanSW 로 각 프레임 속도를 계산하고,
    /// 그 중 최대(Peak)와 시간 평균(Avg)을 반환합니다. Open/Close 게이트는 사용하지 않습니다.
    /// 입력은 KFixedDt 간격으로 정렬되어 있다고 가정합니다.
    /// </details>
    // FHandMetrics CalcMetrics(const TArray<FHandSample>& U, double MeanSW, bool bLeft) const;

    /// <brief>선택된 손(오른손/왼손)의 i번째 손목 좌표를 반환합니다.</brief>
    /// <param name="U">균일 시계열 샘플.</param>
    /// <param name="bRight">true=오른손, false=왼손.</param>
    /// <param name="i">인덱스(0 ≤ i &lt; U.Num()).</param>
    /// <return>U[i].R 또는 U[i].L.</return>
    /// <details>호출 측에서 유효 인덱스를 보장해야 합니다.</details>
    FORCEINLINE FVector2D WristAt(const TArray<FHandSample>& U, int i) const{ return GetCurrentHandSide() == EHandSide::Right ? U[i].R : U[i].L; }

    /// <brief>L/R/C 좌표에 지수가중이동평균(EMA)을 인플레이스로 적용합니다.</brief>
    /// <param name="U">균일 시계열 샘플(시간 오름차순). L/R/C가 이 함수 내에서 갱신됩니다.</param>
    /// <return>없음.</return>
    /// <details>
    /// 현재 구현은 Alpha=1.0로 효과가 없도록 설정되어 있습니다. 스무딩이 필요하면 Alpha를 낮추세요(예: 0.2~0.5).
    /// ShoulderW에는 EMA를 적용하지 않습니다.
    /// </details>
    void ApplyEMA(TArray<FHandSample>& U) const; // 알파=1.0 (원래 코드 유지)

    /// <brief>가변 간격 샘플을 KFixedHz(예: 30Hz) 균일 시계열로 선형 보간합니다.</brief>
    /// <param name="In">원본 샘플(시간 오름차순, 2개 이상).</param>
    /// <return>등간격(KFixedDt)의 새 샘플 배열.</return>
    /// <details>
    /// 출력 시각 t = t0 + i*KFixedDt. 각 성분(T, L, R, C, ShoulderW)을 j,k 프레임 사이에서 선형 보간합니다.
    /// 큰 불연속 구간에서는 보간이 과도할 수 있으므로 사전 아웃라이어 제거를 권장합니다.
    /// </details>
    TArray<FHandSample> ResampleUniform(const TArray<FHandSample>& In) const;

    /// <brief>U[Ui].T에 가장 가까운 원본 스냅샷 시간 인덱스를 반환합니다.</brief>
    /// <param name="SnapTimes">원본 스냅샷 시각(초) 배열, 오름차순.</param>
    /// <param name="U">균일 시계열 샘플 배열(오름차순).</param>
    /// <param name="Ui">매핑할 U의 인덱스(0 ≤ Ui &lt; U.Num()).</param>
    /// <param name="Cursor">전진 전용 커서(참조). 연속 호출 시 재사용하여 선형 시간으로 처리.</param>
    /// <return>최근접 스냅샷의 인덱스.</return>
    /// <details>
    /// 투 포인터 방식으로 SnapTimes[Cursor] ≤ t &lt; SnapTimes[Cursor+1]을 만들고,
    /// 양쪽 후보 중 t에 더 가까운 쪽을 선택합니다. 경계에서는 자동으로 0 또는 마지막 인덱스로 클램프됩니다.
    /// </details>
    int MapUToSnapIndex(const TArray<double>& SnapTimes, const TArray<FHandSample>& U, int Ui, int& Cursor) const;

private:
    // 어느 손을 사용할지
    UPROPERTY(EditAnywhere, Category = "Pose")
    EHandSide CurrentHandSide = EHandSide::Right;
};
