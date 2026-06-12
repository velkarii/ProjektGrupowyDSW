// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_TeleportBehindPlayer.generated.h"

/**
 * Zadanie Behavior Tree powodujące teleportację bossa do gracza.
 */
UCLASS()
class PROJEKTGRUPOWY_API UBTTask_TeleportToPlayer : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_TeleportToPlayer();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
