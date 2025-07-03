// PluginTester.cpp

#include "PluginTester.h"
#include "Modules/ModuleManager.h"
#include "geometry_msgs/pdu_cpptype_conv_Twist.hpp"
#include "hako_mavlink_msgs/pdu_cpptype_conv_HakoHilActuatorControls.hpp"
#include "hako_msgs/pdu_cpptype_conv_GameControllerOperation.hpp"
#include "pdu_convertor.hpp"

UPluginTester::UPluginTester()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UPluginTester::Initialize()
{
    //Super::BeginPlay();

    UE_LOG(LogTemp, Log, TEXT("PluginTester BeginPlay"));

    // 例：プラグインの手動ロード
    const FName ModuleName = "HakoniwaPdu";
    if (!FModuleManager::Get().IsModuleLoaded(ModuleName))
    {
        FModuleManager::Get().LoadModule(ModuleName);
    }

    service = NewObject<UWebSocketCommunicationService>();
    pduManager = NewObject<UPduManager>();

    pduManager->Initialize(ConfigPath, service);
    pduManager->StartService(WebSocketUrl);

    if (!Motor)
    {
        Motor = GetOwner()->FindComponentByClass<UDronePropellerComponent>();
        if (!Motor)
        {
            UE_LOG(LogTemp, Error, TEXT("DronePropellerComponent not found!"));
        }
    }
}

void UPluginTester::Tick()
{

    if (isDeclared == false) {
        if (pduManager->IsServiceEnabled()) {
            if (pduManager->DeclarePduForRead("Drone", "motor")) {
                UE_LOG(LogTemp, Log, TEXT("Successfully declared Drone:pos"));
            }
            else {
                UE_LOG(LogTemp, Warning, TEXT("Failed to declare Drone:pos"));
            }
            if (pduManager->DeclarePduForRead("Drone", "pos")) {
                UE_LOG(LogTemp, Log, TEXT("Successfully declared Drone:motor"));
            }
            else {
                UE_LOG(LogTemp, Warning, TEXT("Failed to declare Drone:motor"));
            }
            isDeclared = true;
        }
        else {
            UE_LOG(LogTemp, Warning, TEXT("Can not be enabled pduManager service"));
        }
    }
    else {
        TArray<uint8> buffer = ReadTest("Drone", "pos");
        if (buffer.Num() > 0) {
            HakoCpp_Twist pos;
            hako::pdu::PduConvertor<HakoCpp_Twist, hako::pdu::msgs::geometry_msgs::Twist> conv;
            conv.pdu2cpp((char*)buffer.GetData(), pos);
#if 0
            UE_LOG(LogTemp, Log, TEXT("Twist.linear = (%lf, %lf, %lf), angular = (%lf, %lf, %lf)"),
                pos.linear.x, pos.linear.y, pos.linear.z,
                pos.angular.x, pos.angular.y, pos.angular.z);
#endif
            FVector NewLocation(pos.linear.x * 100.0f, -pos.linear.y * 100.0f, pos.linear.z * 100.0f);

            FRotator NewRotation = FRotator(
                -FMath::RadiansToDegrees(pos.angular.y),   // Pitch ← ROSのPitch（Y軸）→ 符号反転
                -FMath::RadiansToDegrees(pos.angular.z),   // Yaw   ← ROSのYaw（Z軸） → 符号反転
                FMath::RadiansToDegrees(pos.angular.x)     // Roll  ← ROSのRoll（X軸）→ そのまま
            );

            if (FMath::IsFinite(NewLocation.X) && FMath::IsFinite(NewLocation.Y) && FMath::IsFinite(NewLocation.Z)) {
                AActor* ParentActor = GetOwner();
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
        buffer = ReadTest("Drone", "motor");
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
        }

    }

}
TArray<uint8> UPluginTester::ReadTest(const FString& RobotName, const FString& PduName)
{
    TArray<uint8> buffer;

    int32 pdu_size = pduManager->GetPduSize(RobotName, PduName);
    if (pdu_size <= 0) {
        UE_LOG(LogTemp, Error, TEXT("Can not get pdu size..."));
        return buffer;  // 空の配列を返す
    }
    //UE_LOG(LogTemp, Log, TEXT("Read PDU TEST: robot=%s, pdu=%s, size=%d"),*RobotName, *PduName, pdu_size);
    buffer.SetNum(pdu_size);

    if (pduManager->ReadPduRawData(RobotName, PduName, buffer)) {
        //UE_LOG(LogTemp, Log, TEXT("Read PDU succeeded: robot=%s, pdu=%s, size=%d"), *RobotName, *PduName, pdu_size);
        return buffer;
    }
    else {
        //UE_LOG(LogTemp, Error, TEXT("Failed to read PDU: robot=%s, pdu=%s"), *RobotName, *PduName);
        return TArray<uint8>();  // 読み取り失敗時は空の配列を返す
    }
}
bool UPluginTester::WriteTest(const FString& RobotName, const FString& PduName, TArray<uint8> RawData)
{
    bool ret = pduManager->FlushPduRawData(RobotName, PduName, RawData);
    if (ret == false) {
        UE_LOG(LogTemp, Error, TEXT("Can not write pdu..."));
    }
    else {
        UE_LOG(LogTemp, Log, TEXT("write success: %s %s %d"), *RobotName, *PduName, RawData.Num());
    }
    return ret;
}
void UPluginTester::Finalize()
{

    if (pduManager && IsValid(pduManager)) {
        pduManager->StopService();
    }
    FPlatformProcess::Sleep(0.5f);
    pduManager = nullptr;
    service = nullptr;

}
