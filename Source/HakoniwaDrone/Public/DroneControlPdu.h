// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DroneControlOp.h"
#include "PduManager.h"
#include "hako_msgs/pdu_cpptype_conv_GameControllerOperation.hpp" 

#include "DroneControlPdu.generated.h"

namespace GameOps
{
	const int32 ArmButtonIndex = 0;
	const int32 GrabBaggageButtonIndex = 1;
	const int32 CameraButtonIndex = 2;
	const int32 FlightModeChangeIndex = 3;
	const int32 CameraMoveUpIndex = 11;
	const int32 CameraMoveDownIndex = 12;

	const int32 StickTurnLR = 0;
	const int32 StickUpDown = 1;
	const int32 StickMoveLR = 2;
	const int32 StickMoveFB = 3;
}

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HAKONIWADRONE_API UDroneControlPdu : public UActorComponent, public IDroneControlOp
{
	GENERATED_BODY()

public:
	UDroneControlPdu();

	// Editorから設定可能な変数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hakoniwa PDU")
	FString RobotName = TEXT("Drone");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hakoniwa PDU")
	FString PduName = TEXT("hako_cmd_game");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hakoniwa PDU|Magnet")
	bool bUseMagnet = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hakoniwa PDU|Magnet")
	FString PduNameCmdMagnet = TEXT("hako_cmd_magnet_holder");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hakoniwa PDU|Magnet")
	FString PduNameStatusMagnet = TEXT("hako_status_magnet_holder");

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// コンポーネントが初期化される際に呼び出されます (UnityのAwakeやStartに相当)
	virtual void InitializeComponent() override;

	// Hakoniwaライブラリとの連携用
	void DeclarePdu();

private:
	// PDUマネージャーへのポインタ
	UPROPERTY()
	UPduManager* PduManager_;

	bool IsDeclared = false;

	// ボタンと軸の状態を保持する配列
	HakoCpp_GameControllerOperation CurrStates;
	HakoCpp_GameControllerOperation PrevStates;

#if 0 //not supported yet
	// 電磁石の状態
	bool bStatusMagnet_MagnetOn = false;
	bool bStatusMagnet_ContactOn = false;
#endif

	bool IsButtonPressed(int index) const;
	bool IsButtonReleased(int index) const;
public:
	// --- IDroneControlOp Interface Implementation ---
	// インタフェースで定義した各関数の実体を定義します。
	// BlueprintNativeEventなので、関数名の末尾に「_Implementation」をつけます。
	virtual void DoInitialize_Implementation(const FString& InRobotName) override;
	virtual bool IsReady_Implementation() override;
	virtual void Run_Implementation() override;
	virtual void Flush_Implementation() override;
	virtual FVector2D GetLeftStickInput_Implementation() override;
	virtual FVector2D GetRightStickInput_Implementation() override;
	virtual bool IsAButtonPressed_Implementation() override;
	virtual bool IsAButtonReleased_Implementation() override;
	virtual bool IsBButtonPressed_Implementation() override;
	virtual bool IsBButtonReleased_Implementation() override;
	virtual bool IsXButtonPressed_Implementation() override;
	virtual bool IsXButtonReleased_Implementation() override;
	virtual bool IsYButtonPressed_Implementation() override;
	virtual bool IsYButtonReleased_Implementation() override;
	virtual bool IsUpButtonPressed_Implementation() override;
	virtual bool IsUpButtonReleased_Implementation() override;
	virtual bool IsDownButtonPressed_Implementation() override;
	virtual bool IsDownButtonReleased_Implementation() override;

};
