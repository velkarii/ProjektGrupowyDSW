// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "WieldSword.generated.h"

/**
 * 
 */
UCLASS()
class PROJEKTGRUPOWY_API UWieldSword : public UBTTaskNode
{
	GENERATED_BODY()
	
public:

	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
