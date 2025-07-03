// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Sound/SoundCue.h"
#include "DronePropellerComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class HAKONIWADRONE_API UDronePropellerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDronePropellerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
    UFUNCTION(BlueprintCallable, Category = "Drone")
    void Rotate(float c1, float c2, float c3, float c4);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Propellers", meta = (ExposeOnSpawn = true))
    USceneComponent* Propeller1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Propellers", meta = (ExposeOnSpawn = true))
    USceneComponent* Propeller2;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Propellers", meta = (ExposeOnSpawn = true))
    USceneComponent* Propeller3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Propellers", meta = (ExposeOnSpawn = true))
    USceneComponent* Propeller4;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Propellers", meta = (ExposeOnSpawn = true))
    USceneComponent* Propeller5;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Propellers", meta = (ExposeOnSpawn = true))
    USceneComponent* Propeller6;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    USoundCue* AudioCue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    bool bEnableAudio = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    float MaxRotationSpeed = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    float MaxDistance = 500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    float MinDistance = 0.0f;

    //UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    //ACameraActor* TargetCamera;

private:
    //UAudioComponent* AudioComponent;

    void RotatePropeller(USceneComponent* Propeller, float DutyRate);
    void PlayAudio(float ControlValue);
		
};
