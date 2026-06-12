#include "Enemy/AI/tasks/BTTask_TeleportBehindPlayer.h"
#include "AIController.h"
#include "Enemy/Enemy.h"
#include "Player/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"

UBTTask_TeleportToPlayer::UBTTask_TeleportToPlayer()
{
	NodeName = "Teleport To Player";
}

EBTNodeResult::Type UBTTask_TeleportToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* MyController = OwnerComp.GetAIOwner();
	if (!MyController) return EBTNodeResult::Failed;

	AEnemy* Enemy = Cast<AEnemy>(MyController->GetPawn());
	if (!Enemy) return EBTNodeResult::Failed;

	// Teleportacja jest teraz wywoływana przez kod, ale w tym projekcie 
	// TeleportToPlayer w ExecuteTask historycznie nic nie robił poza odpaleniem animacji,
	// a sama teleportacja działała w AnimNotify. 
	// Jeśli chcemy by BTTask sam teleportował bez animacji:
	
	APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(Enemy->GetWorld(), 0));
	if (Player)
	{
		// Domyślne wartości jeśli nie ma parametrów w BTTask
		Enemy->TeleportToTarget(Player, 150.f, true);
	}

	return EBTNodeResult::Succeeded;
}
