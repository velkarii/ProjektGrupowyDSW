// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SkyFallAttack.generated.h"

/**
 * Zadanie Behavior Tree wykonujące atak Sky Fall przez przeciwnika.
 */
UCLASS()
class PROJEKTGRUPOWY_API UBTTask_SkyFallAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_SkyFallAttack();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	/** Klasa ataku Sky Fall. Jeśli nie ustawiona w BT, zadanie spróbuje użyć SpecialAttackClass z przeciwnika. */
	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<class UAttackType> SkyFallAttackClass;
};
