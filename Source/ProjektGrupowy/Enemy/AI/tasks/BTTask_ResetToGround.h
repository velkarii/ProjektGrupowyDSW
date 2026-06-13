// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ResetToGround.generated.h"

/**
 * Behavior Tree task that resets the enemy position to the ground,
 * restores gravity and sets movement mode to Walking.
 * Useful after flying or jumping attacks.
 */
UCLASS()
class PROJEKTGRUPOWY_API UBTTask_ResetToGround : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ResetToGround();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	/** How far to check for ground. */
	UPROPERTY(EditAnywhere, Category = "Reset")
	float TraceDistance = 2000.f;

	/** Offset above ground to place the actor (to prevent sticking). */
	UPROPERTY(EditAnywhere, Category = "Reset")
	float GroundOffset = 2.0f;
};
