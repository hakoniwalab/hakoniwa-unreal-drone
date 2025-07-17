#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "DroneCollisionComponent.generated.h"

class UBoxComponent;

USTRUCT(BlueprintType)
struct HAKONIWADRONE_API FDroneImpulseCollision
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    bool bCollision = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    bool bIsTargetStatic = true;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FVector TargetVelocity = FVector::ZeroVector;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FVector TargetAngularVelocity = FVector::ZeroVector;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FVector TargetEuler = FVector::ZeroVector;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FVector SelfContactVector = FVector::ZeroVector;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FVector TargetContactVector = FVector::ZeroVector;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FVector TargetInertia = FVector::OneVector;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FVector Normal = FVector::ZeroVector;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    double TargetMass = 1.0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    double RestitutionCoefficient = 0.5;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class HAKONIWADRONE_API UDroneCollisionComponent : public USceneComponent
{
    GENERATED_BODY()

public:
    UDroneCollisionComponent();

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

public:
    // BoxComponent はBPで割り当て
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision", meta = (AllowPrivateAccess = "true"))
    UBoxComponent* TriggerBox;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
    bool bIsHakoniwa = true;

    UPROPERTY(BlueprintReadOnly, Category = "Collision")
    FDroneImpulseCollision CollisionInfo;

    // 衝突情報の取得＆リセット（Unityの GetImpulseCollision に相当）
    UFUNCTION(BlueprintCallable)
    FDroneImpulseCollision GetAndResetCollision();

private:
    FVector ConvertToRosVector(const FVector& UnrealVec) const;
    FVector ConvertToRosAngular(const FVector& UnrealVec) const;
};
