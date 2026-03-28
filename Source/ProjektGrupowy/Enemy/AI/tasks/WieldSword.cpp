// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/AI/tasks/WieldSword.h"
#include "Enemy/Enemy.h"
#include "AIController.h"

EBTNodeResult::Type UWieldSword::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	TObjectPtr<AEnemy> Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Enemy)
		return EBTNodeResult::Failed;

	Enemy->WieldSword();
	return EBTNodeResult::Succeeded;
}
