// Fill out your copyright notice in the Description page of Project Settings.

#include "combat/SkyFallAttack.h"
#include "Enemy/Enemy.h"
#include "Player/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/OverlapResult.h"

USkyFallAttack::USkyFallAttack()
{
	CurrentPhase = EAttackPhase::Warning;
	CurrentHeightOffset = 0.f;
}

void USkyFallAttack::InitializeAttack(USkeletalMeshComponent* InMeshComp, AActor* InOwner, TSet<TObjectPtr<AActor>>* InHitActors)
{
	Super::InitializeAttack(InMeshComp, InOwner, InHitActors);
	
	if (Owner)
	{
		StartLocation = Owner->GetActorLocation();
		CurrentPhase = EAttackPhase::Warning;
		WarningStartTime = GetWorld()->GetTimeSeconds();
		CurrentHeightOffset = 0.f;

		if (PreAttackVFX)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), PreAttackVFX, StartLocation, Owner->GetActorRotation());
		}

		// Wyłącz kolizję z graczem na czas lotu
		APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		if (Player)
		{
			if (ACharacter* OwnerChar = Cast<ACharacter>(Owner))
			{
				// OwnerChar->GetCapsuleComponent()->IgnoreActorWhenMoving(Player, true);
				// UE_LOG(LogTemp, Log, TEXT("[DEBUG_LOG] SkyFallAttack: Ignoring Player collisions during flight"));
			}
		}

		// Wyłącz grawitację na czas ataku, abyśmy mogli ręcznie kontrolować wysokość
		if (ACharacter* Char = Cast<ACharacter>(Owner))
		{
			Char->GetCharacterMovement()->GravityScale = 0.f;
			Char->GetCharacterMovement()->Velocity = FVector::ZeroVector;
		}

		// UE_LOG(LogTemp, Log, TEXT("[DEBUG_LOG] SkyFallAttack: Initialized at %s"), *StartLocation.ToString());
	}
}

void USkyFallAttack::ExecuteAttack()
{
	if (!Owner || !GetWorld()) return;

	float DeltaTime = GetWorld()->GetDeltaSeconds();
	FVector CurrentLoc = Owner->GetActorLocation();

	switch (CurrentPhase)
	{
	case EAttackPhase::Warning:
		{
			if (GetWorld()->GetTimeSeconds() - WarningStartTime >= PreAttackDelay)
			{
				CurrentPhase = EAttackPhase::Rising;
				// UE_LOG(LogTemp, Log, TEXT("[DEBUG_LOG] SkyFallAttack: Warning phase finished, starting Rising"));
			}
		}
		break;

	case EAttackPhase::Rising:
		{
			CurrentHeightOffset += RiseSpeed * DeltaTime;
			FVector NewLoc = StartLocation + FVector(0.f, 0.f, CurrentHeightOffset);
			Owner->SetActorLocation(NewLoc, true);

			if (CurrentHeightOffset >= RiseHeight)
			{
				CurrentPhase = EAttackPhase::Waiting;
				HoverStartTime = GetWorld()->GetTimeSeconds();

				// UE_LOG(LogTemp, Log, TEXT("[DEBUG_LOG] SkyFallAttack: Hovering for %f seconds"), HoverDuration);
			}
		}
		break;

	case EAttackPhase::Waiting:
		{
			// Rysuj sferę podglądu podczas oczekiwania
			// DrawDebugSphere(GetWorld(), Owner->GetActorLocation(), HitboxRadius, 12, FColor::Yellow, false, 0.f);

			if (GetWorld()->GetTimeSeconds() - HoverStartTime >= HoverDuration)
			{
				CurrentPhase = EAttackPhase::Falling;
				
				// Namierz gracza
				APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
				if (Player)
				{
					TargetImpactLocation = Player->GetActorLocation();
					// Dodaj mały offset, żeby trafić w stopy/ziemię
					TargetImpactLocation.Z -= Player->GetSimpleCollisionHalfHeight();
				}
				else
				{
					TargetImpactLocation = StartLocation;
				}
				
				// UE_LOG(LogTemp, Log, TEXT("[DEBUG_LOG] SkyFallAttack: Falling towards %s"), *TargetImpactLocation.ToString());
			}
		}
		break;

	case EAttackPhase::Falling:
		{
			FVector Direction = (TargetImpactLocation - CurrentLoc).GetSafeNormal();
			float DistanceToTarget = FVector::Dist(CurrentLoc, TargetImpactLocation);
			
			// Ruch bezpośrednio w stronę celu
			FVector MoveStep = Direction * FallSpeed * DeltaTime;
			
			// Rysuj sferę podglądu ataku podczas spadania
			// DrawDebugSphere(GetWorld(), CurrentLoc, HitboxRadius, 12, FColor::Yellow, false, 0.f);

			FHitResult Hit;
			// Przesuń aktora. Używamy bSweep=true, ale chcemy ignorować gracza podczas tego ruchu,
			// aby nie zatrzymywać się na jego głowie.
			
			// SetActorLocation z bSweep=true używa domyślnych ustawień kolizji aktora.
			// Skoro już wywołaliśmy IgnoreActorWhenMoving w InitializeAttack, to powinno wystarczyć,
			// ale tutaj dodatkowo upewniamy się, że ignorujemy gracza przy sprawdzaniu czy faza powinna się zakończyć.
			
			Owner->SetActorLocation(CurrentLoc + MoveStep, true, &Hit);

			// Jeśli uderzyliśmy w ziemię, gracza lub dotarliśmy do celu
			if (Hit.bBlockingHit || MoveStep.Size() >= DistanceToTarget)
			{
				if (Hit.bBlockingHit)
				{
					// UE_LOG(LogTemp, Log, TEXT("[DEBUG_LOG] SkyFallAttack: Hit something: %s"), Hit.GetActor() ? *Hit.GetActor()->GetName() : TEXT("NULL"));
					Owner->SetActorLocation(Hit.Location, false);
				}
				else
				{
					Owner->SetActorLocation(TargetImpactLocation, false);
				}
				PerformImpact();
			}
			else
			{
				// Kontynuuj obrót w stronę uderzenia - TYLKO YAW, aby uniknąć przechylania bossa w dół
				if (!Direction.IsNearlyZero())
				{
					FRotator NewRot = Direction.Rotation();
					NewRot.Pitch = 0.f;
					NewRot.Roll = 0.f;
					Owner->SetActorRotation(NewRot);
				}
			}
		}
		break;

	case EAttackPhase::Impacted:
		// Poczekaj chwilę przed przywróceniem kolizji i zakończeniem ataku
		if (GetWorld()->GetTimeSeconds() - ImpactTime >= CollisionIgnoreDurationAfterImpact)
		{
			APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
			if (Player && Owner)
			{
				if (ACharacter* OwnerChar = Cast<ACharacter>(Owner))
				{
					OwnerChar->GetCapsuleComponent()->IgnoreActorWhenMoving(Player, false);
					// UE_LOG(LogTemp, Log, TEXT("[DEBUG_LOG] SkyFallAttack: Restored Player collisions after delay"));
				}
			}

			// UE_LOG(LogTemp, Log, TEXT("[DEBUG_LOG] SkyFallAttack: Phase IMPACTED -> FINISHED"));
			CurrentPhase = EAttackPhase::Finished;
		}
		break;

	default:
		break;
	}
}

void USkyFallAttack::PerformImpact()
{
	CurrentPhase = EAttackPhase::Impacted;
	ImpactTime = GetWorld()->GetTimeSeconds();

	// Przywróć grawitację i zatrzymaj animację spadania
	if (ACharacter* Char = Cast<ACharacter>(Owner))
	{
		Char->GetCharacterMovement()->GravityScale = 1.f;
		Char->GetCharacterMovement()->SetMovementMode(MOVE_Walking); // Powrót do chodzenia natychmiast
		Char->GetCharacterMovement()->Velocity = FVector::ZeroVector;

		// Wyrównanie do podłoża (Ground Snapping) z poprawkami
		FHitResult GroundHit;
		FVector Start = Char->GetActorLocation() + FVector(0, 0, 100.f); // Zacznij wyżej, żeby uniknąć bycia pod podłogą
		FVector End = Start - FVector(0, 0, 500.f); // Większy zasięg
		FCollisionQueryParams TraceParams;
		TraceParams.AddIgnoredActor(Char);

		if (GetWorld()->LineTraceSingleByChannel(GroundHit, Start, End, ECC_Visibility, TraceParams))
		{
			float CapsuleHalfHeight = Char->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
			// Ustawiamy lekko powyżej (2 jednostki), aby CharacterMovement nie uznał, że penetrujemy ziemię
			Char->SetActorLocation(GroundHit.Location + FVector(0, 0, CapsuleHalfHeight + 2.0f), false);

			// Reset rotation - ensure Pitch and Roll are zero, keeping only Yaw
			FRotator CurrentRotation = Char->GetActorRotation();
			FRotator NewRotation = FRotator(0.f, CurrentRotation.Yaw, 0.f);
			Char->SetActorRotation(NewRotation);

			// UE_LOG(LogTemp, Log, TEXT("[DEBUG_LOG] SkyFallAttack: Snapped to ground and rotation reset"));
		}

		// Zatrzymaj montaż ataku (jeśli istnieje)
		if (UAnimInstance* AnimInstance = Char->GetMesh()->GetAnimInstance())
		{
			if (AnimInstance->IsAnyMontagePlaying())
			{
				AnimInstance->Montage_Stop(0.1f, AttackMontage);
				// UE_LOG(LogTemp, Log, TEXT("[DEBUG_LOG] SkyFallAttack: Montage Stopped"));
			}
		}
	}

	FVector ImpactLoc = Owner->GetActorLocation();
	
	// Sferyczny hitbox
	TArray<FOverlapResult> OverlapResults;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(HitboxRadius);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Owner);

	bool bHit = GetWorld()->OverlapMultiByChannel(OverlapResults, ImpactLoc, FQuat::Identity, ECC_Pawn, Sphere, QueryParams);

	if (bHit)
	{
		for (auto& Overlap : OverlapResults)
		{
			AActor* HitActor = Overlap.GetActor();
			if (HitActor && HitActors && !HitActors->Contains(HitActor))
			{
				HitActors->Add(HitActor);
				
				UGameplayStatics::ApplyDamage(HitActor, Damage, Owner->GetInstigatorController(), Owner, UDamageType::StaticClass());
				
				// Efekt Niagara dla każdego trafionego aktora
				if (ImpactVFX)
				{
					UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ImpactVFX, HitActor->GetActorLocation(), FRotator::ZeroRotator, FVector(1.f));
				}
				
   	// Odrzut (Knockback) dla gracza
				if (APlayerCharacter* Player = Cast<APlayerCharacter>(HitActor))
				{
					FVector KnockbackDir = (Player->GetActorLocation() - ImpactLoc).GetSafeNormal2D();
					if (KnockbackDir.IsNearlyZero()) KnockbackDir = Player->GetActorForwardVector() * -1.f;
					
					Player->LaunchCharacter(KnockbackDir * KnockbackStrength + FVector(0,0, 300.f), true, true);
					// UE_LOG(LogTemp, Log, TEXT("[DEBUG_LOG] SkyFallAttack: Knockback applied to Player"));
				}

				// UE_LOG(LogTemp, Log, TEXT("[DEBUG_LOG] SkyFallAttack: Hit %s"), *HitActor->GetName());
			}
		}
	}

	// DrawDebugSphere usunięty lub ustawiony na bardzo krótki czas, aby "zniknął" zaraz po uderzeniu
	// DrawDebugSphere(GetWorld(), ImpactLoc, HitboxRadius, 12, FColor::Red, false, 0.05f);
	// UE_LOG(LogTemp, Log, TEXT("[DEBUG_LOG] SkyFallAttack: IMPACT at %s"), *ImpactLoc.ToString());
}

bool USkyFallAttack::IsFinished() const
{
	return CurrentPhase == EAttackPhase::Finished;
}
