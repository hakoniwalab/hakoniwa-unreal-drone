#include "DronePropellerComponent.h"
//#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

UDronePropellerComponent::UDronePropellerComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UDronePropellerComponent::BeginPlay()
{
    Super::BeginPlay();
#if 0
    if (bEnableAudio && AudioCue)
    {
        AudioComponent = NewObject<UAudioComponent>(this);
        AudioComponent->bAutoActivate = false;
        AudioComponent->SetSound(AudioCue);
        AudioComponent->RegisterComponentWithWorld(GetWorld());
        AudioComponent->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
    }
#endif
    TArray<USceneComponent*> Components;
    GetOwner()->GetComponents<USceneComponent>(Components);

    UE_LOG(LogTemp, Log, TEXT("Drone Propeller Component: BeginPlay()"));
    for (USceneComponent* Comp : Components)
    {
        FString Name = Comp->GetName();
        UE_LOG(LogTemp, Log, TEXT("Found Component: %s"), *Name);  // ← ログ出力

        if (Name == TEXT("Propeller1")) { Propeller1 = Comp; UE_LOG(LogTemp, Log, TEXT("Propeller1 matched")); }
        else if (Name == TEXT("Propeller2")) { Propeller2 = Comp; UE_LOG(LogTemp, Log, TEXT("Propeller2 matched")); }
        else if (Name == TEXT("Propeller3")) { Propeller3 = Comp; UE_LOG(LogTemp, Log, TEXT("Propeller3 matched")); }
        else if (Name == TEXT("Propeller4")) { Propeller4 = Comp; UE_LOG(LogTemp, Log, TEXT("Propeller4 matched")); }
        else if (Name == TEXT("Propeller5")) { Propeller5 = Comp; UE_LOG(LogTemp, Log, TEXT("Propeller5 matched")); }
        else if (Name == TEXT("Propeller6")) { Propeller6 = Comp; UE_LOG(LogTemp, Log, TEXT("Propeller6 matched")); }
    }

}

void UDronePropellerComponent::RotatePropeller(USceneComponent* Propeller, float DutyRate)
{
    if (!Propeller) return;

    float RotationSpeed = MaxRotationSpeed * DutyRate;
    //UE_LOG(LogTemp, Log, TEXT("Rotating %s: DutyRate = %f, Speed = %f"), *Propeller->GetName(), DutyRate, RotationSpeed);
    FRotator RotationDelta(0.f, RotationSpeed * GetWorld()->GetDeltaSeconds(), 0.f);

    Propeller->AddLocalRotation(RotationDelta);
}

void UDronePropellerComponent::PlayAudio(float ControlValue)
{
#if 0
    if (!AudioComponent ||  !TargetCamera ) return;

    float Distance = FVector::Dist(TargetCamera->GetActorLocation(), GetOwner()->GetActorLocation());
    float Volume = 1.0f - FMath::Clamp((Distance - MinDistance) / (MaxDistance - MinDistance), 0.0f, 1.0f);

    if (!AudioComponent->IsPlaying() && ControlValue > 0)
    {
        AudioComponent->Play();
    }
    else if (AudioComponent->IsPlaying() && ControlValue == 0)
    {
        AudioComponent->Stop();
    }

    if (AudioComponent->IsPlaying())
    {
        AudioComponent->SetVolumeMultiplier(Volume);
    }
#endif
}

void UDronePropellerComponent::Rotate(float c1, float c2, float c3, float c4)
{
    //UE_LOG(LogTemp, Warning, TEXT("Rotate called: c1=%.2f, c2=%.2f, c3=%.2f, c4=%.2f"), c1, c2, c3, c4);
    RotatePropeller(Propeller1, c1);
    RotatePropeller(Propeller2, -c2);
    if (Propeller3) RotatePropeller(Propeller3, c3);
    if (Propeller4) RotatePropeller(Propeller4, -c4);
    if (Propeller5) RotatePropeller(Propeller5, c1);
    if (Propeller6) RotatePropeller(Propeller6, c2);

    if (bEnableAudio)
    {
        PlayAudio(c1);  // 代表値としてc1
    }
}
