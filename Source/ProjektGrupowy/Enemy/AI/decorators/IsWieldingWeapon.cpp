// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/AI/decorators/IsWieldingWeapon.h"
#include "AIController.h"
#include "Enemy/Enemy.h"

bool UIsWieldingWeapon::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();
	if(!Pawn)
		return false;

	AEnemy* Enemy = Cast<AEnemy>(Pawn);
	if(!Enemy)
		return false;
	
	return Enemy->IsWieldingSword;
}
