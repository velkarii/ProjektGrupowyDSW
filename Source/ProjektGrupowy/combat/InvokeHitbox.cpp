// Fill out your copyright notice in the Description page of Project Settings.

#include "combat/InvokeHitbox.h"
#include "HitboxComponent.h"

void UInvokeHitbox::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp) return;

	UWorld* World = MeshComp->GetWorld();

	if (World && World->IsGameWorld())
	{
		AActor* Owner = MeshComp->GetOwner();
		if (!Owner) return;
		
		UHitboxComponent* HitDetectionComp = Cast<UHitboxComponent>(Owner->GetComponentByClass(UHitboxComponent::StaticClass()));

		if (HitDetectionComp)
		{
			HitDetectionComp->StartDetection(MeshComp, Owner, AttackType);
		}
	}
}

void UInvokeHitbox::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp) return;

	UWorld* World = MeshComp->GetWorld();
	if (World && World->IsGameWorld())
	{
		AActor* Owner = MeshComp->GetOwner();
		if (!Owner) return;

		UHitboxComponent* HitDetectionComp = Cast<UHitboxComponent>(Owner->GetComponentByClass(UHitboxComponent::StaticClass()));

		if (HitDetectionComp)
		{
			HitDetectionComp->StopDetection();
		}
	}
}
