// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/AI/tasks/AnimNotify_TeleportBehindPlayer.h"
#include "Enemy/Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "Player/PlayerCharacter.h"

void UAnimNotify_TeleportToPlayer::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	AEnemy* Enemy = Cast<AEnemy>(MeshComp->GetOwner());
	if (!Enemy) return;

	APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(Enemy->GetWorld(), 0));
	if (!Player) return;

	Enemy->TeleportToTarget(Player, Distance, bFacePlayer);
}
