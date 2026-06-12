// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ShockwaveAttack.generated.h"

/**
 * Zadanie Behavior Tree wykonujące atak Shockwave (fala uderzeniowa) przez przeciwnika.
 */
UCLASS()
class PROJEKTGRUPOWY_API UBTTask_ShockwaveAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ShockwaveAttack();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	/** Opcjonalny montaż animacji do odtworzenia podczas ataku. Jeśli nie ustawiony, zostanie użyty AttackMontage z klasy Enemy. */
	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* SpecialAttackMontage;

	/** Czy zadanie ma czekać na zakończenie animacji? */
	UPROPERTY(EditAnywhere, Category = "Attack")
	bool bWaitForAnimation = true;

private:
	UFUNCTION()
	void OnAnimationEnded(UAnimMontage* Montage, bool bInterrupted, UBehaviorTreeComponent* OwnerComp);
};
