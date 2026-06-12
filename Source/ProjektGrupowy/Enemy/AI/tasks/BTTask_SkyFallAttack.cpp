// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/AI/tasks/BTTask_SkyFallAttack.h"
#include "AIController.h"
#include "Enemy/Enemy.h"
#include "combat/AttackType.h"
#include "combat/HitboxComponent.h"
#include "Animation/AnimInstance.h"

UBTTask_SkyFallAttack::UBTTask_SkyFallAttack()
{
	NodeName = "Sky Fall Attack";
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_SkyFallAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* MyController = OwnerComp.GetAIOwner();
	if (!MyController) return EBTNodeResult::Failed;

	AEnemy* Enemy = Cast<AEnemy>(MyController->GetPawn());
	if (!Enemy) return EBTNodeResult::Failed;

	// 1. Określ klasę ataku
	TSubclassOf<UAttackType> AttackClass = SkyFallAttackClass ? SkyFallAttackClass : Enemy->SpecialAttackClass;

	if (AttackClass)
	{
		UHitboxComponent* HitboxComp = Cast<UHitboxComponent>(Enemy->GetComponentByClass(UHitboxComponent::StaticClass()));
		if (HitboxComp)
		{
			HitboxComp->StartDetection(Enemy->GetMesh(), Enemy, AttackClass);
		}
	}

	return EBTNodeResult::InProgress;
}

void UBTTask_SkyFallAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* MyController = OwnerComp.GetAIOwner();
	if (!MyController)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	AEnemy* Enemy = Cast<AEnemy>(MyController->GetPawn());
	if (!Enemy)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	UHitboxComponent* HitboxComp = Cast<UHitboxComponent>(Enemy->GetComponentByClass(UHitboxComponent::StaticClass()));
	if (HitboxComp)
	{
		UAttackType* CurrentAttack = HitboxComp->GetCurrentAttack();
		// Jeśli atak się skończył lub w ogóle go nie ma (a powinien być)
		if (!CurrentAttack || CurrentAttack->IsFinished())
		{
			UE_LOG(LogTemp, Log, TEXT("[DEBUG_LOG] BTTask_SkyFallAttack: Attack finished, finishing task."));
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}
	else
	{
		// Jeśli nie ma komponentu hitboxa, kończymy (brak ataku do monitorowania)
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
