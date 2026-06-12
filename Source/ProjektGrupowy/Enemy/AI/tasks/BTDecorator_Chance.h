#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_Chance.generated.h"

/**
 * Decorator that allows a branch to execute based on a percentage chance.
 */
UCLASS()
class PROJEKTGRUPOWY_API UBTDecorator_Chance : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_Chance();

	/** Percentage chance (0-100) for this node to pass. */
	UPROPERTY(EditAnywhere, Category = "Condition", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float Chance;

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	virtual FString GetStaticDescription() const override;
};
