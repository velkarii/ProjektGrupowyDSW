// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/AI/tasks/ClearFocus.h"
#include "AIController.h"

EBTNodeResult::Type UClearFocus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	TObjectPtr<AAIController> AIController = OwnerComp.GetAIOwner();

	if(!AIController)
		return EBTNodeResult::Failed;

	AIController->ClearFocus(EAIFocusPriority::LastFocusPriority);
	return EBTNodeResult::Succeeded;
}