// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/AI/tasks/Attack.h"
#include "AIController.h"
#include "Enemy/Enemy.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"

EBTNodeResult::Type UAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	APawn* ControlledPawn = OwnerComp.GetAIOwner()->GetPawn();

	if (!ControlledPawn)
		return EBTNodeResult::Failed;

	AEnemy* Enemy = Cast<AEnemy>(ControlledPawn);
	UAnimInstance* AnimInstance = Enemy->GetMesh()->GetAnimInstance();

	if (!AnimInstance || !Enemy)
		return EBTNodeResult::Failed;

	UAnimMontage* MontageToPlay = AttackMontage ? AttackMontage : Enemy->AttackMontage;

	if (!MontageToPlay)
		return EBTNodeResult::Failed;
	
	if (AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage);
	}
	else
	{
		Enemy->PlayAttackMontage();
	}

	FOnMontageEnded MontageEndedDelegate;
	MontageEndedDelegate.BindUObject(this, &UAttack::OnAttackMontageEnded, &OwnerComp);
	AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);

	return EBTNodeResult::InProgress;
}

void UAttack::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted, UBehaviorTreeComponent* OwnerComp)
{
	if (OwnerComp && !bInterrupted)
		FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
	else
		FinishLatentTask(*OwnerComp, EBTNodeResult::Failed);
}