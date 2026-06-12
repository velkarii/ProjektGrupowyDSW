#include "Enemy/AI/tasks/BTTask_TeleportAttack.h"
#include "AIController.h"
#include "Enemy/Enemy.h"
#include "Animation/AnimInstance.h"
#include "NiagaraFunctionLibrary.h"

UBTTask_TeleportAttack::UBTTask_TeleportAttack()
{
	NodeName = "Teleport Attack";
}

EBTNodeResult::Type UBTTask_TeleportAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AEnemy* Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Enemy)
	{
		return EBTNodeResult::Failed;
	}

	// 0. Spawn Start VFX if assigned
	if (StartVFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(Enemy->GetWorld(), StartVFX, Enemy->GetActorLocation(), Enemy->GetActorRotation());
	}

	// 1. Play Special Montage (Teleportation logic should be in AnimNotify within the montage)
	if (TeleportAttackMontage)
	{
		UAnimInstance* AnimInstance = Enemy->GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			float Duration = AnimInstance->Montage_Play(TeleportAttackMontage);
			if (Duration > 0.0f)
			{
				FOnMontageEnded MontageEndedDelegate;
				MontageEndedDelegate.BindUObject(this, &UBTTask_TeleportAttack::OnAttackMontageEnded, &OwnerComp);
				AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, TeleportAttackMontage);
				
				return EBTNodeResult::InProgress;
			}
		}
	}

	return EBTNodeResult::Succeeded;
}

void UBTTask_TeleportAttack::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted, UBehaviorTreeComponent* OwnerComp)
{
	if (OwnerComp)
	{
		FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
	}
}
