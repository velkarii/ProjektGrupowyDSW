// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AttackType.h"
#include "HitboxComponent.h"
#include "InvokeHitbox.generated.h"

UCLASS()
class PROJEKTGRUPOWY_API UInvokeHitbox : public UAnimNotifyState
{
	GENERATED_BODY()

protected:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference);
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference);

private:
	UHitboxComponent* HitDetection;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAttackType> AttackType;
	
};
