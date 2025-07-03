// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HakoniwaDrone/DronePropellerComponent.h"
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
private:
	UPduManager* pduManager = nullptr;
	bool isDeclared = false;
	UDronePropellerComponent* Motor = nullptr;
	void DeclarePdu();
	void DoTask();
	TArray<uint8> Read(const FString& PduName);

};
