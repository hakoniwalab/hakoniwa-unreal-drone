// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PduManager.h"
#include "WebSocketCommunicationService.h"
#include "HakoniwaWebClient.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class HAKONIWADRONE_API AHakoniwaWebClient : public AActor
{
	GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hakoniwa")
    FString ConfigPath = "Config/webavatar.json";

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hakoniwa")
    FString WebSocketUrl = "ws://172.31.9.252:8765";

    UFUNCTION(BlueprintCallable, Category = "Hakoniwa")
    void Start();
    UFUNCTION(BlueprintCallable, Category = "Hakoniwa")
    void Stop();


    UFUNCTION(BlueprintCallable, Category = "Hakoniwa")
    UPduManager* GetPduManager() const { return pduManager; }

private:
    UPROPERTY()
    UWebSocketCommunicationService* service = nullptr;
    UPROPERTY()
    UPduManager* pduManager = nullptr;
};
