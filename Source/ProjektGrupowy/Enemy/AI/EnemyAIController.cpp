// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (!BehaviorTree)
		return;
    
	BBComp = GetBlackboardComponent();

	if (!UseBlackboard(BehaviorTree->BlackboardAsset, BBComp))
		return;
    
	RunBehaviorTree(BehaviorTree);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AEnemyAIController::SetAttackTargetDelayed, 0.1f, false);
}

void AEnemyAIController::SetAttackTargetDelayed()
{
	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!Player)
		return;

	GetBlackboardComponent()->SetValueAsObject(AttackTargetKey, (UObject*)Player);
}