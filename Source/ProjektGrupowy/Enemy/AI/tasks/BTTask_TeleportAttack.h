#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_TeleportAttack.generated.h"

/**
 * Task that teleports the boss to the player and plays a special attack animation.
 */
UCLASS()
class PROJEKTGRUPOWY_API UBTTask_TeleportAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_TeleportAttack();

	/** Montage to play. Teleportation should be handled via AnimNotify within this montage. */
	UPROPERTY(EditAnywhere, Category = "Attack")
	class UAnimMontage* TeleportAttackMontage;

	/** Optional Niagara VFX to spawn at the start of the task. */
	UPROPERTY(EditAnywhere, Category = "VFX")
	class UNiagaraSystem* StartVFX;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted, UBehaviorTreeComponent* OwnerComp);
};
