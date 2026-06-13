// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "combat/AttackType.h"
#include "SkyFallAttack.generated.h"

class UNiagaraSystem;

/**
 * Atak, w którym przeciwnik wznosi się w górę, a następnie spada na gracza,
 * tworząc duży sferyczny hitbox przy uderzeniu o ziemię.
 */
UCLASS(Blueprintable, BlueprintType)
class PROJEKTGRUPOWY_API USkyFallAttack : public UAttackType
{
	GENERATED_BODY()

public:
	USkyFallAttack();

	virtual void InitializeAttack(USkeletalMeshComponent* InMeshComp, AActor* InOwner, TSet<TObjectPtr<AActor>>* InHitActors) override;
	virtual void ExecuteAttack() override;
	virtual bool IsFinished() const override;

	/** Wysokość, na jaką wznosi się przeciwnik. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sky Fall")
	float RiseHeight = 1500.f;

	/** Prędkość wznoszenia. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sky Fall")
	float RiseSpeed = 2000.f;

	/** Prędkość opadania. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sky Fall")
	float FallSpeed = 6000.f;

	/** Odległość lądowania przed graczem. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sky Fall")
	float LandingOffsetDistance = 200.f;

	/** Promień sferycznego hitboxa przy uderzeniu. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sky Fall")
	float HitboxRadius = 500.f;

	/** Obrażenia ataku. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sky Fall")
	float Damage = 30.f;

	/** System cząsteczek przy uderzeniu. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sky Fall|Visuals")
	TObjectPtr<UNiagaraSystem> ImpactVFX;

	/** System cząsteczek przed startem ataku. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sky Fall|Visuals")
	TObjectPtr<UNiagaraSystem> PreAttackVFX;

	/** Opóźnienie przed wzniesieniem się. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sky Fall")
	float PreAttackDelay = 1.0f;

	/** Siła odrzutu gracza przy uderzeniu. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sky Fall")
	float KnockbackStrength = 1500.f;

	/** Czas przez jaki kolizje z graczem są ignorowane po uderzeniu. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sky Fall")
	float CollisionIgnoreDurationAfterImpact = 0.5f;

	/** Czas zatrzymania w powietrzu przed skokiem. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sky Fall")
	float HoverDuration = 4.0f;

protected:
	float ImpactTime;
	float HoverStartTime;
	float WarningStartTime;
	enum class EAttackPhase : uint8
	{
		Warning,
		Rising,
		Waiting,
		Falling,
		Impacted,
		Finished
	};

	EAttackPhase CurrentPhase;
	FVector TargetImpactLocation;
	FVector StartLocation;
	float CurrentHeightOffset;

	void PerformImpact();
};
