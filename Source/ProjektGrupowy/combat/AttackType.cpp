// Fill out your copyright notice in the Description page of Project Settings.


#include "combat/AttackType.h"

void UAttackType::InitializeAttack(USkeletalMeshComponent* InMeshComp, AActor* InOwner, TSet<TObjectPtr<AActor>>* InHitActors)
{
	MeshComponent = InMeshComp;
	Owner = InOwner;
	HitActors = InHitActors;
}