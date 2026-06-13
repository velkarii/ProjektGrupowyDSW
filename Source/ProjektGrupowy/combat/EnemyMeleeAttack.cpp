// Fill out your copyright notice in the Description page of Project Settings.

#include "combat/EnemyMeleeAttack.h"
#include "Enemy/Enemy.h"
#include "Player/PlayerCharacter.h"
#include "Engine/World.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

void UEnemyMeleeAttack::ExecuteAttack()
{
	if (Weapons.IsEmpty())
	{
		SetupWeapons();
	}

	PerformSweep();
}

void UEnemyMeleeAttack::SetupWeapons()
{
	if (!Owner) return;

	Owner->GetAttachedActors(Weapons);

	// Filtrowanie aktorów z tagiem "Weapon"
	for (int32 i = Weapons.Num() - 1; i >= 0; --i)
	{
		if (!Weapons[i]->ActorHasTag(TEXT("Weapon")))
		{
			Weapons.RemoveAt(i);
		}
	}

	for (AActor* Weapon : Weapons)
	{
		UStaticMeshComponent* MeshComp = Weapon->FindComponentByClass<UStaticMeshComponent>();
		if (MeshComp)
		{
			WeaponsMeshes.Add(MeshComp);
			WeaponsSockets.Add(MeshComp->GetAllSocketNames());
		}
	}
}

void UEnemyMeleeAttack::PerformSweep()
{
	if (!Owner || !GetWorld()) return;

	float Damage = 10.f;
	if (AEnemy* EnemyOwner = Cast<AEnemy>(Owner))
	{
		Damage = EnemyOwner->AttackDamage;
	}

	for (int32 i = 0; i < WeaponsMeshes.Num(); i++)
	{
		UStaticMeshComponent* MeshComp = WeaponsMeshes[i];
		const TArray<FName>& Sockets = WeaponsSockets[i];

		for (int32 j = 0; j + 1 < Sockets.Num(); j += 2)
		{
			FVector Start = MeshComp->GetSocketLocation(Sockets[j]);
			FVector End = MeshComp->GetSocketLocation(Sockets[j + 1]);

			FVector Diff = Start - End;
			float HalfHeight = Diff.Size() / 2.0f;
			FQuat Rotation = FRotationMatrix::MakeFromZ(Diff).ToQuat();
			FCollisionShape Shape = FCollisionShape::MakeCapsule(12.f, HalfHeight);

			FCollisionQueryParams Params;
			Params.AddIgnoredActor(Owner);
			for (AActor* Weapon : Weapons) Params.AddIgnoredActor(Weapon);

			TArray<FHitResult> OutHits;
			GetWorld()->SweepMultiByChannel(OutHits, Start, End, Rotation, ECC_Pawn, Shape, Params);

			for (const FHitResult& Hit : OutHits)
			{
				AActor* HitActor = Hit.GetActor();
				if (!HitActor || HitActors->Contains(HitActor)) continue;

				// Zadawanie obrażeń graczowi lub innym przeciwnikom (zależnie od potrzeb)
				APlayerCharacter* Player = Cast<APlayerCharacter>(HitActor);
				if (Player)
				{
					HitActors->Add(HitActor);
					UGameplayStatics::ApplyDamage(Player, Damage, Owner->GetInstigatorController(), Owner, UDamageType::StaticClass());
					UE_LOG(LogTemp, Warning, TEXT("Enemy hit player for %f damage"), Damage);
				}
				
				AEnemy* OtherEnemy = Cast<AEnemy>(HitActor);
				if (OtherEnemy && OtherEnemy != Owner)
				{
					HitActors->Add(HitActor);
					OtherEnemy->TakeDamageAmount(Damage);
				}
			}

			// Debug
			// DrawDebugCapsule(GetWorld(), (Start + End) * 0.5f, HalfHeight, 12.f, Rotation, FColor::Yellow, false, 0.1f);
		}
	}
}
