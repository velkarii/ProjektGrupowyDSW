#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_IsPlayerTooClose.generated.h"

/**
 * Decorator that checks if the player is within a certain distance.
 */
UCLASS()
class PROJEKTGRUPOWY_API UBTDecorator_IsPlayerTooClose : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_IsPlayerTooClose();

	/** Distance threshold to consider the player "too close". */
	UPROPERTY(EditAnywhere, Category = "Condition")
	float CloseDistance;

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	virtual FString GetStaticDescription() const override;
};
