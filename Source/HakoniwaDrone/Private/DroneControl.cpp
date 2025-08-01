// Fill out your copyright notice in the Description page of Project Settings.


#include "DroneControl.h"
#include <HakoniwaWebClient.h>

#include <Kismet/GameplayStatics.h>

// Sets default values for this component's properties
UDroneControl::UDroneControl()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UDroneControl::BeginPlay()
{
	Super::BeginPlay();

    AActor* Owner = GetOwner();
    if (!Owner) return;

    // --- IDroneControlOpを実装したコンポーネントを取得 ---
    // この方法だと、Pdu版でもXbox版でも、同じコードでコンポーネントを取得できます
    ControlOp = Owner->FindComponentByInterface(UDroneControlOp::StaticClass());

    if (!ControlOp)
    {
        UE_LOG(LogTemp, Warning, TEXT("DroneControl: IDroneControlOp component not found!"));
    }
    else {
        //ControlOp->DoInitialize(RobotName);
        IDroneControlOp::Execute_DoInitialize(ControlOp.GetObject(), RobotName);
    }

    // --- CameraControllerInterfaceを実装したコンポーネントを取得 ---
    CameraController = Owner->FindComponentByInterface(UCameraControllerInterface::StaticClass());

    if (!CameraController)
    {
        UE_LOG(LogTemp, Warning, TEXT("DroneControl: CameraControllerInterface component not found!"));
    }
}


// Called every frame
void UDroneControl::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
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
    if (PduManager_ != nullptr) {
        if (!IDroneControlOp::Execute_IsReady(ControlOp.GetObject())) {
            return;
        }
        if (CameraController->IsReady()) {
            HandleCameraControl(DeltaTime);
        }
    }
}

void UDroneControl::HandleCameraControl(float DeltaTime)
{
    if (IDroneControlOp::Execute_IsYButtonPressed(ControlOp.GetObject())) {
        UE_LOG(LogTemp, Log, TEXT("SHOT!!"));
        CameraController->Scan();
        CameraController->WriteCameraDataPdu(PduManager_);
    }
    if (IDroneControlOp::Execute_IsUpButtonPressed(ControlOp.GetObject()))
    {
        UE_LOG(LogTemp, Log, TEXT("Up Pressed"));
        is_pressed_up = true;
    }
    else if (IDroneControlOp::Execute_IsUpButtonReleased(ControlOp.GetObject()))
    {
        UE_LOG(LogTemp, Log, TEXT("Up Released"));
        is_pressed_up = false;
    }
    if (is_pressed_up)
    {
        camera_move_button_time_duration += DeltaTime;
        if (camera_move_button_time_duration > camera_move_button_threshold_speedup)
        {
            CameraController->RotateCamera(move_step * 3.0);
        }
        else
        {
            CameraController->RotateCamera(move_step);
        }
        CameraController->UpdateCameraAngle();
    }
    if (IDroneControlOp::Execute_IsDownButtonPressed(ControlOp.GetObject()))
    {
        UE_LOG(LogTemp, Log, TEXT("Down Pressed"));
        is_pressed_down = true;
    }
    else if (IDroneControlOp::Execute_IsDownButtonReleased(ControlOp.GetObject()))
    {
        UE_LOG(LogTemp, Log, TEXT("Down Released"));
        is_pressed_down = false;
    }

    if (is_pressed_down)
    {
        camera_move_button_time_duration += DeltaTime;
        if (camera_move_button_time_duration > camera_move_button_threshold_speedup)
        {
            CameraController->RotateCamera(-move_step * 3.0);
        }
        else
        {
            CameraController->RotateCamera(-move_step);
        }
        CameraController->UpdateCameraAngle();
    }

    if (!is_pressed_down && !is_pressed_up)
    {
        camera_move_button_time_duration = 0;
    }
}