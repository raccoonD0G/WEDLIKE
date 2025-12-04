// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/HandFollower.h"
#include "Actors/MirroredPlayer.h"
#include "Global/VisionMotionUdpStruct.h"
#include "Components/HandPoseClassifierComponent.h"
#include "Utilities/VisionMotionMathLibraries.h"
#include "Utilities/VisionMotionUtils.h"

AHandFollower::AHandFollower()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;
}

void AHandFollower::SetOwningPlayer(AMirroredPlayer* InMirroredPlayer)
{
    OwningPlayer = InMirroredPlayer;
}

void AHandFollower::BeginPlay()
{
	Super::BeginPlay();

	check(OwningPlayer);

    if (UPoseUdpReceiverComponent* Recv = OwningPlayer->GetPoseReceiver())
    {
        Recv->OnPoseBufferChanged.AddDynamic(this, &AHandFollower::OnPoseInput);
    }
}

void AHandFollower::SetHandPoseClassifierComponent(UHandPoseClassifierComponent* InHandPoseClassifierComponent)
{
	HandPoseClassifierComponent = InHandPoseClassifierComponent;
}

void AHandFollower::OnPoseInput(const FVector2f& Anchor2D, const TArray<FPersonPose>& Poses, float PixelToUU, const FTransform& OwnerXform)
{
    FVector Center;
    bool bOk = false;

    if (!HandPoseClassifierComponent) return;

    switch (HandPoseClassifierComponent->GetCurrentHandSide())
    {
    case EHandSide::Right:
        bOk = TryComputeExtendedPoint(Anchor2D, Poses.Last(), OwnerXform, true, Center, nullptr);
        break;

    case EHandSide::Left:
        bOk = TryComputeExtendedPoint(Anchor2D, Poses.Last(), OwnerXform, false, Center, nullptr);
        break;

    default:
        break;
    }

    if (!bOk) return;

    SetHandFollowerPose(Center);
}

void AHandFollower::SetHandFollowerPose(const FVector& CenterWorld)
{
    FVector XChanged = CenterWorld;
    XChanged.X = HandPoseClassifierComponent->GetOwner()->GetActorLocation().X - 100.0f;
    SetActorLocation(XChanged);
}


bool AHandFollower::TryComputeExtendedPoint(const FVector2f& Anchor2D, const FPersonPose& Pose, const FTransform& OwnerXform, bool bRightHand, FVector& OutPoint, float* OutScore) const
{
    if (!HandPoseClassifierComponent) return false;

    const int32 IdxElbow = bRightHand ? MotionCapture::COCO_REL : MotionCapture::COCO_LEL;
    const int32 IdxWrist = bRightHand ? MotionCapture::COCO_RWR : MotionCapture::COCO_LWR;
    if (!Pose.XY.IsValidIndex(IdxElbow) || !Pose.XY.IsValidIndex(IdxWrist)) return false;

    const FVector2f El2D = Pose.XY[IdxElbow];
    const FVector2f Wr2D = Pose.XY[IdxWrist];
    if (!(UVisionMotionMathLibraries::IsFinite2D(El2D) && UVisionMotionMathLibraries::IsFinite2D(Wr2D))) return false;

    // ★ 절대 2D → 월드 (앵커 미사용)
    const bool bUseAbsoluteCanvas = true;
    const FVector ElW = HandPoseClassifierComponent->GetReceiver()->SampleToWorld(El2D);
    const FVector WrW = HandPoseClassifierComponent->GetReceiver()->SampleToWorld(Wr2D);

    const FVector Forearm = (WrW - ElW);
    OutPoint = WrW + Forearm * ExtendRatio;

    if (OutScore)
    {
        float cEl = Pose.Conf.IsValidIndex(IdxElbow) ? Pose.Conf[IdxElbow] : 1.f;
        float cWr = Pose.Conf.IsValidIndex(IdxWrist) ? Pose.Conf[IdxWrist] : 1.f;
        *OutScore = cEl + cWr + 0.001f * Forearm.Size();
    }
    return true;
}
