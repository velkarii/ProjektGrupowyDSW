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

	// 1. Wykonaj logikę ataku (jeśli klasa ataku jest ustawiona)
	if (Enemy->SpecialAttackClass)
	{
		// Instead of creating a local instance and calling Execute once,
		// we should rely on HitboxComponent which supports ticking if InvokeHitbox notify is used in animation.
		// However, if we want it to be triggered by BT task directly and support ticking, 
		// we'd need a more complex setup.
		// Usually, Special Attacks are triggered via Anim Montages containing Anim Notifies.
		
		// If the user expects it to work from BTTask, we'll keep the logic but 
		// if they only see one block, it's likely because they aren't using the Notify 
		// OR the HitboxComponent is not ticking this specific instance.
		
		// If we want it to be "one-shot" from BT, we should at least use the HitboxComponent
		// so it can tick if the animation is playing.
		
		UHitboxComponent* HitboxComp = Cast<UHitboxComponent>(Enemy->GetComponentByClass(UHitboxComponent::StaticClass()));
		if (HitboxComp)
		{
			HitboxComp->StartDetection(Enemy->GetMesh(), Enemy, Enemy->SpecialAttackClass);
		}
	}

	// 2. Obsłuż animację
	UAnimMontage* MontageToPlay = SpecialAttackMontage ? SpecialAttackMontage : Enemy->AttackMontage;
	
	if (MontageToPlay)
	{
		Enemy->PlayAttackMontage(); // Ta metoda w Enemy odtwarza AttackMontage, jeśli chcemy konkretny to musimy zmodyfikować Enemy lub odtworzyć tutaj
		
		// Jeśli PlayAttackMontage odtwarza tylko domyślny, a my mamy SpecialAttackMontage:
		if (SpecialAttackMontage != nullptr && SpecialAttackMontage != Enemy->AttackMontage)
		{
			Enemy->GetMesh()->GetAnimInstance()->Montage_Play(SpecialAttackMontage);
		}

		if (bWaitForAnimation)
		{
			UAnimInstance* AnimInstance = Enemy->GetMesh()->GetAnimInstance();
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
