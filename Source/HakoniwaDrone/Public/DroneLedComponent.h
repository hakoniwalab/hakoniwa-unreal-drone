// DroneLedComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DroneLedComponent.generated.h"

UENUM(BlueprintType)
enum class EDroneMode : uint8
{
    DISARM     UMETA(DisplayName = "Disarm"),
    TAKEOFF    UMETA(DisplayName = "Takeoff"),
    HOVER      UMETA(DisplayName = "Hover"),
    LANDING    UMETA(DisplayName = "Landing")
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class HAKONIWADRONE_API UDroneLedComponent : public USceneComponent
{
    GENERATED_BODY()

public:
    UDroneLedComponent();

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable)
    void SetMode(EDroneMode NewMode);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LED")
    UMeshComponent* LedMesh;

    UPROPERTY(EditAnywhere, Category = "LED|Materials")
    UMaterialInterface* DisarmMaterial;

    UPROPERTY(EditAnywhere, Category = "LED|Materials")
    UMaterialInterface* TakeoffMaterial;

    UPROPERTY(EditAnywhere, Category = "LED|Materials")
    UMaterialInterface* HoverMaterial;

    UPROPERTY(EditAnywhere, Category = "LED|Materials")
    UMaterialInterface* LandingMaterial;

    UPROPERTY(EditAnywhere, Category = "LED|Blink")
    float TakeoffBlinkInterval = 0.5f;

    UPROPERTY(EditAnywhere, Category = "LED|Blink")
    float LandingBlinkInterval = 0.5f;

private:
    EDroneMode CurrentMode = EDroneMode::DISARM;
    float BlinkTimer = 0.f;
    bool bLedOn = false;

    void UpdateLedImmediate();
    void UpdateBlink(float Interval, UMaterialInterface* ModeMaterial);
    void SetLedOn(bool bOn, UMaterialInterface* ModeMaterial);
};
