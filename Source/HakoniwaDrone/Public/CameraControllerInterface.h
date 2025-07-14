// CameraControllerInterface.h

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PduManager.h"

#include "CameraControllerInterface.generated.h"

UINTERFACE(Blueprintable)
class HAKONIWADRONE_API UCameraControllerInterface : public UInterface
{
    GENERATED_BODY()
};

class HAKONIWADRONE_API ICameraControllerInterface
{
    GENERATED_BODY()

public:
    virtual void Initialize() = 0;
    virtual void UpdateCameraAngle() = 0;
    virtual void DeclarePdu(const FString& RobotName, UPduManager* PduManager) = 0;
    virtual void RotateCamera(float Step) = 0;
    virtual void WriteCameraInfo(int32 MoveCurrentId, UPduManager* PduManager) = 0;
    virtual void WriteCameraDataPdu(UPduManager* PduManager) = 0;
    virtual void Scan() = 0;
    virtual void SetCameraAngle(float Angle) = 0;
    virtual void UpdateCameraImageTexture() = 0;
    virtual void CameraImageRequest(UPduManager* PduManager) = 0;
    virtual void CameraMoveRequest(UPduManager* PduManager) = 0;
};
