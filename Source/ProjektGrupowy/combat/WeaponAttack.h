// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "combat/AttackType.h"
#include "WeaponAttack.generated.h"

/**
 * 
 */
UCLASS()
class PROJEKTGRUPOWY_API UWeaponAttack : public UAttackType
{
	GENERATED_BODY()
	
public:
	UWeaponAttack();
	void SetupWeapons();
	void PerformSweep();
	void ExecuteAttack() override;

protected:
	/** Promień hitboksa broni (kapsuły). Można edytować w Blueprincie. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Settings")
	float HitboxRadius;
	
private:
	TArray<AActor*> Weapons;
	TArray<UStaticMeshComponent*> WeaponsMeshes;
	TArray<TArray<FName>> WeaponsSockets;
	
};
