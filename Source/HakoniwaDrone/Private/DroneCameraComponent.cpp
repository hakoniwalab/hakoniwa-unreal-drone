// Fill out your copyright notice in the Description page of Project Settings.


#include "DroneCameraComponent.h"
#include "hako_msgs/pdu_cpptype_conv_HakoCameraInfo.hpp" 
#include "hako_msgs/pdu_cpptype_conv_HakoCameraData.hpp" 
#include "hako_msgs/pdu_cpptype_conv_HakoCmdCameraMove.hpp" 
#include "hako_msgs/pdu_cpptype_conv_HakoCmdCamera.hpp" 
#include "pdu_convertor.hpp"
#include "IImageWrapperModule.h"
#include "IImageWrapper.h"
#include "HakoniwaWebClient.h"

#include <Kismet/GameplayStatics.h>


// Sets default values for this component's properties
UDroneCameraComponent::UDroneCameraComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

    // 自分自身の子コンポーネントとして SceneCaptureComponent2D を生成
    SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture"));

    // このコンポーネントのルートにアタッチ（親子関係を設定）
    SceneCapture->SetupAttachment(this);
}


// Called when the game starts
void UDroneCameraComponent::BeginPlay()
{
	Super::BeginPlay();

    Initialize();
}


// Called every frame
void UDroneCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (PduManager_ == nullptr) {
        if (GetOwner())
        {
            AHakoniwaWebClient* WebClient = Cast<AHakoniwaWebClient>(UGameplayStatics::GetActorOfClass(GetOwner()->GetWorld(), AHakoniwaWebClient::StaticClass()));
            if (WebClient)
            {
                PduManager_ = WebClient->GetPduManager();
            }
        }
    }
    if (PduManager_ != nullptr)
    {
        if (IsDeclared) {
            //UE_LOG(LogTemp, Log, TEXT("Tick DroneCameraActor start pdu actions"));
            CameraMoveRequest(PduManager_);
            CameraImageRequest(PduManager_);
        }
        else if (PduManager_->IsServiceEnabled()) {
            UE_LOG(LogTemp, Log, TEXT("Tick DroneCameraActor start Declare"));
            DeclarePdu(RobotName, PduManager_);
            IsDeclared = true;
        }
    }
    else {
        UE_LOG(LogTemp, Error, TEXT("DroneCameraActor can not find pduManager."));
    }

}


void UDroneCameraComponent::Initialize()
{
    // RenderTarget を生成
    RenderTarget = NewObject<UTextureRenderTarget2D>(this);
    RenderTarget->InitAutoFormat(640, 480);
    RenderTarget->ClearColor = FLinearColor::Black;
    RenderTarget->UpdateResourceImmediate(true);

    // SceneCapture にアタッチ
    SceneCapture->TextureTarget = RenderTarget;
    SceneCapture->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
    SceneCapture->bCaptureEveryFrame = false;
    SceneCapture->bCaptureOnMovement = false;

    // 初期値
    ManualRotationDeg = 0.0f;
    MoveStep = 1.0f;
    CameraMoveUpDeg = -15.0f;
    CameraMoveDownDeg = 90.0f;
    // 使用するPDU名の定義（固定）
    PduCmdCamera = TEXT("hako_cmd_camera");
    PduCmdCameraMove = TEXT("hako_cmd_camera_move");
    PduCameraData = TEXT("hako_camera_data");
    PduCameraInfo = TEXT("hako_cmd_camera_info");

    UE_LOG(LogTemp, Log, TEXT("DroneCameraActor::Initialize completed"));
}


void UDroneCameraComponent::UpdateCameraAngle()
{
    if (!SceneCapture) return;

    // Pitch だけをマニュアル制御、他の軸は固定
    FRotator NewRotation = SceneCapture->GetRelativeRotation();
    NewRotation.Pitch = ManualRotationDeg;
    SceneCapture->SetRelativeRotation(NewRotation);
}
bool UDroneCameraComponent::DeclarePdu(const FString& InRobotName, UPduManager* PduManager)
{
    if (!PduManager)
    {
        UE_LOG(LogTemp, Error, TEXT("DeclarePdu: PduManager is null"));
        return false;
    }

    RobotName = InRobotName;
    bool bAllSucceeded = true;

    // Read PDU 1
    if (!PduManager->DeclarePduForRead(RobotName, PduCmdCamera))
    {
        UE_LOG(LogTemp, Error, TEXT("DeclarePdu: FAILED to declare %s"), *PduCmdCamera);
        bAllSucceeded = false;
    }

    // Read PDU 2
    if (!PduManager->DeclarePduForRead(RobotName, PduCmdCameraMove))
    {
        UE_LOG(LogTemp, Error, TEXT("DeclarePdu: FAILED to declare %s"), *PduCmdCameraMove);
        bAllSucceeded = false;
    }

    // Write PDU 1
    if (!PduManager->DeclarePduForWrite(RobotName, PduCameraData))
    {
        UE_LOG(LogTemp, Error, TEXT("DeclarePdu: FAILED to declare %s"), *PduCameraData);
        bAllSucceeded = false;
    }

    // Write PDU 2
    if (!PduManager->DeclarePduForWrite(RobotName, PduCameraInfo))
    {
        UE_LOG(LogTemp, Error, TEXT("DeclarePdu: FAILED to declare %s"), *PduCameraInfo);
        bAllSucceeded = false;
    }

    if (bAllSucceeded)
    {
        UE_LOG(LogTemp, Log, TEXT("DeclarePdu: All declarations succeeded for %s"), *RobotName);
    }

    return bAllSucceeded;
}

void UDroneCameraComponent::RotateCamera(float Step)
{
    SetCameraAngle(ManualRotationDeg + Step);
}

void UDroneCameraComponent::WriteCameraInfo(int32 InMoveCurrentId, UPduManager* PduManager)
{
    if (!PduManager || PduCameraInfo.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("WriteCameraInfo: PduManager is null or Pdu name is not set"));
        return;
    }

    // PDUサイズ取得
    int32 PduSize = PduManager->GetPduSize(RobotName, PduCameraInfo);
    if (PduSize <= 0)
    {
        UE_LOG(LogTemp, Error, TEXT("WriteCameraInfo: Failed to get PDU size for %s:%s"), *RobotName, *PduCameraInfo);
        return;
    }

    TArray<uint8> Buffer;
    Buffer.SetNum(PduSize);

    // HakoCameraInfo 変換構造体を使ってデータ構築
    HakoCpp_HakoCameraInfo CameraInfo;
    CameraInfo.request_id = InMoveCurrentId;
    CameraInfo.angle.x = 0.0f;
    CameraInfo.angle.y = ManualRotationDeg;
    CameraInfo.angle.z = 0.0f;

    hako::pdu::PduConvertor<HakoCpp_HakoCameraInfo, hako::pdu::msgs::hako_msgs::HakoCameraInfo> Conv;
    Conv.cpp2pdu(CameraInfo, (char*)Buffer.GetData(), Buffer.Num());

    // 書き込み
    if (!PduManager->FlushPduRawData(RobotName, PduCameraInfo, Buffer))
    {
        UE_LOG(LogTemp, Error, TEXT("WriteCameraInfo: Failed to flush PDU data"));
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("WriteCameraInfo: Sent camera info. yaw=%.2f"), ManualRotationDeg);
    }
}
void UDroneCameraComponent::WriteCameraDataPdu(UPduManager* PduManager)
{
    if (!PduManager || PduCameraData.IsEmpty()) {
        UE_LOG(LogTemp, Error, TEXT("WriteCameraDataPdu: Invalid state"));
        return;
    }

    int32 PduSize = PduManager->GetPduSize(RobotName, PduCameraData);
    if (PduSize <= 0) {
        UE_LOG(LogTemp, Error, TEXT("WriteCameraDataPdu: Failed to get PDU size"));
        return;
    }

    TArray<uint8> Buffer;
    Buffer.SetNum(PduSize);

    // 構造体構築
    HakoCpp_HakoCameraData CameraData;
    CameraData.request_id = CurrentId;

    // タイムスタンプを設定
    int64 NowUsec = FDateTime::UtcNow().ToUnixTimestamp() * 1000000 + (FPlatformTime::Cycles64() % 1000000);
    CameraData.image.header.stamp.sec = static_cast<int32>(NowUsec / 1000000);
    CameraData.image.header.stamp.nanosec = static_cast<uint32>(NowUsec % 1000000) * 1000;

    CameraData.image.header.frame_id = TCHAR_TO_ANSI(*RobotName);  // UnrealのFString→char*
    CameraData.image.format = (EncodeType == 0) ? "png" : "jpeg";

    // バッファコピー
    CameraData.image.data.resize(CompressedImageBytes.Num());
    FMemory::Memcpy(CameraData.image.data.data(), CompressedImageBytes.GetData(), CompressedImageBytes.Num());

    hako::pdu::PduConvertor<HakoCpp_HakoCameraData, hako::pdu::msgs::hako_msgs::HakoCameraData> Conv;
    Conv.cpp2pdu(CameraData, (char*)Buffer.GetData(), Buffer.Num());

    if (!PduManager->FlushPduRawData(RobotName, PduCameraData, Buffer)) {
        UE_LOG(LogTemp, Error, TEXT("WriteCameraDataPdu: Failed to flush PDU"));
    }
    else {
        UE_LOG(LogTemp, Log, TEXT("WriteCameraDataPdu: PDU sent successfully. Size = %d"), CompressedImageBytes.Num());
    }
}



void UDroneCameraComponent::Scan()
{
    if (!RenderTarget || !SceneCapture)
    {
        UE_LOG(LogTemp, Error, TEXT("Scan: RenderTarget or SceneCapture not initialized"));
        return;
    }

    SceneCapture->CaptureScene();

    FTextureRenderTargetResource* RenderResource = RenderTarget->GameThread_GetRenderTargetResource();
    TArray<FColor> PixelData;
    FReadSurfaceDataFlags ReadDataFlags;
    ReadDataFlags.SetLinearToGamma(false);

    if (!RenderResource->ReadPixels(PixelData, ReadDataFlags) || PixelData.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Scan: Failed to read pixels from RenderTarget"));
        return;
    }

    int32 Width = RenderTarget->SizeX;
    int32 Height = RenderTarget->SizeY;

    // 上下反転 & RGB 抽出
    RawImageBytes.Reset();
    RawImageBytes.AddUninitialized(Width * Height * 4);


    for (int32 y = 0; y < Height; y++)
    {
        for (int32 x = 0; x < Width; x++)
        {
            int32 SrcIndex = (Height - 1 - y) * Width + x;
            int32 DstIndex = (y * Width + x) * 4;
            const FColor& Color = PixelData[SrcIndex];
            RawImageBytes[DstIndex + 0] = Color.R;
            RawImageBytes[DstIndex + 1] = Color.G;
            RawImageBytes[DstIndex + 2] = Color.B;
            RawImageBytes[DstIndex + 3] = 255;
        }
    }

    // ImageWrapper を使ってエンコード
    IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
    EImageFormat ImageFormat = (EncodeType == 0) ? EImageFormat::PNG : EImageFormat::JPEG;

    TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(ImageFormat);

    if (ImageWrapper.IsValid() &&
        ImageWrapper->SetRaw(RawImageBytes.GetData(), RawImageBytes.Num(), Width, Height, ERGBFormat::RGBA, 8))
    {
        CompressedImageBytes = ImageWrapper->GetCompressed();
        UE_LOG(LogTemp, Log, TEXT("Scan: Image encoded. Size = %d bytes"), CompressedImageBytes.Num());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Scan: Failed to encode image"));
    }
}


void UDroneCameraComponent::SetCameraAngle(float Angle)
{
    float NewPitch = Angle;

    if (NewPitch > 180.0f) NewPitch -= 360.0f;
    ManualRotationDeg = FMath::Clamp(NewPitch, CameraMoveUpDeg, CameraMoveDownDeg);
}

void UDroneCameraComponent::UpdateCameraImageTexture()
{
    //TODO
}

void UDroneCameraComponent::CameraImageRequest(UPduManager* PduManager)
{
    if (!PduManager || PduCmdCamera.IsEmpty()) {
        UE_LOG(LogTemp, Error, TEXT("CameraImageRequest: Invalid state"));
        return;
    }

    int32 PduSize = PduManager->GetPduSize(RobotName, PduCmdCamera);
    if (PduSize <= 0) {
        //UE_LOG(LogTemp, Error, TEXT("CameraImageRequest: Failed to get PDU size"));
        //data is not arrived
        return;
    }

    TArray<uint8> Buffer;
    Buffer.SetNum(PduSize);
    if (!PduManager->ReadPduRawData(RobotName, PduCmdCamera, Buffer)) {
        //UE_LOG(LogTemp, Error, TEXT("CameraImageRequest: Failed to read PDU"));
        //data is not arrived.
        return;
    }

    HakoCpp_HakoCmdCamera Cmd;
    hako::pdu::PduConvertor<HakoCpp_HakoCmdCamera, hako::pdu::msgs::hako_msgs::HakoCmdCamera> Conv;
    Conv.pdu2cpp((char*)Buffer.GetData(), Cmd);

    if (Cmd.header.request) {
        RequestId = Cmd.request_id;
        if (CurrentId != RequestId) {
            CurrentId = RequestId;
            UE_LOG(LogTemp, Log, TEXT("CameraImageRequest: New request received. id=%d"), RequestId);
            Scan();  // カメラ画像送信処理を呼ぶ
            WriteCameraDataPdu(PduManager);
        }
    }
}

void UDroneCameraComponent::CameraMoveRequest(UPduManager* PduManager)
{
    if (!PduManager || PduCmdCameraMove.IsEmpty()) {
        UE_LOG(LogTemp, Error, TEXT("CameraMoveRequest: Invalid state"));
        return;
    }

    int32 PduSize = PduManager->GetPduSize(RobotName, PduCmdCameraMove);
    if (PduSize <= 0) {
        UE_LOG(LogTemp, Error, TEXT("CameraMoveRequest: Failed to get PDU size"));
        return;
    }

    TArray<uint8> Buffer;
    Buffer.SetNum(PduSize);
    if (!PduManager->ReadPduRawData(RobotName, PduCmdCameraMove, Buffer)) {
        //UE_LOG(LogTemp, Error, TEXT("CameraMoveRequest: Failed to read PDU"));
        //data is not arrived
        return;
    }

    HakoCpp_HakoCmdCameraMove CmdMove;
    hako::pdu::PduConvertor<HakoCpp_HakoCmdCameraMove, hako::pdu::msgs::hako_msgs::HakoCmdCameraMove> Conv;
    Conv.pdu2cpp((char*)Buffer.GetData(), CmdMove);

    if (CmdMove.header.request) {
        MoveRequestId = CmdMove.request_id;
        if (MoveCurrentIdInternal != MoveRequestId) {
            MoveCurrentIdInternal = MoveRequestId;
            float TargetAngle = -CmdMove.angle.y;  // Unityと同様にY軸反転
            SetCameraAngle(TargetAngle);
            UpdateCameraAngle();
            WriteCameraInfo(MoveCurrentIdInternal, PduManager);
            UE_LOG(LogTemp, Log, TEXT("CameraMoveRequest: angle.y = %.2f (target = %.2f)"), CmdMove.angle.y, TargetAngle);
        }
    }
}
