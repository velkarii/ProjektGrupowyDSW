// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AttackType.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class PROJEKTGRUPOWY_API UAttackType : public UObject
{
	GENERATED_BODY()

public:
	virtual void InitializeAttack(USkeletalMeshComponent* InMeshComp, AActor* InOwner, TSet<TObjectPtr<AActor>>* InHitActors);

	virtual void ExecuteAttack() PURE_VIRTUAL(UAttackTypeBase::ExecuteAttack);

	USkeletalMeshComponent* MeshComponent;

	UPROPERTY()
	AActor* Owner;

	TSet<TObjectPtr<AActor>>* HitActors;
};
