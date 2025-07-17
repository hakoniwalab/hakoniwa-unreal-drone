// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DronePropellerComponent.h"
#include "DroneLedComponent.h"
#include "FlightModeLedComponent.h"
#include "PduManager.h"

#include "HakoniwaAvatar.generated.h"

UCLASS(Blueprintable, BlueprintType)
class HAKONIWADRONE_API AHakoniwaAvatar : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHakoniwaAvatar();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hakoniwa")
	FString DroneName = "Drone";

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPduManager* GetPduManager() { return pduManager; }
private:
	bool isDeclared = false;
	UPROPERTY()
	UPduManager* pduManager = nullptr;
	UPROPERTY()
	UDronePropellerComponent* Motor = nullptr;
	UPROPERTY()
	UDroneLedComponent* DroneState = nullptr;
	UPROPERTY()
	UFlightModeLedComponent* FlightMode = nullptr;
	void DeclarePdu();
	void DoTask();
	TArray<uint8> Read(const FString& PduName);
	bool is_motor_activated = false;

};
