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
    UE_LOG(LogTemp, Warning, TEXT("Overlap Detected!% s"), *OtherActor->GetName());

    FVector ContactPoint = SweepResult.ImpactPoint;
    FVector SelfPos = GetComponentLocation();
    FVector SelfContactVector = ContactPoint - SelfPos;
    FVector TargetPos = OtherComp->GetComponentLocation();
    FVector TargetContactVector = ContactPoint - TargetPos;

    float Epsilon = 0.0001f;
    if (SelfContactVector.SizeSquared() < Epsilon * Epsilon)
    {
        UE_LOG(LogTemp, Warning, TEXT("Collision vector too small"));
        return;
    }

    CollisionInfo.bCollision = true;
    CollisionInfo.bIsTargetStatic = true;

    // �ÓI���̂̂��ߑ��x�Ȃǂ�0
    CollisionInfo.TargetVelocity = ConvertToRosVector(FVector::ZeroVector);
    CollisionInfo.TargetAngularVelocity = ConvertToRosAngular(FVector::ZeroVector);
    CollisionInfo.TargetEuler = ConvertToRosAngular(FVector::ZeroVector);

    CollisionInfo.SelfContactVector = ConvertToRosVector(SelfContactVector);
    CollisionInfo.TargetContactVector = ConvertToRosVector(TargetContactVector);
    CollisionInfo.TargetInertia = FVector::OneVector;
    CollisionInfo.Normal = ConvertToRosVector(-SelfContactVector.GetSafeNormal());
    CollisionInfo.TargetMass = 1.0;
    CollisionInfo.RestitutionCoefficient = 0.5;

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
    // Unreal (X:�E, Y:�O, Z:��) �� ROS (X:�O, Y:��, Z:��)
    return FVector(
        -UnrealVec.Y,  // ROS.X �� Unreal.Y
        UnrealVec.X,  // ROS.Y �� Unreal.X�i�E�����j
        UnrealVec.Z   // ROS.Z �� Unreal.Z�i��j
    );
}


FVector UDroneCollisionComponent::ConvertToRosAngular(const FVector& UnrealVec) const
{
    // Unreal (����n) �� ROS (�E��n) �ɕϊ��{��]�������]
    return FVector(
        UnrealVec.Y,   // ROS.X �� Unreal.Y
        -UnrealVec.X,   // ROS.Y �� Unreal.X
        -UnrealVec.Z    // ROS.Z �� Unreal.Z
    );
}
