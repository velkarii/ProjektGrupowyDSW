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
	Owner->GetAttachedActors(Weapons);

	for (auto Weapon : Weapons)
	{
		if (!Weapon->ActorHasTag(TEXT("Weapon")))
			Weapons.Remove(Weapon);
	}

	for (auto Weapon : Weapons)
	{
		WeaponsMeshes.Add(Weapon->FindComponentByClass<UStaticMeshComponent>());
	}

	for (auto WeaponMesh : WeaponsMeshes)
	{
		TArray<FName> Sockets;
		Sockets = WeaponMesh->GetAllSocketNames();
		WeaponsSockets.Add(Sockets);
	}
}

void UWeaponAttack::PerformSweep()
{
	for (int i = 0; i < Weapons.Num(); i++)
	{
		for (int j = 0; j < WeaponsSockets[i].Num(); j += 2)
		{
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
			GetWorld()->SweepMultiByChannel(HitResults, StartLocation, EndLocation, Rotation, ECC_Pawn, CollisionShape, QueryParams);

			for (auto Hit : HitResults)
			{
				AActor* HitActor = Hit.GetActor();

				if (!HitActor || HitActors->Contains(HitActor))
					continue;

				APlayerCharacter* Character = Cast<APlayerCharacter>(HitActor);
				AEnemy* Enemy = Cast<AEnemy>(HitActor);

				if (Character)
				{
					HitActors->Add(HitActor);
					
					float Damage = 10.f;
					if (AEnemy* Attacker = Cast<AEnemy>(Owner))
					{
						Damage = Attacker->AttackDamage;
					}
					
					UGameplayStatics::ApplyDamage(Character, Damage, Owner->GetInstigatorController(), Owner, UDamageType::StaticClass());
					UE_LOG(LogTemp, Warning, TEXT("Weapon hit player for %f damage"), Damage);
				}
				else if (Enemy)
				{
					HitActors->Add(HitActor);
					
					// Pobierz obrażenia od właściciela ataku, jeśli jest nim Enemy
					float Damage = 10.f;
					if (AEnemy* Attacker = Cast<AEnemy>(Owner))
					{
						Damage = Attacker->AttackDamage;
					}
					
					Enemy->TakeDamageAmount(Damage);
				}
				
			}

			DrawDebugCapsule(GetWorld(), (StartLocation + EndLocation) / 2, HalfHeight, HitboxRadius, Rotation, FColor::Red, false, 0); // debug draw
		}
	}
}

void UWeaponAttack::ExecuteAttack()
{
	if (Weapons.IsEmpty())
		SetupWeapons();

	PerformSweep();
}