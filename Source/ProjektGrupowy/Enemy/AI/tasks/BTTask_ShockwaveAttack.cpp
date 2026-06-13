// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/AI/tasks/BTTask_ShockwaveAttack.h"
#include "AIController.h"
#include "Enemy/Enemy.h"
#include "combat/AttackType.h"
#include "combat/HitboxComponent.h"
#include "Animation/AnimInstance.h"

UBTTask_ShockwaveAttack::UBTTask_ShockwaveAttack()
{
	NodeName = "Shockwave Attack";
}

EBTNodeResult::Type UBTTask_ShockwaveAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* MyController = OwnerComp.GetAIOwner();
	if (!MyController) return EBTNodeResult::Failed;

	AEnemy* Enemy = Cast<AEnemy>(MyController->GetPawn());
	if (!Enemy) return EBTNodeResult::Failed;

	// Just play the animation. The AnimNotify (InvokeHitbox) will handle the shockwave logic.
	UAnimMontage* MontageToPlay = SpecialAttackMontage ? SpecialAttackMontage : Enemy->AttackMontage;
	
	if (MontageToPlay)
	{
		// Make sure we play the intended montage
		UAnimInstance* AnimInstance = Enemy->GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(MontageToPlay);
		}

		if (bWaitForAnimation)
		{
			if (AnimInstance)
			{
				FOnMontageEnded MontageEndedDelegate;
				MontageEndedDelegate.BindUObject(this, &UBTTask_ShockwaveAttack::OnAnimationEnded, &OwnerComp);
				AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);
				return EBTNodeResult::InProgress;
			}
		}
	}

	return EBTNodeResult::Succeeded;
}

void UBTTask_ShockwaveAttack::OnAnimationEnded(UAnimMontage* Montage, bool bInterrupted, UBehaviorTreeComponent* OwnerComp)
{
	if (OwnerComp)
	{
		FinishLatentTask(*OwnerComp, bInterrupted ? EBTNodeResult::Failed : EBTNodeResult::Succeeded);
	}
}
