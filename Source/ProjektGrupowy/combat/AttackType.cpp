// Fill out your copyright notice in the Description page of Project Settings.


#include "combat/AttackType.h"
#include "GameFramework/Character.h"
#include "NiagaraFunctionLibrary.h"

void UAttackType::InitializeAttack(USkeletalMeshComponent* InMeshComp, AActor* InOwner, TSet<TObjectPtr<AActor>>* InHitActors)
{
	MeshComponent = InMeshComp;
	Owner = InOwner;
	HitActors = InHitActors;

	// Odtwórz montaż ataku jeśli jest przypisany
	if (AttackMontage && Owner)
	{
		if (ACharacter* Character = Cast<ACharacter>(Owner))
		{
			Character->PlayAnimMontage(AttackMontage);
		}
	}

	// Zespawnuj efekt Niagara jeśli jest przypisany
	if (StartVFX && Owner)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(Owner->GetWorld(), StartVFX, Owner->GetActorLocation(), Owner->GetActorRotation());
	}
}