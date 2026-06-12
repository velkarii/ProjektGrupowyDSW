#include "combat/HitboxComponent.h"
#include "Player/PlayerCharacter.h"
#include "Enemy/Enemy.h"

UHitboxComponent::UHitboxComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UHitboxComponent::StartDetection(USkeletalMeshComponent* MeshComp, AActor* Owner, TSubclassOf<UAttackType> AttackType)
{
	HitActors.Empty();
	bNotifyActive = true;

	if (AttackType)
	{
		CurrentAttack = NewObject<UAttackType>(this, AttackType);
		CurrentAttack->InitializeAttack(MeshComp, Owner, &HitActors);

		SetComponentTickEnabled(true);
	}
}

void UHitboxComponent::StopDetection()
{
	bNotifyActive = false;
	
	// Atak może trwać dłużej niż sam Notify (np. SkyFallAttack).
	// TickComponent zajmie się wyłączeniem ticka, gdy CurrentAttack->IsFinished() zwróci true.
	if (CurrentAttack && !CurrentAttack->IsFinished())
	{
		UE_LOG(LogTemp, Log, TEXT("[DEBUG_LOG] HitboxComponent: NotifyEnd, but attack continues..."));
		return;
	}

	SetComponentTickEnabled(false);

	int i = 0;
	for (auto HitActor : HitActors)
	{
		if (!HitActor)
			continue;

		GEngine->AddOnScreenDebugMessage(i++, 5.f, FColor::Red, FString::Printf(TEXT("Hit Actor: %s"), *HitActor->GetClass()->GetFName().ToString()));
	}
}

void UHitboxComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UHitboxComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CurrentAttack)
	{
		CurrentAttack->ExecuteAttack();

		// If the notify has ended and the attack sequence is also finished, stop ticking
		if (!bNotifyActive && CurrentAttack->IsFinished())
		{
			SetComponentTickEnabled(false);
			CurrentAttack = nullptr;
		}
	}
	else if (!bNotifyActive)
	{
		SetComponentTickEnabled(false);
	}
}

