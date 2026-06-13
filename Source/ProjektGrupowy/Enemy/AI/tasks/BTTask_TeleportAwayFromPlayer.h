// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "NiagaraSystem.h"
#include "BTTask_TeleportAwayFromPlayer.generated.h"

/**
 * Zadanie Behavior Tree powodujące teleportację bossa z dala od gracza.
 */
UCLASS()
class PROJEKTGRUPOWY_API UBTTask_TeleportAwayFromPlayer : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_TeleportAwayFromPlayer();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	/** Minimalny dystans od gracza po teleportacji. */
	UPROPERTY(EditAnywhere, Category = "Teleport")
	float MinDistance = 600.f;

	/** Maksymalny dystans od gracza po teleportacji. */
	UPROPERTY(EditAnywhere, Category = "Teleport")
	float MaxDistance = 1000.f;

	/** Czy obrócić się w stronę gracza po teleportacji? */
	UPROPERTY(EditAnywhere, Category = "Teleport")
	bool bFacePlayer = true;

	/** Opóźnienie teleportacji po zespawnowaniu efektu. */
	UPROPERTY(EditAnywhere, Category = "Teleport")
	float TeleportDelay = 1.0f;

	/** Efekt Niagara spawnowany przed teleportacją. */
	UPROPERTY(EditAnywhere, Category = "Teleport")
	class UNiagaraSystem* TeleportVFX;
};
