// DroneCameraActor.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CameraControllerInterface.h"
#include "PduManager.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/SceneCaptureComponent2D.h"
#include "DroneCameraActor.generated.h"

UCLASS()
class HAKONIWADRONE_API ADroneCameraActor : public AActor, public ICameraControllerInterface
{
    GENERATED_BODY()

public:
    ADroneCameraActor();

    virtual void Tick(float DeltaTime) override;
    virtual void BeginPlay() override;

    // ICameraControllerInterface 実装
    virtual void Initialize() override;
    virtual void UpdateCameraAngle() override;
    virtual void DeclarePdu(const FString& RobotName, UPduManager* PduManager) override;
    virtual void RotateCamera(float Step) override;
    virtual void WriteCameraInfo(int32 MoveCurrentId, UPduManager* PduManager) override;
    virtual void WriteCameraDataPdu(UPduManager* PduManager) override;
    virtual void Scan() override;
    virtual void SetCameraAngle(float Angle) override;
    virtual void UpdateCameraImageTexture() override;
    virtual void CameraImageRequest(UPduManager* PduManager) override;
    virtual void CameraMoveRequest(UPduManager* PduManager) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hakoniwa", meta = (AllowPrivateAccess = "true"))
    USceneCaptureComponent2D* SceneCapture;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hakoniwa")
    FString RobotName = "Drone";
protected:
    UPROPERTY()
    UTextureRenderTarget2D* RenderTarget;

private:
    UPduManager* PduManager_;
    float ManualRotationDeg;
    int32 CurrentId;
    int32 RequestId;

    int32 MoveCurrentIdInternal;
    int32 MoveRequestId;
    float MoveStep;
    float CameraMoveUpDeg;
    float CameraMoveDownDeg;

    // PDU names
    FString PduCmdCamera;
    FString PduCmdCameraMove;
    FString PduCameraData;
    FString PduCameraInfo;
    bool IsDeclared = false;

    // バッファ用
    TArray<uint8> RawImageBytes;
    TArray<uint8> CompressedImageBytes;

    // エンコード選択（0 = PNG, 1 = JPEG）
    int32 EncodeType = 0;

};
