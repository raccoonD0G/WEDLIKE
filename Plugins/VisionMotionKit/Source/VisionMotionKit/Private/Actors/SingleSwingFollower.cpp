// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/SingleSwingFollower.h"
#include "Actors/MirroredPlayer.h"
#include "Components/PoseUdpReceiverComponent.h"
#include "Components/SingleSwingClassifierComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Actors/SingleSwingEffect.h"
#include "Utilities/VisionMotionUtils.h"
#include "Utilities/VisionMotionMathLibraries.h"
#include "Global/VisionMotionUdpStruct.h"

ASingleSwingFollower::ASingleSwingFollower()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ASingleSwingFollower::SetHandPoseClassifierComponent(class UHandPoseClassifierComponent* InHandPoseClassifierComponent)
{
	Super::SetHandPoseClassifierComponent(InHandPoseClassifierComponent);
    USingleSwingClassifierComponent* Comp = Cast<USingleSwingClassifierComponent>(InHandPoseClassifierComponent);

	check(Comp);

	SingleSwingClassifierComponent = Comp;
}

void ASingleSwingFollower::BeginPlay()
{
	Super::BeginPlay();

    if (SingleSwingClassifierComponent)
    {
        SingleSwingClassifierComponent->OnSingleSwingDetected.AddDynamic(this, &ASingleSwingFollower::HandleSingleSwingDetected);
    }
}

void ASingleSwingFollower::HandleSingleSwingDetected(TArray<FTimedPoseSnapshot> Snaps, TArray<int32> PersonIdxOf, EHandSide InHandSide, int32 EnterIdx, int32 ExitIdx)
{
    UWorld* World = GetWorld();
    if (!World || Snaps.Num() < 3) return;
    if (PersonIdxOf.Num() != Snaps.Num()) return;

    // 1) 카메라/오너 축
    FVector CamLoc, CamFwd;
    if (!SingleSwingClassifierComponent || !SingleSwingClassifierComponent->GetCameraView(CamLoc, CamFwd)) return;

    // 원본과 동일한 기준 트랜스폼 사용
    const FTransform OwnerXf =
        (SingleSwingClassifierComponent && SingleSwingClassifierComponent->GetOwner())
        ? SingleSwingClassifierComponent->GetOwner()->GetActorTransform()
        : GetActorTransform();

    // COCO 인덱스
    const int32 IdxWr = InHandSide == EHandSide::Right ? MotionCapture::COCO_RWR : MotionCapture::COCO_LWR;
    const int32 IdxEl = InHandSide == EHandSide::Right ? MotionCapture::COCO_REL : MotionCapture::COCO_LEL;

    // 안전 클램프
    EnterIdx = FMath::Clamp(EnterIdx, 0, Snaps.Num() - 1);
    ExitIdx = FMath::Clamp(ExitIdx, 0, Snaps.Num() - 1);

    auto PersonAt = [&](int i)->const FPersonPose*
        {
            const int32 p = PersonIdxOf.IsValidIndex(i) ? PersonIdxOf[i] : INDEX_NONE;
            return (p == INDEX_NONE || !Snaps[i].Poses.IsValidIndex(p)) ? nullptr : &Snaps[i].Poses[p];
        };

    const FPersonPose* PEnter = PersonAt(EnterIdx);
    const FPersonPose* PExit = PersonAt(ExitIdx);
    if (!PEnter || !PExit) return;

    // 손목/팔꿈치 2D (절대 픽셀 좌표를 그대로 사용)
    if (!PEnter->XY.IsValidIndex(IdxWr) || !UVisionMotionMathLibraries::IsFinite2D(PEnter->XY[IdxWr])) return;
    if (!PExit->XY.IsValidIndex(IdxWr) || !UVisionMotionMathLibraries::IsFinite2D(PExit->XY[IdxWr]))  return;
    if (!PEnter->XY.IsValidIndex(IdxEl) || !UVisionMotionMathLibraries::IsFinite2D(PEnter->XY[IdxEl])) return;
    if (!PExit->XY.IsValidIndex(IdxEl) || !UVisionMotionMathLibraries::IsFinite2D(PExit->XY[IdxEl]))  return;

    const FVector2f HandEnter2D = PEnter->XY[IdxWr];
    const FVector2f HandExit2D = PExit->XY[IdxWr];
    const FVector2f ElbowEnter2D = PEnter->XY[IdxEl];
    const FVector2f ElbowExit2D = PExit->XY[IdxEl];

    // 변환 파라미터 (컴포넌트에서 가져옴)
    const bool  bInvertY = SingleSwingClassifierComponent ? SingleSwingClassifierComponent->GetIsInvertImageYToUp() : false;
    const float PixelToUU = SingleSwingClassifierComponent ? SingleSwingClassifierComponent->GetPixelToUU() : 1.f;
    const float DepthOffsetX = SingleSwingClassifierComponent ? SingleSwingClassifierComponent->GetDepthOffsetX() : 0.f;

    const FVector HandEnterW = SingleSwingClassifierComponent->GetReceiver()->SampleToWorld(HandEnter2D);
    const FVector HandExitW = SingleSwingClassifierComponent->GetReceiver()->SampleToWorld(HandExit2D);
    const FVector ElbowEnterW = SingleSwingClassifierComponent->GetReceiver()->SampleToWorld(ElbowEnter2D);
    const FVector ElbowExitW = SingleSwingClassifierComponent->GetReceiver()->SampleToWorld(ElbowExit2D);

    // ★★★ 추가: Enter~Exit 구간 손목 경로(폴리라인) 길이 계산
    auto Wrist2DAt = [&](int i)->TOptional<FVector2f>
        {
            const FPersonPose* P = PersonAt(i);
            const int32 IdxWrLoc = InHandSide == EHandSide::Right ? MotionCapture::COCO_RWR : MotionCapture::COCO_LWR;
            if (!P || !P->XY.IsValidIndex(IdxWrLoc)) return {};
            const FVector2f& V = P->XY[IdxWrLoc];
            if (!UVisionMotionMathLibraries::IsFinite2D(V)) return {};
            return V;
        };

// ===== 스윙 경로 길이 계산(그대로) =====
    TArray<FVector> WristWorld; WristWorld.Reserve(ExitIdx - EnterIdx + 1);
    for (int i = EnterIdx; i <= ExitIdx; ++i)
    {
        if (const TOptional<FVector2f> Wr2D = Wrist2DAt(i))
        {
            WristWorld.Add(SingleSwingClassifierComponent->GetReceiver()->SampleToWorld(*Wr2D));
        }
    }
    double PathLenUU = 0.0;
    for (int i = 1; i < WristWorld.Num(); ++i)
        PathLenUU += (WristWorld[i] - WristWorld[i - 1]).Size();
    if (PathLenUU <= KINDA_SMALL_NUMBER)
        PathLenUU = (HandExitW - HandEnterW).Size();

    // ===== 진행 방향/평면 축 계산(그대로) =====
    FVector DirMove = (HandExitW - HandEnterW).GetSafeNormal();
    if (DirMove.IsNearlyZero())
    {
        FVector ForearmEnter = (HandEnterW - ElbowEnterW);
        DirMove = ForearmEnter.Normalize() ? ForearmEnter : CamFwd;
    }
    FVector N = FVector::CrossProduct(CamFwd, DirMove).GetSafeNormal();
    if (N.IsNearlyZero())
    {
        N = FVector::CrossProduct(FVector::UpVector, DirMove).GetSafeNormal();
        if (N.IsNearlyZero()) N = FVector::RightVector;
    }
    const FVector Zaxis = FVector::CrossProduct(N, DirMove).GetSafeNormal();
    const FQuat RotFixed = FQuat(FMatrix(
        FPlane(N.X, N.Y, N.Z, 0.f),
        FPlane(DirMove.X, DirMove.Y, DirMove.Z, 0.f),
        FPlane(Zaxis.X, Zaxis.Y, Zaxis.Z, 0.f),
        FPlane(0, 0, 0, 1)));

    // ===== 스폰 오프셋(그대로) =====
    const FVector ForearmEnterVec = (HandEnterW - ElbowEnterW);
    const float   ForearmLenEn = ForearmEnterVec.Size();
    const float   ForearmExtendRatio = 0.25f;
    const FVector SpawnOffsetEnter = (ForearmLenEn > KINDA_SMALL_NUMBER)
        ? (ForearmEnterVec.GetSafeNormal() * (ForearmLenEn * ForearmExtendRatio))
        : FVector::ZeroVector;

    // ====== "하나의 평면"만 만들기 ======
    // HalfLength 를 스윙 길이의 절반으로 설정
    // (필요하면 클램프 범위를 프로젝트 성향에 맞게 조정)
    const float MinHalfLen = 20.f;          // 너무 짧아지지 않도록
    const float MaxHalfLen = 5000.f;        // 과도한 확장 방지
    const float HalfLen = FMath::Clamp(static_cast<float>(PathLenUU * 0.5), MinHalfLen, MaxHalfLen);

    // 평면의 월드 중심: 스윙 중점 + 스폰 오프셋
    const FVector Mid = FMath::Lerp(HandEnterW, HandExitW, 0.5f);
    FVector Center = Mid + SpawnOffsetEnter;
    Center.X -= 100.0f;

    float PlaneLength = HalfLen;
    float PlaneWidth = 1500.0f;

    if (SingleSwingEffectClass)
    {
        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        Params.Owner = this;
        Params.Instigator = GetInstigator();
        ASingleSwingEffect* SingleSwingEffect = World->SpawnActor<ASingleSwingEffect>(SingleSwingEffectClass, Center, RotFixed.Rotator(), Params);
        if (SingleSwingEffect)
        {
            SingleSwingEffect->InitializeSize(Center, DirMove.Rotation(), PlaneLength, PlaneWidth);

            if (AttackLifeSeconds > 0.f)
            {
                SingleSwingEffect->SetLifeSpan(AttackLifeSeconds);
            }
        }
    }

    if (bDoDebug)
    {
        DrawDebugDirectionalArrow(World, Mid, Mid + DirMove * 120.f, 20.f, FColor::Green, false, 2.0f, 0, 2.f);
        DrawDebugDirectionalArrow(World, Mid, Mid + N * 120.f, 20.f, FColor::Magenta, false, 2.0f, 0, 2.f);
        DrawDebugSphere(World, Center, 6.f, 12, FColor::Yellow, false, 2.0f);
    }
}