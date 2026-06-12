#include "Enemy/AI/tasks/BTDecorator_Chance.h"

UBTDecorator_Chance::UBTDecorator_Chance()
{
	NodeName = "Chance Decorator";
	Chance = 50.0f;

	// We want to evaluate this each time the branch is reached
	bAllowAbortLowerPri = false;
	bAllowAbortNone = true;
	FlowAbortMode = EBTFlowAbortMode::None;
}

bool UBTDecorator_Chance::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const float RandomValue = FMath::FRandRange(0.0f, 100.0f);
	return RandomValue <= Chance;
}

FString UBTDecorator_Chance::GetStaticDescription() const
{
	return FString::Printf(TEXT("Chance: %.1f%%"), Chance);
}
