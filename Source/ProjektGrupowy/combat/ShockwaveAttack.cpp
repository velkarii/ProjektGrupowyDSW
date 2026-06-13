// Fill out your copyright notice in the Description page of Project Settings.

#include "combat/ShockwaveAttack.h"
#include "combat/HitboxComponent.h"
#include "Enemy/Enemy.h"
#include "Player/PlayerCharacter.h"
#include "Engine/World.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/OverlapResult.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"

void UShockwaveAttack::InitializeAttack(USkeletalMeshComponent* InMeshComp, AActor* InOwner, TSet<TObjectPtr<AActor>>* InHitActors)
{
	Super::InitializeAttack(InMeshComp, InOwner, InHitActors);
	if (Owner)
	{
		ForwardDirection = Owner->GetActorForwardVector();
		
		FVector OwnerLoc = Owner->GetActorLocation();
		float HalfHeight = Owner->GetSimpleCollisionHalfHeight();
		// Set InitialBaseLocation to the floor level directly under the owner
		InitialBaseLocation = FVector(OwnerLoc.X, OwnerLoc.Y, OwnerLoc.Z - HalfHeight);
		LastBlockLocation = InitialBaseLocation;
		
		CurrentTime = 0.f;
		WarningStartTime = 0.f;
		LastSpawnedBlockIndex = -1;
		HitActors.Empty();

		if (PreAttackDelay > 0.f)
		{
			CurrentPhase = EShockwavePhase::Warning;
			if (PreAttackVFX)
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), PreAttackVFX, InitialBaseLocation, ForwardDirection.Rotation());
			}
		}
		else
		{
			CurrentPhase = EShockwavePhase::Active;
		}
	}
}

bool UShockwaveAttack::IsFinished() const
{
	return CurrentPhase == EShockwavePhase::Finished;
}

void UShockwaveAttack::ExecuteAttack()
{
	if (!Owner || !GetWorld()) return;

	if (CurrentPhase == EShockwavePhase::Warning)
	{
		WarningStartTime += GetWorld()->GetDeltaSeconds();
		if (WarningStartTime >= PreAttackDelay)
		{
			CurrentPhase = EShockwavePhase::Active;
			
			// Refresh direction and location at the moment the attack becomes active
			if (Owner)
			{
				ForwardDirection = Owner->GetActorForwardVector();
				FVector OwnerLoc = Owner->GetActorLocation();
				float HalfHeight = Owner->GetSimpleCollisionHalfHeight();
				InitialBaseLocation = FVector(OwnerLoc.X, OwnerLoc.Y, OwnerLoc.Z - HalfHeight);
			}
		}
		else
		{
			return;
		}
	}

	if (CurrentPhase == EShockwavePhase::Active)
	{
		CurrentTime += GetWorld()->GetDeltaSeconds();
		
		int32 TargetBlockIndex = FMath::FloorToInt(CurrentTime / TimeBetweenBlocks);
		
		// If we only want one block, TargetBlockIndex should be clamped to 0
		if (NumBlocks > 0 && TargetBlockIndex >= NumBlocks)
		{
			TargetBlockIndex = NumBlocks - 1;
		}
		
		if (TargetBlockIndex > LastSpawnedBlockIndex)
		{
			for (int32 i = LastSpawnedBlockIndex + 1; i <= TargetBlockIndex; ++i)
			{
				float CapsuleRadius = 42.0f;
				if (Owner)
				{
					CapsuleRadius = Owner->GetSimpleCollisionRadius();
				}
				
				// Ensure we spawn at least at the current location or offset
				float Offset = CapsuleRadius + (i * BlockStepDistance) + BlockExtent.X;
				FVector SpawnLocation = InitialBaseLocation + (ForwardDirection * Offset);
				FVector BoxCenter = SpawnLocation + FVector(0.f, 0.f, BlockExtent.Z);
				
				FCollisionShape BoxShape = FCollisionShape::MakeBox(BlockExtent);
				FCollisionQueryParams QueryParams;
				QueryParams.AddIgnoredActor(Owner);
				
				TArray<FOverlapResult> OverlapResults;
				bool bHit = GetWorld()->OverlapMultiByChannel(
					OverlapResults,
					BoxCenter,
					ForwardDirection.ToOrientationQuat(),
					ECC_Pawn,
					BoxShape,
					QueryParams
				);

				if (bHit)
				{
					for (const FOverlapResult& Overlap : OverlapResults)
					{
						AActor* HitActor = Overlap.GetActor();
						if (HitActor && !HitActors.Contains(HitActor))
						{
							HitActors.Add(HitActor);

							AEnemy* Enemy = Cast<AEnemy>(HitActor);
							APlayerCharacter* Player = Cast<APlayerCharacter>(HitActor);

							float Damage = 20.f;
							if (AEnemy* Attacker = Cast<AEnemy>(Owner))
							{
								Damage = Attacker->AttackDamage * 1.5f;
							}

							if (Enemy)
							{
								Enemy->TakeDamageAmount(Damage);
							}
							else if (Player)
							{
								UGameplayStatics::ApplyDamage(Player, Damage, Owner->GetInstigatorController(), Owner, UDamageType::StaticClass());
							}
							
							GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, FString::Printf(TEXT("Shockwave hit: %s"), *HitActor->GetName()));
						}
					}
				}

				if (ShockwaveVFX)
				{
					UNiagaraFunctionLibrary::SpawnSystemAtLocation(
						GetWorld(),
						ShockwaveVFX,
						SpawnLocation,
						ForwardDirection.Rotation(),
						VFXScale,
						true,
						true,
						ENCPoolMethod::AutoRelease,
						true
					);
				}
				
				DrawDebugBox(
					GetWorld(),
					BoxCenter,
					BlockExtent,
					ForwardDirection.ToOrientationQuat(),
					FColor::Orange,
					false,
					0.2f,
					0,
					2.0f
				);
				
				LastSpawnedBlockIndex = i; 
			}
		}

		if (LastSpawnedBlockIndex >= NumBlocks - 1)
		{
			CurrentPhase = EShockwavePhase::Finished;
		}
	}
}