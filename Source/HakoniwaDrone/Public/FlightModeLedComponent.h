#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FlightModeLedComponent.generated.h"

UENUM(BlueprintType)
enum class EFlightMode : uint8
{
    ATTI    UMETA(DisplayName = "ATTI"),
    GPS     UMETA(DisplayName = "GPS")
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class HAKONIWADRONE_API UFlightModeLedComponent : public USceneComponent
{
    GENERATED_BODY()

public:
    UFlightModeLedComponent();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LED")
    UStaticMeshComponent* LedMesh = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
    UMaterialInterface* AttiMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
    UMaterialInterface* GpsMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
    UMaterialInterface* OffMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blink")
    float AttiBlinkInterval = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blink")
    float GpsBlinkInterval = 0.5f;

    UFUNCTION(BlueprintCallable)
    void SetMode(EFlightMode NewMode);

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    EFlightMode CurrentMode = EFlightMode::GPS;
    float BlinkTimer = 0.f;
    bool bLedOn = false;

    void UpdateBlink(float Interval, UMaterialInterface* ModeMaterial);
    void UpdateLedImmediate();
    void SetLedOn(bool bOn, UMaterialInterface* ModeMaterial);
};
