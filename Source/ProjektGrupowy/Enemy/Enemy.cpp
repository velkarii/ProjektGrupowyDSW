#include "Enemy/Enemy.h"

AEnemy::AEnemy()
{
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemy::Tick(float DeltaTime)
{
}

void AEnemy::PlayAttackMontage()
{
	if (!AttackMontage) 
		return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance || AnimInstance->Montage_IsPlaying(AttackMontage))
		return;

	AnimInstance->Montage_Play(AttackMontage);
}

void AEnemy::WieldSword()
{
	if (BP_Weapon == nullptr) 
		return;

	AActor* Weapon = GetWorld()->SpawnActor<AActor>(BP_Weapon, GetActorLocation(), GetActorRotation());
	Weapon->Tags.Add(TEXT("Weapon"));
	Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName(TEXT("WeaponR")));
	IsWieldingSword = true;
}

