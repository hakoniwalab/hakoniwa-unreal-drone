// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PduManager.h"
#include "WebSocketCommunicationService.h"
#include "DronePropellerComponent.h"

#include "PluginTester.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HAKONIWADRONE_API UPluginTester : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY()
	UWebSocketCommunicationService* service = nullptr;
	UPROPERTY()
	UPduManager* pduManager = nullptr;
	bool isDeclared = false;
	TArray<uint8> ReadTest(const FString& RobotName, const FString& PduName);
	bool WriteTest(const FString& RobotName, const FString& PduName, TArray<uint8> RawData);
	TArray<uint8> pdu_buffer_hako_cmd_game;

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hakoniwa")
	FString ConfigPath = "Config/webavatar.json";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hakoniwa")
	FString WebSocketUrl = "ws://172.31.9.252:8765";
	// Sets default values for this component's properties

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hakoniwa")
	UDronePropellerComponent* Motor = nullptr;

	UPluginTester();

protected:
	// Called when the game starts
	//virtual void BeginPlay() override;

	//virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:	
	// Called every frame
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Hakoniwa")
	void Initialize();
	UFUNCTION(BlueprintCallable, Category = "Hakoniwa")
	void Finalize();
	UFUNCTION(BlueprintCallable, Category = "Hakoniwa")
	void Tick();
};
