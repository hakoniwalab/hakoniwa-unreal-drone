// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"

#include "CameraControllerInterface.h"
#include "PduManager.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/SceneCaptureComponent2D.h"


#include "DroneCameraComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HAKONIWADRONE_API UDroneCameraComponent : public USceneComponent, public ICameraControllerInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDroneCameraComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


    // ICameraControllerInterface ����
    virtual void Initialize() override;
    virtual void UpdateCameraAngle() override;
    virtual bool DeclarePdu(const FString& RobotName, UPduManager* PduManager) override;
    virtual void RotateCamera(float Step) override;
    virtual void WriteCameraInfo(int32 MoveCurrentId, UPduManager* PduManager) override;
    virtual void WriteCameraDataPdu(UPduManager* PduManager) override;
    virtual void Scan() override;
    virtual void SetCameraAngle(float Angle) override;
    virtual void UpdateCameraImageTexture() override;
    virtual void CameraImageRequest(UPduManager* PduManager) override;
    virtual void CameraMoveRequest(UPduManager* PduManager) override;

private:
    // UPROPERTY��ǉ����A�G�f�B�^�Őݒ�\�ɂ���
    UPROPERTY(EditAnywhere, Category = "Hakoniwa")
    FString RobotName = "Drone"; // �f�t�H���g�l���ݒ�\

    // UPROPERTY��ǉ����AGC����ی삵�A�G�f�B�^�Ŋm�F�ł���悤�ɂ���
    UPROPERTY(VisibleAnywhere, Category = "Camera")
    USceneCaptureComponent2D* SceneCapture;

    // UPROPERTY��ǉ�����GC����ی삷��
    UPROPERTY()
    UTextureRenderTarget2D* RenderTarget;

    // UPROPERTY��ǉ�����GC����ی삷��
    UPROPERTY()
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

    // �o�b�t�@�p
    TArray<uint8> RawImageBytes;
    TArray<uint8> CompressedImageBytes;

    // �G���R�[�h�I���i0 = PNG, 1 = JPEG�j
    int32 EncodeType = 0;
};
