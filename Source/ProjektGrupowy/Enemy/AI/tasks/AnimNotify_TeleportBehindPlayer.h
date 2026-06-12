// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_TeleportBehindPlayer.generated.h"

/**
 * Notify wywołujący teleportację za plecy gracza.
 */
UCLASS()
class PROJEKTGRUPOWY_API UAnimNotify_TeleportToPlayer : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	/** Dystans przed graczem. */
	UPROPERTY(EditAnywhere, Category = "Teleport")
	float Distance = 150.f;

	/** Czy obrócić się w stronę gracza? */
	UPROPERTY(EditAnywhere, Category = "Teleport")
	bool bFacePlayer = true;
};
