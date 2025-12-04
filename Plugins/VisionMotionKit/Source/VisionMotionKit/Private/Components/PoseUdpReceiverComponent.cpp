// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/PoseUdpReceiverComponent.h"
#include "HAL/RunnableThread.h"
#include "Common/UdpSocketBuilder.h"
#include "DrawDebugHelpers.h"
#include "Utilities/VisionMotionUtils.h"
#include "Utilities/VisionMotionMathLibraries.h"



UPoseUdpReceiverComponent::UPoseUdpReceiverComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UPoseUdpReceiverComponent::BeginPlay()
{
    Super::BeginPlay();
    RecvBuffer.SetNumUninitialized(2048);
    InitSocket(7777);
}

void UPoseUdpReceiverComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    if (!GetWorld() || !GetOwner()) return;

    // === 입력 패킷 수집(최근 상태 보존) ======================
    TArray<FPersonPose> Poses;
    TArray<FHandPose>   Hands;
    uint64 Ts = 0;

    while (ReceiveOnce(Poses, Hands, Ts))
    {
        LatestPoses = Poses;
        LatestHands = Hands;
        LatestTimestamp = Ts;
    }

    // 더 이상 입력이 없으면 종료
    if (LatestPoses.Num() == 0 && LatestHands.Num() == 0) return;

    const FTransform& OwnerXform = GetOwner()->GetActorTransform();

    if (bDrawDebug)
    {
        // === 바디 디버그 드로잉 =================================
        for (const FPersonPose& Pose : LatestPoses)
        {
            if (Pose.XY.Num() < 17) continue;

            // 라인 스켈레톤
            for (int32 e = 0; e < MotionCapture::NUM_EDGES; ++e)
            {
                const int32 A = MotionCapture::EDGES[e][0];
                const int32 B = MotionCapture::EDGES[e][1];
                if (!Pose.XY.IsValidIndex(A) || !Pose.XY.IsValidIndex(B)) continue;

                const FVector2f& Pa2D = Pose.XY[A];
                const FVector2f& Pb2D = Pose.XY[B];
                if (!(UVisionMotionMathLibraries::IsFinite2D(Pa2D) && UVisionMotionMathLibraries::IsFinite2D(Pb2D))) continue;

                const FVector Wa = SampleToWorld(Pa2D);
                const FVector Wb = SampleToWorld(Pb2D);

                DrawDebugLine(GetWorld(), Wa, Wb, LineColor, false, 0.f, 0, LineThickness);
            }

            // 포인트
            for (int32 k = 0; k < 17; ++k)
            {
                const FVector2f& P2D = Pose.XY[k];
                if (!UVisionMotionMathLibraries::IsFinite2D(P2D)) continue;

                const FVector Wp = SampleToWorld(P2D);
                DrawDebugSphere(GetWorld(), Wp, PointRadius, 8, PointColor, false, 0.f, 0, 0.5f);
            }
        }

        // === 손 디버그 드로잉 ===================================
        for (const FHandPose& H : LatestHands)
        {

            for (int32 k = 0; k < H.XY.Num(); ++k)
            {
                const FVector2f& P2D = H.XY[k];
                if (!UVisionMotionMathLibraries::IsFinite2D(P2D)) continue;

                const FVector Wp = SampleToWorld(P2D);

                const FColor C =
                    (H.Which == 0) ? FColor(0, 200, 255) :
                    (H.Which == 1) ? FColor(255, 200, 0) :
                    FColor(200, 200, 200);

                DrawDebugSphere(GetWorld(), Wp, PointRadius, 8, C, false, 0.f, 0, 0.5f);
            }

            if (UVisionMotionMathLibraries::IsFinite2D(H.Center))
            {
                const FVector Wc = SampleToWorld(H.Center);
                DrawDebugSphere(GetWorld(), Wc, PointRadius * 1.5f, 10, FColor::Cyan, false, 0.f, 0, 0.8f);
            }
        }
    }
    

    // === 브로드캐스트(리스너가 동일 원점으로 해석하도록 전달) =========
    const FTransform& OwnerXformFinal = OwnerXform;
    const FVector2f BroadcastOrigin2D = FrameOrigin2D;

    if (LatestPoses.Num() > 0)
    {
        OnPoseBufferChanged.Broadcast(BroadcastOrigin2D, LatestPoses, PixelToUU, OwnerXformFinal);
    }

    if (LatestHands.Num() > 0)
    {
        OnHandBufferChanged.Broadcast(BroadcastOrigin2D, LatestHands, PixelToUU, OwnerXformFinal);
    }
}

void UPoseUdpReceiverComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    CloseSocket();
    Super::EndPlay(EndPlayReason);
}

bool UPoseUdpReceiverComponent::InitSocket(int32 Port)
{
    CloseSocket();

    Socket = FUdpSocketBuilder(TEXT("PoseRecv"))
        .AsNonBlocking()
        .AsReusable()
        .BoundToEndpoint(FIPv4Endpoint(FIPv4Address::Any, Port))
        .WithReceiveBufferSize(1 << 20);

    return Socket != nullptr;
}

void UPoseUdpReceiverComponent::CloseSocket()
{
    if (Socket)
    {
        Socket->Close();
        ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
        Socket = nullptr;
    }
}

bool UPoseUdpReceiverComponent::ReceiveOnce(TArray<FPersonPose>& OutPoses, TArray<FHandPose>& OutHands, uint64& OutTsMs)
{
    OutPoses.Reset();
    OutHands.Reset();
    OutTsMs = 0;

    if (!Socket) return false;

    uint32 BytesPending = 0;
    if (!Socket->HasPendingData(BytesPending) || BytesPending < MotionCapture::HEADER_SIZE) return false;

    RecvBuffer.SetNumUninitialized(BytesPending);
    int32 BytesRead = 0;
    if (!Socket->Recv(RecvBuffer.GetData(), RecvBuffer.Num(), BytesRead, ESocketReceiveFlags::None)) return false;
    if (BytesRead < MotionCapture::HEADER_SIZE) return false;

    const uint8* p = RecvBuffer.GetData();
    const uint8* end = RecvBuffer.GetData() + BytesRead;

    auto readU32 = [&p]() { uint32 v = 0; FMemory::Memcpy(&v, p, 4); p += 4; return v; };
    auto readU16 = [&p]() { uint16 v = 0; FMemory::Memcpy(&v, p, 2); p += 2; return v; };
    auto readU8 = [&p]() {  uint8 v = *p; p += 1; return v; };
    auto readU64 = [&p]() { uint64 v = 0; FMemory::Memcpy(&v, p, 8); p += 8; return v; };
    auto readF32 = [&p]() {  float v = 0; FMemory::Memcpy(&v, p, 4); p += 4; return v; };

    // ---- Header (match "<4sBBHHQ") ----
    uint32 magic = readU32();
    if (magic != *((uint32*)"POSE")) return false;
    uint8  ver = readU8();
    (void)readU8();              // flags (현재 사용 안 함)
    uint16 persons = readU16();
    uint16 hands = readU16();  // v2부터 존재
    OutTsMs = readU64();

    // ---- Persons ----
    OutPoses.Reserve(persons);
    for (uint16 i = 0; i < persons; ++i)
    {
        if (p + MotionCapture::EXPECTED_PERSON_BLOCK > end) return (OutPoses.Num() + OutHands.Num()) > 0;

        FPersonPose pose;
        pose.PersonId = readU16();

        pose.XY.SetNumUninitialized(17);
        for (int k = 0; k < 17; ++k)
        {
            const float x = readF32();
            const float y = readF32();
            pose.XY[k] = FVector2f(x, y);
        }
        pose.Conf.SetNumUninitialized(17);
        for (int k = 0; k < 17; ++k)
        {
            pose.Conf[k] = readF32();
        }
        OutPoses.Add(MoveTemp(pose));
    }

    // ---- Hands (v2 이상) ----
    if (ver >= 2)
    {
        OutHands.Reserve(hands);
        for (uint16 i = 0; i < hands; ++i)
        {
            if (p + MotionCapture::EXPECTED_HAND_BLOCK > end) break;

            FHandPose H;
            // 송신은 hand_id(H)만 오고, personId는 안 옴 → Unknown
            const uint16 hand_id = readU16(); (void)hand_id;
            const uint8  handed = readU8();  // 0=Right,1=Left,2=Unknown (송신)
            const float  score = readF32();

            // 좌표 21개
            H.XY.SetNumUninitialized(21);
            for (int k = 0; k < 21; ++k)
            {
                const float x = readF32();
                const float y = readF32();
                H.XY[k] = FVector2f(x, y);
            }

            // 각 키포인트 conf (21개)
            H.Conf.SetNumUninitialized(21);
            for (int k = 0; k < 21; ++k)
            {
                H.Conf[k] = readF32();
            }

            // 수신 구조 채우기
            H.PersonId = 0xFFFF; // 매칭 정보 없음
            // 주의: 내부 표기(0=Left,1=Right)라면 여기서 스왑 필요
            H.Which = (handed == 0) ? 1 : (handed == 1) ? 0 : 2;

            // Center는 평균으로 계산(옵션)
            FVector2f sum(0, 0);
            int n = 0;
            for (const auto& p2 : H.XY) { if (UVisionMotionMathLibraries::IsFinite2D(p2)) { sum += p2; ++n; } }
            H.Center = (n > 0) ? (sum / float(n)) : FVector2f(0, 0);

            OutHands.Add(MoveTemp(H));
        }
    }

    return (OutPoses.Num() + OutHands.Num()) > 0;
}

bool UPoseUdpReceiverComponent::GetShoulderMidWorld(FVector& OutMidWorld) const
{
    OutMidWorld = FVector::ZeroVector;

    const TArray<FPersonPose>& Poses = GetLatestPoses();
    if (Poses.Num() == 0) return false;

    const int32 LSH = 5, RSH = 6, LHIP = 11, RHIP = 12;

    int32 BestIdx = INDEX_NONE;
    float BestScore = -FLT_MAX;

    // 신뢰도 높은 사람 고르기
    for (int32 i = 0; i < Poses.Num(); ++i)
    {
        const FPersonPose& P = Poses[i];
        if (!(P.XY.IsValidIndex(LSH) && P.XY.IsValidIndex(RSH) &&
            P.XY.IsValidIndex(LHIP) && P.XY.IsValidIndex(RHIP))) continue;

        const FVector2f& LSh = P.XY[LSH];
        const FVector2f& RSh = P.XY[RSH];
        const FVector2f& LHp = P.XY[LHIP];
        const FVector2f& RHp = P.XY[RHIP];
        if (!(UVisionMotionMathLibraries::IsFinite2D(LSh) && UVisionMotionMathLibraries::IsFinite2D(RSh) &&
            UVisionMotionMathLibraries::IsFinite2D(LHp) && UVisionMotionMathLibraries::IsFinite2D(RHp))) continue;

        const float cL = (P.Conf.IsValidIndex(LSH) ? P.Conf[LSH] : 1.f);
        const float cR = (P.Conf.IsValidIndex(RSH) ? P.Conf[RSH] : 1.f);
        const float Score = cL + cR;

        if (Score > BestScore) { BestScore = Score; BestIdx = i; }
    }
    if (BestIdx == INDEX_NONE) return false;

    const FPersonPose& Best = Poses[BestIdx];
    const FVector2f& LSh = Best.XY[LSH];
    const FVector2f& RSh = Best.XY[RSH];

    FVector2f Anchor2D = FVector2f::ZeroVector;

    const FVector LShW = SampleToWorld(LSh);
    const FVector RShW = SampleToWorld(RSh);

    OutMidWorld = 0.5f * (LShW + RShW);
    return true;
}

FVector UPoseUdpReceiverComponent::SampleToWorld(const FVector2f& P2D) const
{
    const FVector2f Origin2D = FrameOrigin2D;
    const FVector   L = MakeLocalFrom2D(P2D, Origin2D);

    if (AActor* Owner = GetOwner())
    {
        return Owner->GetActorTransform().TransformPosition(L);
    }

    return L; // Owner 없으면 로컬 좌표 그대로 반환
}

FVector UPoseUdpReceiverComponent::MakeLocalFrom2D(const FVector2f& P, const FVector2f& Origin2D) const
{
    const FVector2f Rel = P - Origin2D;
    const float Y = Rel.X * PixelToUU;
    const float Z = -Rel.Y * PixelToUU;
    return FVector(DepthOffsetX, Y, Z);
}