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

	if (AttackType)
	{
		CurrentAttack = NewObject<UAttackType>(this, AttackType);
		CurrentAttack->InitializeAttack(MeshComp, Owner, &HitActors);

		SetComponentTickEnabled(true);
	}
}

void UHitboxComponent::StopDetection()
{
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
	}
}

