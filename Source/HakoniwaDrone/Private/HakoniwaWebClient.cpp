// Fill out your copyright notice in the Description page of Project Settings.


#include "HakoniwaWebClient.h"

void AHakoniwaWebClient::Start()
{
    UE_LOG(LogTemp, Log, TEXT("AHakoniwaWebClient BeginPlay()"));

    const FName ModuleName = "HakoniwaPdu";

    // Ç∑Ç≈Ç…ÉçÅ[ÉhçœÇ›Ç»ÇÁâΩÇ‡ÇµÇ»Ç¢
    if (FModuleManager::Get().IsModuleLoaded(ModuleName))
    {
        UE_LOG(LogTemp, Log, TEXT("Module '%s' already loaded."), *ModuleName.ToString());
    }
    else
    {
        FModuleManager::Get().LoadModule(ModuleName);
        UE_LOG(LogTemp, Log, TEXT("Module '%s' loaded."), *ModuleName.ToString());
    }

    if (pduManager)
    {
        UE_LOG(LogTemp, Log, TEXT("UHakoniwaWebClient already initialized, skipping."));
        return;
    }

    service = NewObject<UWebSocketCommunicationService>(this);
    pduManager = NewObject<UPduManager>(this);

    pduManager->Initialize(ConfigPath, service);
    pduManager->StartService(WebSocketUrl);

    UE_LOG(LogTemp, Log, TEXT("UHakoniwaWebClient BeginPlay() successfully finished."));
}

void AHakoniwaWebClient::Stop()
{
    if (pduManager && IsValid(pduManager))
    {
        UE_LOG(LogTemp, Log, TEXT("Stopping PduManager..."));
        pduManager->StopService();
    }

    FPlatformProcess::Sleep(0.5f);

    pduManager = nullptr;
    service = nullptr;

    UE_LOG(LogTemp, Log, TEXT("UHakoniwaWebClient Finalize() complete."));
}