#include "Enemy/AI/tasks/BTDecorator_IsPlayerTooClose.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

UBTDecorator_IsPlayerTooClose::UBTDecorator_IsPlayerTooClose()
{
	NodeName = "Is Player Too Close";
	CloseDistance = 500.0f;

	bAllowAbortLowerPri = false;
	bAllowAbortNone = true;
	FlowAbortMode = EBTFlowAbortMode::None;
}

bool UBTDecorator_IsPlayerTooClose::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	APawn* ControlledPawn = OwnerComp.GetAIOwner()->GetPawn();
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	
	if (!ControlledPawn || !PlayerController || !PlayerController->GetPawn())
	{
		return false;
	}

	AActor* PlayerActor = PlayerController->GetPawn();
	float Distance = FVector::Dist(ControlledPawn->GetActorLocation(), PlayerActor->GetActorLocation());

	return Distance <= CloseDistance;
}

FString UBTDecorator_IsPlayerTooClose::GetStaticDescription() const
{
	return FString::Printf(TEXT("Is Player closer than %.1f units?"), CloseDistance);
}
