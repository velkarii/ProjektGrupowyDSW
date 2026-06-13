#include "Enemy/Enemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NavigationSystem.h"
#include "Components/CapsuleComponent.h"

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

	// Rozsyłamy początkowe HP, aby pasek w UI nie startował od 0
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void AEnemy::Tick(float DeltaTime)
{
}

void AEnemy::TakeDamageAmount(float DamageAmount)
{
	TakeDamage(DamageAmount, FDamageEvent(), nullptr, nullptr);
}

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	CurrentHealth = FMath::Clamp(CurrentHealth - ActualDamage, 0.f, MaxHealth);
	// UE_LOG(LogTemp, Log, TEXT("Enemy took %f damage. Current Health: %f"), ActualDamage, CurrentHealth);

	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);

	if (CurrentHealth <= 0.f)
	{
		OnDeath();
	}

	return ActualDamage;
}

void AEnemy::ApplyNormalDamage(float DamageAmount)
{
	TakeDamage(DamageAmount, FDamageEvent(), nullptr, nullptr);
}

void AEnemy::Die()
{
	CurrentHealth = 0.f;
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	OnDeath();
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
		// UE_LOG(LogTemp, Warning, TEXT("[DEBUG_LOG] TeleportToTarget: Target is NULL!"));
		return;
	}

	FVector TargetLocation = Target->GetActorLocation();
	FVector TargetForward = Target->GetActorForwardVector();
	
	// Teleportacja przed gracza: TargetLocation + (TargetForward * Distance)
	FVector CandidateLocation = TargetLocation + (TargetForward * Distance);
	
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	FVector TeleportLocation = CandidateLocation;

	if (NavSys)
	{
		FNavLocation NavLocation;
		if (NavSys->ProjectPointToNavigation(CandidateLocation, NavLocation, FVector(200.f, 200.f, 500.f)))
		{
			TeleportLocation = NavLocation.Location;
		}
	}

	// UE_LOG(LogTemp, Log, TEXT("[DEBUG_LOG] TeleportToTarget: Attempting teleport to %s (Target: %s, Forward: %s)"), 
	// 	*TeleportLocation.ToString(), *TargetLocation.ToString(), *TargetForward.ToString());

	// Próbujemy teleportacji z bSweep = false, aby sprawdzić czy to kolizja blokuje.
	// Wyrównujemy do podłoża biorąc pod uwagę skalę kapsuły.
	float CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	TeleportLocation.Z += CapsuleHalfHeight + 2.0f; 

	bool bTeleported = SetActorLocation(TeleportLocation, false, nullptr, ETeleportType::TeleportPhysics);

	if (bTeleported)
	{
		// UE_LOG(LogTemp, Log, TEXT("[DEBUG_LOG] TeleportToTarget: SUCCESS"));
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
		// UE_LOG(LogTemp, Error, TEXT("[DEBUG_LOG] TeleportToTarget: FAILED to SetActorLocation"));
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
		// UE_LOG(LogTemp, Warning, TEXT("[DEBUG_LOG] TeleportAwayFromTarget: Target is NULL!"));
		return;
	}

	FVector TargetLocation = Target->GetActorLocation();
	
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!NavSys)
	{
		// UE_LOG(LogTemp, Error, TEXT("[DEBUG_LOG] TeleportAwayFromTarget: NavSys is NULL!"));
		return;
	}

	FVector TeleportLocation;
	bool bFoundValidLocation = false;

	// Próbujemy znaleźć losowy punkt na NavMesh w pobliżu celu w zadanym zakresie dystansu.
	// GetRandomReachablePointInRadius może być zbyt restrykcyjne (wymaga ścieżki).
	// Użyjemy losowania kąta i dystansu, a potem rzutowania na NavMesh.
	
	for (int32 i = 0; i < 10; ++i)
	{
		float RandomAngle = FMath::RandRange(0.f, 360.f);
		float RandomDistance = FMath::RandRange(MinDistance, MaxDistance);
		
		FVector Offset = FVector(FMath::Cos(FMath::DegreesToRadians(RandomAngle)), FMath::Sin(FMath::DegreesToRadians(RandomAngle)), 0.f) * RandomDistance;
		FVector CandidateLocation = TargetLocation + Offset;

		FNavLocation NavLocation;
		if (NavSys->ProjectPointToNavigation(CandidateLocation, NavLocation, FVector(200.f, 200.f, 500.f)))
		{
			TeleportLocation = NavLocation.Location;
			bFoundValidLocation = true;
			break;
		}
	}

	if (!bFoundValidLocation)
	{
		// UE_LOG(LogTemp, Error, TEXT("[DEBUG_LOG] TeleportAwayFromTarget: FAILED to find valid NavMesh location around target"));
		return;
	}
	
	// Podniesienie w górę biorąc pod uwagę skalę kapsuły, aby uniknąć problemów z podłożem
	float CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	TeleportLocation.Z += CapsuleHalfHeight + 2.0f;

	// UE_LOG(LogTemp, Log, TEXT("[DEBUG_LOG] TeleportAwayFromTarget: Attempting teleport to %s (Target: %s)"), 
	// 	*TeleportLocation.ToString(), *TargetLocation.ToString());

	bool bTeleported = SetActorLocation(TeleportLocation, false, nullptr, ETeleportType::TeleportPhysics);

	if (bTeleported)
	{
		// UE_LOG(LogTemp, Log, TEXT("[DEBUG_LOG] TeleportAwayFromTarget: SUCCESS"));
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
		// UE_LOG(LogTemp, Error, TEXT("[DEBUG_LOG] TeleportAwayFromTarget: FAILED to SetActorLocation"));
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

