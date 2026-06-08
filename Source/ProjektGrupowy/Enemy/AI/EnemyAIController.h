// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class PROJEKTGRUPOWY_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	UBehaviorTree* BehaviorTree;

private:
	UPROPERTY()
	UBlackboardComponent* BBComp;

	FTimerHandle TimerHandle;

	const FName AttackTargetKey = TEXT("Player");

private:
	void SetAttackTargetDelayed();

protected:
	virtual void OnPossess(APawn* InPawn) override;
};
