#include "FlightModeLedComponent.h"

UFlightModeLedComponent::UFlightModeLedComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UFlightModeLedComponent::BeginPlay()
{
    Super::BeginPlay();

    if (!LedMesh && GetOwner())
    {
        TArray<UActorComponent*> Components;
        GetOwner()->GetComponents(Components);
        for (auto* Comp : Components)
        {
            if (Comp && Comp->GetName().Equals(TEXT("LED_flight_mode")))
            {
                LedMesh = Cast<UStaticMeshComponent>(Comp);
                break;
            }
        }
    }

    UpdateLedImmediate();
}

void UFlightModeLedComponent::SetMode(EFlightMode NewMode)
{
    if (CurrentMode != NewMode)
    {
        CurrentMode = NewMode;
        BlinkTimer = 0.f;
        bLedOn = false;
        UpdateLedImmediate();
    }
}

void UFlightModeLedComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    switch (CurrentMode)
    {
    case EFlightMode::ATTI:
        UpdateBlink(AttiBlinkInterval, AttiMaterial);
        break;
    case EFlightMode::GPS:
        UpdateBlink(GpsBlinkInterval, GpsMaterial);
        break;
    }
}

void UFlightModeLedComponent::UpdateBlink(float Interval, UMaterialInterface* ModeMaterial)
{
    BlinkTimer += GetWorld()->GetDeltaSeconds();
    if (BlinkTimer >= Interval)
    {
        BlinkTimer = 0.f;
        bLedOn = !bLedOn;
        SetLedOn(bLedOn, ModeMaterial);
    }
}

void UFlightModeLedComponent::UpdateLedImmediate()
{
    switch (CurrentMode)
    {
    case EFlightMode::ATTI:
        SetLedOn(false, AttiMaterial);
        break;
    case EFlightMode::GPS:
        SetLedOn(false, GpsMaterial);
        break;
    }
}

void UFlightModeLedComponent::SetLedOn(bool bOn, UMaterialInterface* ModeMaterial)
{
    if (!LedMesh) return;

    UMaterialInterface* MaterialToApply = bOn ? ModeMaterial : OffMaterial;
    if (MaterialToApply)
    {
        LedMesh->SetMaterial(0, MaterialToApply);
    }
}
