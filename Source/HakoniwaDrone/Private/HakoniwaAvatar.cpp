// Fill out your copyright notice in the Description page of Project Settings.


#include "HakoniwaAvatar.h"
#include "HakoniwaWebClient.h"
#include "geometry_msgs/pdu_cpptype_conv_Twist.hpp"
#include "hako_mavlink_msgs/pdu_cpptype_conv_HakoHilActuatorControls.hpp"
#include "hako_msgs/pdu_cpptype_conv_GameControllerOperation.hpp"
#include "hako_msgs/pdu_cpptype_conv_DroneStatus.hpp"
#include "pdu_convertor.hpp"
#include <Kismet/GameplayStatics.h>

// Sets default values
AHakoniwaAvatar::AHakoniwaAvatar()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}
void AHakoniwaAvatar::DeclarePdu()
{
    if (pduManager == nullptr) {
        if (const auto* GameInstance = GetGameInstance())
        {
            AHakoniwaWebClient* WebClient = Cast<AHakoniwaWebClient>(UGameplayStatics::GetActorOfClass(GetWorld(), AHakoniwaWebClient::StaticClass()));
            if (WebClient != nullptr)
            {
                pduManager = WebClient->GetPduManager();
            }
        }
    }
    if (pduManager == nullptr) {
        return;
    }
    if (isDeclared == false) {
        if (pduManager->IsServiceEnabled()) {
            if (pduManager->DeclarePduForRead(DroneName, "motor")) {
                UE_LOG(LogTemp, Log, TEXT("Successfully declared %s:motor"), *DroneName);
            }
            else {
                UE_LOG(LogTemp, Warning, TEXT("Failed to declare %s:motor"), *DroneName);
            }

            if (pduManager->DeclarePduForRead(DroneName, "pos")) {
                UE_LOG(LogTemp, Log, TEXT("Successfully declared %s:pos"), *DroneName);
            }
            else {
                UE_LOG(LogTemp, Warning, TEXT("Failed to declare %s:pos"), *DroneName);
            }
            if (pduManager->DeclarePduForRead(DroneName, "status")) {
                UE_LOG(LogTemp, Log, TEXT("Successfully declared %s:status"), *DroneName);
            }
            else {
                UE_LOG(LogTemp, Warning, TEXT("Failed to declare %s:status"), *DroneName);
            }
            isDeclared = true;
        }
        else {
            UE_LOG(LogTemp, Warning, TEXT("Can not be enabled pduManager service"));
        }
    }
}
void AHakoniwaAvatar::DoTask()
{
    TArray<uint8> buffer = Read("pos");
    if (buffer.Num() > 0) {
        HakoCpp_Twist pos;
        hako::pdu::PduConvertor<HakoCpp_Twist, hako::pdu::msgs::geometry_msgs::Twist> conv;
        conv.pdu2cpp((char*)buffer.GetData(), pos);
        FVector NewLocation(pos.linear.x * 100.0f, -pos.linear.y * 100.0f, pos.linear.z * 100.0f);

        FRotator NewRotation = FRotator(
            -FMath::RadiansToDegrees(pos.angular.y),   // Pitch ← ROSのPitch（Y軸）→ 符号反転
            -FMath::RadiansToDegrees(pos.angular.z),   // Yaw   ← ROSのYaw（Z軸） → 符号反転
            FMath::RadiansToDegrees(pos.angular.x)     // Roll  ← ROSのRoll（X軸）→ そのまま
        );

        if (FMath::IsFinite(NewLocation.X) && FMath::IsFinite(NewLocation.Y) && FMath::IsFinite(NewLocation.Z)) {
            AActor* ParentActor = this;
            if (ParentActor && IsValid(ParentActor))
            {
                FVector CurrentLocation = ParentActor->GetActorLocation();
                FRotator CurrentRotation = ParentActor->GetActorRotation();
                FVector CurrentScale = ParentActor->GetActorScale3D();
                // 急激な変更を避ける
                float MaxDistance = 1000.0f; // 1フレームでの最大移動距離
                if (FVector::Dist(CurrentLocation, NewLocation) > MaxDistance)
                {
                    UE_LOG(LogTemp, Warning, TEXT("Position change too large, skipping update"));
                    return;
                }

                // SetActorTransform を使用（より安全）
                FTransform NewTransform(NewRotation, NewLocation, CurrentScale);
                ParentActor->SetActorTransform(NewTransform, false, nullptr, ETeleportType::TeleportPhysics);
            }
        }
        else {
            UE_LOG(LogTemp, Error, TEXT("Invalid position: (%f, %f, %f)"), NewLocation.X, NewLocation.Y, NewLocation.Z);
        }
    }
    buffer = Read("motor");
    if (buffer.Num() > 0) {
        HakoCpp_HakoHilActuatorControls motor;
        hako::pdu::PduConvertor<HakoCpp_HakoHilActuatorControls, hako::pdu::msgs::hako_mavlink_msgs::HakoHilActuatorControls> conv;
        conv.pdu2cpp((char*)buffer.GetData(), motor);
        //UE_LOG(LogTemp, Log, TEXT("motor.time_usec = %llu"), motor.time_usec);

        if (Motor)
        {
            Motor->Rotate(
                motor.controls[0],
                motor.controls[1],
                motor.controls[2],
                motor.controls[3]
            );
        }
        is_motor_activated = motor.controls[0] > 0.001;
    }
    buffer = Read("status");
    HakoCpp_DroneStatus status;
    if (buffer.Num() > 0) {
        hako::pdu::PduConvertor<HakoCpp_DroneStatus, hako::pdu::msgs::hako_msgs::DroneStatus> conv;
        conv.pdu2cpp((char*)buffer.GetData(), status);
        if (status.flight_mode == 0) {
            FlightMode->SetMode(EFlightMode::ATTI);
        }
        else {
            FlightMode->SetMode(EFlightMode::GPS);
        }
    }
    if (is_motor_activated && buffer.Num() > 0) {
        switch (status.internal_state)
        {
        case 0:
            DroneState->SetMode(EDroneMode::TAKEOFF);
            break;
        case 1:
            DroneState->SetMode(EDroneMode::HOVER);
            break;
        case 2:
            DroneState->SetMode(EDroneMode::LANDING);
            break;
        default:
            break;
        }
    }
    else if (is_motor_activated == false) {
        DroneState->SetMode(EDroneMode::DISARM);
    }
}
TArray<uint8> AHakoniwaAvatar::Read(const FString& PduName)
{
    TArray<uint8> buffer;

    int32 pdu_size = pduManager->GetPduSize(DroneName, PduName);
    if (pdu_size <= 0) {
        UE_LOG(LogTemp, Error, TEXT("Can not get pdu size..."));
        return buffer;
    }
    //UE_LOG(LogTemp, Log, TEXT("Read PDU TEST: robot=%s, pdu=%s, size=%d"),*DroneName, *PduName, pdu_size);
    buffer.SetNum(pdu_size);

    if (pduManager->ReadPduRawData(DroneName, PduName, buffer)) {
        //UE_LOG(LogTemp, Log, TEXT("Read PDU succeeded: robot=%s, pdu=%s, size=%d"), *DroneName, *PduName, pdu_size);
        return buffer;
    }
    else {
        //UE_LOG(LogTemp, Error, TEXT("Failed to read PDU: robot=%s, pdu=%s"), *DroneName, *PduName);
        return TArray<uint8>();
    }
}
// Called when the game starts or when spawned
void AHakoniwaAvatar::BeginPlay()
{
	Super::BeginPlay();
    if (!Motor)
    {
        Motor = FindComponentByClass<UDronePropellerComponent>();
        if (!Motor)
        {
            UE_LOG(LogTemp, Error, TEXT("DronePropellerComponent not found!"));
        }
    }
    if (!DroneState)
    {
        DroneState = FindComponentByClass<UDroneLedComponent>();
        if (!DroneState)
        {
            UE_LOG(LogTemp, Error, TEXT("UDroneLedComponent not found!"));
        }
    }
    if (!FlightMode)
    {
        FlightMode = FindComponentByClass<UFlightModeLedComponent>();
        if (!FlightMode)
        {
            UE_LOG(LogTemp, Error, TEXT("UFlightModeLedComponent not found!"));
        }
    }
}

// Called every frame
void AHakoniwaAvatar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (!isDeclared) {
        DeclarePdu();
    }
    else {
        DoTask();
    }
}

