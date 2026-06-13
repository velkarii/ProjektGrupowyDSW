// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "NiagaraSystem.h"
#include "AnimNotify_TeleportAwayFromPlayer.generated.h"

/**
 * Notify wywołujący teleportację z dala od gracza.
 */
UCLASS()
class PROJEKTGRUPOWY_API UAnimNotify_TeleportAwayFromPlayer : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	/** Minimalny dystans od gracza. */
	UPROPERTY(EditAnywhere, Category = "Teleport")
	float MinDistance = 600.f;

	/** Maksymalny dystans od gracza. */
	UPROPERTY(EditAnywhere, Category = "Teleport")
	float MaxDistance = 1000.f;

	/** Czy obrócić się w stronę gracza? */
	UPROPERTY(EditAnywhere, Category = "Teleport")
	bool bFacePlayer = true;

	/** Opóźnienie teleportacji po zespawnowaniu efektu. */
	UPROPERTY(EditAnywhere, Category = "Teleport")
	float TeleportDelay = 1.0f;

	/** Efekt Niagara spawnowany przed teleportacją. */
	UPROPERTY(EditAnywhere, Category = "Teleport")
	class UNiagaraSystem* TeleportVFX;
};
