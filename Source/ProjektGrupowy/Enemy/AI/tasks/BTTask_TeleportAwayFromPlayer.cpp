// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/AI/tasks/BTTask_TeleportAwayFromPlayer.h"
#include "AIController.h"
#include "Enemy/Enemy.h"
#include "Player/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"
#include "NiagaraFunctionLibrary.h"

UBTTask_TeleportAwayFromPlayer::UBTTask_TeleportAwayFromPlayer()
{
	NodeName = "Teleport Away From Player";
}

EBTNodeResult::Type UBTTask_TeleportAwayFromPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* MyController = OwnerComp.GetAIOwner();
	if (!MyController) return EBTNodeResult::Failed;

	AEnemy* Enemy = Cast<AEnemy>(MyController->GetPawn());
	if (!Enemy) return EBTNodeResult::Failed;

	APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(Enemy->GetWorld(), 0));
	if (Player)
	{
		if (TeleportVFX)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(Enemy->GetWorld(), TeleportVFX, Enemy->GetActorLocation(), Enemy->GetActorRotation());
		}
		Enemy->TeleportAwayFromTargetDelayed(Player, MinDistance, MaxDistance, bFacePlayer, TeleportDelay);
	}

	return EBTNodeResult::Succeeded;
}
