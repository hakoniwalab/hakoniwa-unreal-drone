// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DroneControlOp.h"
#include "CameraControllerInterface.h"
#include "PduManager.h"

#include "DroneControl.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HAKONIWADRONE_API UDroneControl : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDroneControl();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UPROPERTY(EditAnywhere, Category = "Hakoniwa")
	FString RobotName = "Drone";
private:
	UPROPERTY()
	TScriptInterface<IDroneControlOp> ControlOp;

	UPROPERTY()
	TScriptInterface<ICameraControllerInterface> CameraController;

	UPROPERTY()
	UPduManager* PduManager_;

	void HandleCameraControl(float DeltaTime);

	float move_step = 1.0f;  // 一回の動きのステップ量
	float camera_move_button_time_duration = 0;
	float camera_move_button_threshold_speedup = 1.0;
	bool is_pressed_up = false;
	bool is_pressed_down = false;
};
