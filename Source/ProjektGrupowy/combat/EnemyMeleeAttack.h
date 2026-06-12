// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "combat/AttackType.h"
#include "EnemyMeleeAttack.generated.h"

/**
 * Zwykły atak wręcz dla przeciwnika, wykorzystujący broń (jeśli posiada).
 */
UCLASS()
class PROJEKTGRUPOWY_API UEnemyMeleeAttack : public UAttackType
{
	GENERATED_BODY()

public:
	virtual void ExecuteAttack() override;

private:
	void SetupWeapons();
	void PerformSweep();

	UPROPERTY()
	TArray<AActor*> Weapons;
	
	TArray<UStaticMeshComponent*> WeaponsMeshes;
	TArray<TArray<FName>> WeaponsSockets;
};
