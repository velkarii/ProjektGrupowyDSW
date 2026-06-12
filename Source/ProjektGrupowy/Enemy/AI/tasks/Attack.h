// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Attack.generated.h"

/**
 * 
 */
UCLASS()
class PROJEKTGRUPOWY_API UAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:

	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:

	/** Opcjonalny montaż animacji do odtworzenia. Jeśli nie ustawiony, użyje domyślnego z przeciwnika. */
	UPROPERTY(EditAnywhere, Category = "Attack")
	class UAnimMontage* AttackMontage;

private:

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted, UBehaviorTreeComponent* OwnerComp);
};
