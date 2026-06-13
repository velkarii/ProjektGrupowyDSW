#include "Enemy/Enemy.h"
#include "GameFramework/CharacterMovementComponent.h"

AEnemy::AEnemy()
{
	CurrentHealth = MaxHealth;

	// Zapobiega zwalnianiu przed celem podczas korzystania z MoveTo
	if (GetCharacterMovement())
	{
		// W niektórych wersjach UE parametr ten może znajdować się w PathFollowingComponent
		// ale wyłączenie bRequestedMoveUseAcceleration w CharacterMovement również pomaga.
		GetCharacterMovement()->bRequestedMoveUseAcceleration = false;
	}
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
}

void AEnemy::Tick(float DeltaTime)
{
}

void AEnemy::TakeDamageAmount(float DamageAmount)
{
	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.f, MaxHealth);
	
	if (CurrentHealth <= 0.f)
	{
		OnDeath();
	}
}

void AEnemy::OnDeath_Implementation()
{
	// Logika śmierci (np. animacja ragdoll, usunięcie aktora)
	Destroy();
}

void AEnemy::PlayAttackMontage()
{
	if (!AttackMontage) 
		return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance || AnimInstance->Montage_IsPlaying(AttackMontage))
		return;

	AnimInstance->Montage_Play(AttackMontage);
}

void AEnemy::WieldSword()
{
	if (BP_Weapon == nullptr) 
		return;

	AActor* Weapon = GetWorld()->SpawnActor<AActor>(BP_Weapon, GetActorLocation(), GetActorRotation());
	Weapon->Tags.Add(TEXT("Weapon"));
	Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName(TEXT("WeaponR")));
	IsWieldingSword = true;
}

void AEnemy::TeleportToTarget(AActor* Target, float Distance, bool bFaceTarget)
{
	if (!Target)
	{
		UE_LOG(LogTemp, Warning, TEXT("[DEBUG_LOG] TeleportToTarget: Target is NULL!"));
		return;
	}

	FVector TargetLocation = Target->GetActorLocation();
	FVector TargetForward = Target->GetActorForwardVector();
	
	// Teleportacja przed gracza: TargetLocation + (TargetForward * Distance)
	FVector TeleportLocation = TargetLocation + (TargetForward * Distance);
	
	UE_LOG(LogTemp, Log, TEXT("[DEBUG_LOG] TeleportToTarget: Attempting teleport to %s (Target: %s, Forward: %s)"), 
		*TeleportLocation.ToString(), *TargetLocation.ToString(), *TargetForward.ToString());

	// Próbujemy teleportacji z bSweep = false, aby sprawdzić czy to kolizja blokuje.
	// Jeśli chcemy uniknąć blokowania przez podłogę, warto dodać lekki offset w górę.
	TeleportLocation.Z += 50.f; 

	bool bTeleported = SetActorLocation(TeleportLocation, false, nullptr, ETeleportType::TeleportPhysics);

	if (bTeleported)
	{
		UE_LOG(LogTemp, Log, TEXT("[DEBUG_LOG] TeleportToTarget: SUCCESS"));
		if (bFaceTarget)
		{
			FVector LookAtDir = (TargetLocation - GetActorLocation());
			LookAtDir.Z = 0;
			if (!LookAtDir.IsNearlyZero())
			{
				SetActorRotation(LookAtDir.Rotation());
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[DEBUG_LOG] TeleportToTarget: FAILED to SetActorLocation"));
	}
}

void AEnemy::TeleportToTargetDelayed(AActor* Target, float Distance, bool bFaceTarget, float Delay)
{
	if (Delay <= 0.f)
	{
		TeleportToTarget(Target, Distance, bFaceTarget);
		return;
	}

	FTimerHandle TimerHandle;
	FTimerDelegate TimerDel;
	TimerDel.BindUObject(this, &AEnemy::TeleportToTarget, Target, Distance, bFaceTarget);
	GetWorldTimerManager().SetTimer(TimerHandle, TimerDel, Delay, false);
}

void AEnemy::TeleportAwayFromTarget(AActor* Target, float MinDistance, float MaxDistance, bool bFaceTarget)
{
	if (!Target)
	{
		UE_LOG(LogTemp, Warning, TEXT("[DEBUG_LOG] TeleportAwayFromTarget: Target is NULL!"));
		return;
	}

	FVector TargetLocation = Target->GetActorLocation();
	FVector EnemyLocation = GetActorLocation();
	
	FVector DirectionAway = (EnemyLocation - TargetLocation);
	DirectionAway.Z = 0;
	
	if (DirectionAway.IsNearlyZero())
	{
		// Jeśli stoimy w tym samym miejscu, teleportujemy się w losowym kierunku
		DirectionAway = FVector(FMath::RandRange(-1.f, 1.f), FMath::RandRange(-1.f, 1.f), 0).GetSafeNormal();
	}
	else
	{
		DirectionAway.Normalize();
	}

	float RandomDistance = FMath::RandRange(MinDistance, MaxDistance);
	FVector TeleportLocation = TargetLocation + (DirectionAway * RandomDistance);
	
	// Podniesienie lekko w górę, aby uniknąć problemów z podłożem
	TeleportLocation.Z += 50.f;

	UE_LOG(LogTemp, Log, TEXT("[DEBUG_LOG] TeleportAwayFromTarget: Attempting teleport away to %s (Target: %s)"), 
		*TeleportLocation.ToString(), *TargetLocation.ToString());

	bool bTeleported = SetActorLocation(TeleportLocation, false, nullptr, ETeleportType::TeleportPhysics);

	if (bTeleported)
	{
		UE_LOG(LogTemp, Log, TEXT("[DEBUG_LOG] TeleportAwayFromTarget: SUCCESS"));
		if (bFaceTarget)
		{
			FVector LookAtDir = (TargetLocation - GetActorLocation());
			LookAtDir.Z = 0;
			if (!LookAtDir.IsNearlyZero())
			{
				SetActorRotation(LookAtDir.Rotation());
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[DEBUG_LOG] TeleportAwayFromTarget: FAILED to SetActorLocation"));
	}
}

void AEnemy::TeleportAwayFromTargetDelayed(AActor* Target, float MinDistance, float MaxDistance, bool bFaceTarget, float Delay)
{
	if (Delay <= 0.f)
	{
		TeleportAwayFromTarget(Target, MinDistance, MaxDistance, bFaceTarget);
		return;
	}

	FTimerHandle TimerHandle;
	FTimerDelegate TimerDel;
	TimerDel.BindUObject(this, &AEnemy::TeleportAwayFromTarget, Target, MinDistance, MaxDistance, bFaceTarget);
	GetWorldTimerManager().SetTimer(TimerHandle, TimerDel, Delay, false);
}

