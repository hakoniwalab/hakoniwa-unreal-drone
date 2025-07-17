// DroneLedComponent.cpp

#include "DroneLedComponent.h"

UDroneLedComponent::UDroneLedComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UDroneLedComponent::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Log, TEXT("LED BeginPlay()"));

    if (!LedMesh && GetOwner())
    {
        TArray<UActorComponent*> Components;
        GetOwner()->GetComponents(Components);

        for (UActorComponent* Comp : Components)
        {
            if (Comp && Comp->GetName().Equals(TEXT("LED_status")))
            {
                LedMesh = Cast<UStaticMeshComponent>(Comp);
                if (LedMesh)
                {
                    UE_LOG(LogTemp, Log, TEXT("LED STATUS mesh is found via GetOwner().GetComponents()."));
                }
                break;
            }
        }
    }

    if (!LedMesh)
    {
        UE_LOG(LogTemp, Warning, TEXT("LED STATUS mesh is not found."));
    }

    UpdateLedImmediate();
}


void UDroneLedComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    switch (CurrentMode)
    {
    case EDroneMode::DISARM:
        SetLedOn(false, DisarmMaterial);
        break;
    case EDroneMode::HOVER:
        SetLedOn(true, HoverMaterial);
        break;
    case EDroneMode::TAKEOFF:
        UpdateBlink(TakeoffBlinkInterval, TakeoffMaterial);
        break;
    case EDroneMode::LANDING:
        UpdateBlink(LandingBlinkInterval, LandingMaterial);
        break;
    }
}
void UDroneLedComponent::UpdateBlink(float Interval, UMaterialInterface* ModeMaterial)
{
    BlinkTimer += GetWorld()->GetDeltaSeconds();
    if (BlinkTimer >= Interval)
    {
        BlinkTimer = 0.f;
        bLedOn = !bLedOn;
        //UE_LOG(LogTemp, Log, TEXT("DroneLED: Blink toggled -> %s"), bLedOn ? TEXT("ON") : TEXT("OFF"));
        SetLedOn(bLedOn, ModeMaterial);
    }
}


void UDroneLedComponent::UpdateLedImmediate()
{
    switch (CurrentMode)
    {
    case EDroneMode::DISARM:
        SetLedOn(false, DisarmMaterial);
        break;
    case EDroneMode::HOVER:
        SetLedOn(true, HoverMaterial);
        break;
    case EDroneMode::TAKEOFF:
    case EDroneMode::LANDING:
        SetLedOn(false, TakeoffMaterial); // èâä˙èÛë‘OFF
        break;
    }
}
void UDroneLedComponent::SetMode(EDroneMode NewMode)
{
    if (CurrentMode != NewMode)
    {
        UE_LOG(LogTemp, Log, TEXT("DroneLED: Mode changed from %d to %d"), static_cast<int32>(CurrentMode), static_cast<int32>(NewMode));
        CurrentMode = NewMode;
        BlinkTimer = 0.f;
        bLedOn = false;
        UpdateLedImmediate();
    }
}
void UDroneLedComponent::SetLedOn(bool bOn, UMaterialInterface* ModeMaterial)
{
    if (!LedMesh)
    {
        UE_LOG(LogTemp, Warning, TEXT("DroneLED: LedMesh is null in SetLedOn"));
        return;
    }

    UMaterialInterface* MaterialToApply = bOn ? ModeMaterial : DisarmMaterial;
    if (MaterialToApply)
    {
        LedMesh->SetMaterial(0, MaterialToApply);
#if 0
        UE_LOG(LogTemp, Log, TEXT("DroneLED: Set material to %s (bOn = %s)"),
            *MaterialToApply->GetName(),
            bOn ? TEXT("true") : TEXT("false"));
#endif
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("DroneLED: MaterialToApply is null (bOn = %s)"), bOn ? TEXT("true") : TEXT("false"));
    }
}
