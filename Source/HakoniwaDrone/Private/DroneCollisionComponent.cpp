#include "DroneCollisionComponent.h"
#include "Components/BoxComponent.h"

UDroneCollisionComponent::UDroneCollisionComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UDroneCollisionComponent::BeginPlay()
{
    Super::BeginPlay();

    if (!TriggerBox)
    {
        UE_LOG(LogTemp, Warning, TEXT("TriggerBox is null"));
        TArray<USceneComponent*> Children;
        GetChildrenComponents(true, Children);
        for (USceneComponent* Child : Children)
        {
            if (auto* Box = Cast<UBoxComponent>(Child))
            {
                TriggerBox = Box;
                UE_LOG(LogTemp, Log, TEXT("TriggerBox is found!! name: %s"), *Box->GetFName().ToString());
                break;
            }
        }
        if (!TriggerBox) {
            UE_LOG(LogTemp, Error, TEXT("TriggerBox can not set..."));
            return;
        }
    }

    TriggerBox->SetGenerateOverlapEvents(true);
    TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &UDroneCollisionComponent::OnOverlapBegin);
}

void UDroneCollisionComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor || !OtherComp) return;
    UE_LOG(LogTemp, Warning, TEXT("Overlap Detected! %s"), *OtherActor->GetName());

    FVector SelfPos = GetComponentLocation();
    FVector TargetPos = OtherComp->GetComponentLocation();

    // ContactPointを安全に取得する
    FVector ContactPoint;
    float Distance = OtherComp->GetClosestPointOnCollision(SelfPos, ContactPoint);


    if (Distance < 0.0f)
    {
        UE_LOG(LogTemp, Warning, TEXT("GetClosestPointOnCollision failed, fallback to TargetPos"));
        ContactPoint = TargetPos;
    }

    FVector SelfContactVector = ContactPoint - SelfPos;
    FVector TargetContactVector = ContactPoint - TargetPos;

    UE_LOG(LogTemp, Log, TEXT("SelfPos: %s, TargetPos: %s, ContactPoint: %s"), *SelfPos.ToString(), *TargetPos.ToString(), *ContactPoint.ToString());
    UE_LOG(LogTemp, Log, TEXT("SelfContactVector: %s, TargetContactVector: %s"), *SelfContactVector.ToString(), *TargetContactVector.ToString());

    float Epsilon = 0.000001f;
    if (SelfContactVector.SizeSquared() < Epsilon * Epsilon)
    {
        UE_LOG(LogTemp, Warning, TEXT("Collision vector too small"));
        return;
    }

    CollisionInfo.bCollision = true;
    CollisionInfo.bIsTargetStatic = true;

    CollisionInfo.TargetVelocity = ConvertToRosVector(FVector::ZeroVector);
    CollisionInfo.TargetAngularVelocity = ConvertToRosAngular(FVector::ZeroVector);
    CollisionInfo.TargetEuler = ConvertToRosAngular(FVector::ZeroVector);

    CollisionInfo.SelfContactVector = ConvertToRosVector(SelfContactVector);
    CollisionInfo.TargetContactVector = ConvertToRosVector(TargetContactVector);
    CollisionInfo.TargetInertia = FVector(1, 1, 1);
    CollisionInfo.Normal = FVector::ZeroVector;  // Normal計算は別途対応？
    CollisionInfo.TargetMass = 1.0;
    CollisionInfo.RestitutionCoefficient = 1.0;

    UE_LOG(LogTemp, Log, TEXT("Collision! ROS vector: %s"), *CollisionInfo.SelfContactVector.ToString());
}


FDroneImpulseCollision UDroneCollisionComponent::GetAndResetCollision()
{
    FDroneImpulseCollision Result = CollisionInfo;
    CollisionInfo.bCollision = false;
    return Result;
}

FVector UDroneCollisionComponent::ConvertToRosVector(const FVector& UnrealVec) const
{
    return FVector(
        UnrealVec.X * 0.01f,
        -UnrealVec.Y * 0.01f,
        UnrealVec.Z * 0.01f
    );
}


FVector UDroneCollisionComponent::ConvertToRosAngular(const FVector& UnrealVec) const
{
    return FVector(
        -UnrealVec.X,
        UnrealVec.Y,
        -UnrealVec.Z
    );
}
