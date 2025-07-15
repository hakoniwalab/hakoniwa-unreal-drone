// Fill out your copyright notice in the Description page of Project Settings.


#include "DroneControlPdu.h"
#include "hako_msgs/pdu_cpptype_conv_HakoCmdMagnetHolder.hpp" 
#include "hako_msgs/pdu_cpptype_conv_HakoStatusMagnetHolder.hpp"
#include "pdu_convertor.hpp"

// Sets default values for this component's properties
UDroneControlPdu::UDroneControlPdu()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UDroneControlPdu::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UDroneControlPdu::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UDroneControlPdu::InitializeComponent()
{
	Super::InitializeComponent();

	CurrStates.axis = {};
	CurrStates.button = {};
	PrevStates.axis = {};
	PrevStates.button = {};
}

void UDroneControlPdu::DeclarePdu()
{
	if (!PduManager)
	{
		UE_LOG(LogTemp, Error, TEXT("UDroneControlPdu: Can not get Pdu Manager"));
		return;
	}

	bool bResult = PduManager->DeclarePduForWrite(RobotName, PduName);
	UE_LOG(LogTemp, Log, TEXT("Declare PDU %s: %s"), *PduName, bResult ? TEXT("Success") : TEXT("Failed"));

#if 0 //not supported yet
	if (bUseMagnet)
	{
		bResult = PduManager->DeclarePduForRead(RobotName, PduNameCmdMagnet);
		if (!bResult)
		{
			UE_LOG(LogTemp, Error, TEXT("Can not declare pdu for read: %s %s"), *RobotName, *PduNameCmdMagnet);
		}

		bResult = PduManager->DeclarePduForWrite(RobotName, PduNameStatusMagnet);
		if (!bResult)
		{
			UE_LOG(LogTemp, Error, TEXT("Can not declare pdu for write: %s %s"), *RobotName, *PduNameStatusMagnet);
		}
	}
#endif
}


// --- インタフェース関数の実装 ---

void UDroneControlPdu::DoInitialize_Implementation(const FString& InRobotName)
{
	this->RobotName = InRobotName;
}
void UDroneControlPdu::Run_Implementation()
{
	if (!PduManager) {
		return;
	}
	//read gamectrl
	int32 PduSize = PduManager->GetPduSize(RobotName, PduName);
	if (PduSize <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("WriteCameraInfo: Failed to get PDU size for %s:%s"), *RobotName, *PduName);
		return;
	}
	TArray<uint8> Buffer;
	Buffer.SetNum(PduSize);
	if (PduManager->ReadPduRawData(RobotName, PduName, Buffer)) {
		PrevStates = CurrStates;
		hako::pdu::PduConvertor<HakoCpp_GameControllerOperation, hako::pdu::msgs::hako_msgs::GameControllerOperation> Conv;
		Conv.pdu2cpp((char*)Buffer.GetData(), CurrStates);
	}
}

void UDroneControlPdu::Flush_Implementation()
{
	if (!PduManager) {
		return;
	}
#if 0 //not supported yet
	/*
	 * Magnet
	 */
	if (bUseMagnet)
	{
		int32 PduSize = PduManager->GetPduSize(RobotName, PduNameStatusMagnet);
		if (PduSize <= 0)
		{
			UE_LOG(LogTemp, Error, TEXT("WriteCameraInfo: Failed to get PDU size for %s:%s"), *RobotName, *PduNameStatusMagnet);
			return;
		}

		TArray<uint8> Buffer;
		Buffer.SetNum(PduSize);

		HakoCpp_HakoStatusMagnetHolder status_magnet;
		status_magnet.magnet_on = bStatusMagnet_MagnetOn;
		status_magnet.contact_on = bStatusMagnet_ContactOn;
		hako::pdu::PduConvertor<HakoCpp_HakoStatusMagnetHolder, hako::pdu::msgs::hako_msgs::HakoStatusMagnetHolder> Conv;
		Conv.cpp2pdu(status_magnet, (char*)Buffer.GetData(), Buffer.Num());
		PduManager->FlushPduRawData(RobotName, PduNameStatusMagnet, Buffer);
	}
	UE_LOG(LogTemp, Log, TEXT("DoFlush Called. PDU writing logic should be implemented here."));
#endif
}
bool UDroneControlPdu::IsButtonPressed(int32 ButtonIndex) const
{
	// 「今回は押されている」AND「前回は押されていなかった」
	return CurrStates.button[ButtonIndex] && !PrevStates.button[ButtonIndex];
}

bool UDroneControlPdu::IsButtonReleased(int32 ButtonIndex) const
{
	// 「今回は押されていない」AND「前回は押されていた」
	return !CurrStates.button[ButtonIndex] && PrevStates.button[ButtonIndex];
}

FVector2D UDroneControlPdu::GetLeftStickInput_Implementation()
{
	FVector2D vec;
	vec.X = CurrStates.axis[GameOps::StickUpDown];
	vec.Y = CurrStates.axis[GameOps::StickTurnLR];
	return vec;
}

FVector2D UDroneControlPdu::GetRightStickInput_Implementation()
{
	FVector2D vec;
	vec.X = CurrStates.axis[GameOps::StickMoveFB];
	vec.Y = CurrStates.axis[GameOps::StickMoveLR];
	return vec;
}

bool UDroneControlPdu::IsAButtonPressed_Implementation()
{
	return IsButtonPressed(GameOps::ArmButtonIndex);
}

bool UDroneControlPdu::IsAButtonReleased_Implementation()
{
	return IsButtonReleased(GameOps::ArmButtonIndex);
}

bool UDroneControlPdu::IsBButtonPressed_Implementation()
{
	return IsButtonPressed(GameOps::GrabBaggageButtonIndex);
}

bool UDroneControlPdu::IsBButtonReleased_Implementation()
{
	return IsButtonReleased(GameOps::GrabBaggageButtonIndex);
}

bool UDroneControlPdu::IsXButtonPressed_Implementation()
{
	return IsButtonPressed(GameOps::FlightModeChangeIndex);
}

bool UDroneControlPdu::IsXButtonReleased_Implementation()
{
	return IsButtonReleased(GameOps::FlightModeChangeIndex);
}

bool UDroneControlPdu::IsYButtonPressed_Implementation()
{
	return IsButtonPressed(GameOps::CameraButtonIndex);
}

bool UDroneControlPdu::IsYButtonReleased_Implementation()
{
	return IsButtonReleased(GameOps::CameraButtonIndex);
}

bool UDroneControlPdu::IsUpButtonPressed_Implementation()
{
	return IsButtonPressed(GameOps::CameraMoveUpIndex);
}

bool UDroneControlPdu::IsUpButtonReleased_Implementation()
{
	return IsButtonReleased(GameOps::CameraMoveUpIndex);
}

bool UDroneControlPdu::IsDownButtonPressed_Implementation()
{
	return IsButtonPressed(GameOps::CameraMoveDownIndex);
}

bool UDroneControlPdu::IsDownButtonReleased_Implementation()
{
	return IsButtonReleased(GameOps::CameraMoveDownIndex);
}
