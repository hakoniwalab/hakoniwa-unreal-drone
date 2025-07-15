#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DroneControlOp.generated.h"

UINTERFACE(MinimalAPI)
class UDroneControlOp : public UInterface
{
	GENERATED_BODY()
};

/**
 * @class IDroneControlOp
 * @brief ドローンを操作するためのC++インタフェース。
 */
class HAKONIWADRONE_API IDroneControlOp
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Drone Control")
	void DoInitialize(const FString& RobotName);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Drone Control")
	void Run();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Drone Control")
	bool IsReady();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Drone Control")
	void Flush();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Drone Input")
	FVector2D GetLeftStickInput();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Drone Input")
	FVector2D GetRightStickInput();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Drone Input")
	bool IsAButtonPressed();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Drone Input")
	bool IsAButtonReleased();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Drone Input")
	bool IsBButtonPressed();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Drone Input")
	bool IsBButtonReleased();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Drone Input")
	bool IsXButtonPressed();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Drone Input")
	bool IsXButtonReleased();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Drone Input")
	bool IsYButtonPressed();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Drone Input")
	bool IsYButtonReleased();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Drone Input")
	bool IsUpButtonPressed();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Drone Input")
	bool IsUpButtonReleased();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Drone Input")
	bool IsDownButtonPressed();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Drone Input")
	bool IsDownButtonReleased();

};