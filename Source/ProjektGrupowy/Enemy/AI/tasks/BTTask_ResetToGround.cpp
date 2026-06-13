// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/AI/tasks/BTTask_ResetToGround.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"

UBTTask_ResetToGround::UBTTask_ResetToGround()
{
	NodeName = "Reset To Ground";
}

EBTNodeResult::Type UBTTask_ResetToGround::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* MyController = OwnerComp.GetAIOwner();
	if (!MyController) return EBTNodeResult::Failed;

	ACharacter* Character = Cast<ACharacter>(MyController->GetPawn());
	if (!Character) return EBTNodeResult::Failed;

	// Snap to ground
	FHitResult GroundHit;
	FVector Start = Character->GetActorLocation() + FVector(0, 0, 100.f);
	FVector End = Start - FVector(0, 0, TraceDistance);
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(Character);

	if (GetWorld()->LineTraceSingleByChannel(GroundHit, Start, End, ECC_Visibility, TraceParams))
	{
		float CapsuleHalfHeight = Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		Character->SetActorLocation(GroundHit.Location + FVector(0, 0, CapsuleHalfHeight + GroundOffset), false);
		
		// Reset rotation - ensure Pitch and Roll are zero, keeping only Yaw
		FRotator CurrentRotation = Character->GetActorRotation();
		FRotator NewRotation = FRotator(0.f, CurrentRotation.Yaw, 0.f);
		Character->SetActorRotation(NewRotation);
		
		// UE_LOG(LogTemp, Log, TEXT("[DEBUG_LOG] ResetToGround: Actor %s snapped to ground and rotation reset"), *Character->GetName());
		
		UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement();
		if (MoveComp)
		{
			MoveComp->GravityScale = 1.f;
			MoveComp->Velocity = FVector::ZeroVector;
			MoveComp->SetMovementMode(MOVE_Walking);
		}

		return EBTNodeResult::Succeeded;
	}

	// UE_LOG(LogTemp, Warning, TEXT("[DEBUG_LOG] ResetToGround: FAILED to find ground for %s"), *Character->GetName());
	return EBTNodeResult::Failed;
}
