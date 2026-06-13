// Fill out your copyright notice in the Description page of Project Settings.


#include "combat/WeaponAttack.h"
#include "Player/PlayerCharacter.h"
#include "Enemy/Enemy.h"
#include "Kismet/GameplayStatics.h"

UWeaponAttack::UWeaponAttack()
{
	HitboxRadius = 12.f; // Domyślna wartość
}

void UWeaponAttack::SetupWeapons()
{
	if (!Owner) return;

	Owner->GetAttachedActors(Weapons);
	
	// Sprawdź czy cokolwiek jest podpięte
	if (Weapons.IsEmpty())
	{
		// UE_LOG(LogTemp, Error, TEXT("[DEBUG_LOG] WeaponAttack: Owner has NO attached actors!"));
		return;
	}

	for (int32 i = Weapons.Num() - 1; i >= 0; i--)
	{
		if (!Weapons[i]->ActorHasTag(TEXT("Weapon")))
		{
			// UE_LOG(LogTemp, Warning, TEXT("[DEBUG_LOG] WeaponAttack: Ignoring actor %s - missing 'Weapon' tag"), *Weapons[i]->GetName());
			Weapons.RemoveAt(i);
		}
	}

	if (Weapons.IsEmpty())
	{
		// UE_LOG(LogTemp, Error, TEXT("[DEBUG_LOG] WeaponAttack: No actors with 'Weapon' tag found!"));
		return;
	}

	for (auto Weapon : Weapons)
	{
		UStaticMeshComponent* Mesh = Weapon->FindComponentByClass<UStaticMeshComponent>();
		if (Mesh)
		{
			WeaponsMeshes.Add(Mesh);
			// UE_LOG(LogTemp, Log, TEXT("[DEBUG_LOG] WeaponAttack: Found mesh for weapon %s"), *Weapon->GetName());
		}
		else
		{
			// UE_LOG(LogTemp, Warning, TEXT("[DEBUG_LOG] WeaponAttack: Weapon %s has NO StaticMeshComponent!"), *Weapon->GetName());
		}
	}

	for (auto WeaponMesh : WeaponsMeshes)
	{
		TArray<FName> Sockets = WeaponMesh->GetAllSocketNames();
		WeaponsSockets.Add(Sockets);
		// UE_LOG(LogTemp, Log, TEXT("[DEBUG_LOG] WeaponAttack: Weapon mesh has %d sockets"), Sockets.Num());
	}
}

void UWeaponAttack::PerformSweep()
{
	if (Weapons.IsEmpty()) return;

	for (int i = 0; i < Weapons.Num(); i++)
	{
		if (i >= WeaponsSockets.Num()) continue;

		for (int j = 0; j < WeaponsSockets[i].Num(); j += 2)
		{
			if (j + 1 >= WeaponsSockets[i].Num()) break;

			FName StartSocket = WeaponsSockets[i][j];
			FName EndSocket = WeaponsSockets[i][j + 1];

			FVector StartLocation = WeaponsMeshes[i]->GetSocketLocation(StartSocket);
			FVector EndLocation = WeaponsMeshes[i]->GetSocketLocation(EndSocket);

			FVector Diff = StartLocation - EndLocation;

			float HalfHeight = Diff.Size() / 2;
			FQuat Rotation = FRotationMatrix::MakeFromZ(Diff).ToQuat();
			FCollisionShape CollisionShape = FCollisionShape::MakeCapsule(HitboxRadius, HalfHeight);

			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(Owner);
			TArray<FHitResult> HitResults;
			
			// Używamy ECC_WorldDynamic lub Visibility jako fallback, jeśli ECC_Pawn nie działa
			bool bHit = GetWorld()->SweepMultiByChannel(HitResults, StartLocation, EndLocation, Rotation, ECC_Pawn, CollisionShape, QueryParams);
			
			if (bHit)
			{
				for (auto Hit : HitResults)
				{
					AActor* HitActor = Hit.GetActor();
					if (!HitActor || HitActors->Contains(HitActor)) continue;

					// UE_LOG(LogTemp, Log, TEXT("[DEBUG_LOG] WeaponAttack: Detected hit on %s"), *HitActor->GetName());

					APlayerCharacter* Character = Cast<APlayerCharacter>(HitActor);
					AEnemy* Enemy = Cast<AEnemy>(HitActor);

					if (Character)
					{
						HitActors->Add(HitActor);
						float Damage = 10.f;
						if (AEnemy* Attacker = Cast<AEnemy>(Owner)) Damage = Attacker->AttackDamage;
						UGameplayStatics::ApplyDamage(Character, Damage, Owner->GetInstigatorController(), Owner, UDamageType::StaticClass());
					}
					else if (Enemy)
					{
						HitActors->Add(HitActor);
						float Damage = BaseDamage;
						if (AEnemy* Attacker = Cast<AEnemy>(Owner)) Damage = Attacker->AttackDamage;
						UGameplayStatics::ApplyDamage(Enemy, Damage, Owner->GetInstigatorController(), Owner, UDamageType::StaticClass());
						// UE_LOG(LogTemp, Warning, TEXT("[DEBUG_LOG] WeaponAttack: Applied %f damage to %s"), Damage, *HitActor->GetName());
					}
				}
			}

			// DrawDebugCapsule(GetWorld(), (StartLocation + EndLocation) / 2, HalfHeight, HitboxRadius, Rotation, FColor::Red, false, 0);
		}
	}
}

void UWeaponAttack::ExecuteAttack()
{
	if (Weapons.IsEmpty())
		SetupWeapons();

	PerformSweep();
}