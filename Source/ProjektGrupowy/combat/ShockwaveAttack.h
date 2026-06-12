// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "combat/AttackType.h"
#include "combat/HitboxComponent.h"
#include "ShockwaveAttack.generated.h"

class UNiagaraSystem;


UCLASS(Blueprintable, BlueprintType)
class PROJEKTGRUPOWY_API UShockwaveAttack : public UAttackType
{
	GENERATED_BODY()

public:
	virtual void InitializeAttack(USkeletalMeshComponent* InMeshComp, AActor* InOwner, TSet<TObjectPtr<AActor>>* InHitActors) override;
	virtual void ExecuteAttack() override;
	virtual bool IsFinished() const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shockwave")
	int32 NumBlocks = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shockwave")
	float TimeBetweenBlocks = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shockwave")
	FVector BlockExtent = FVector(80.f, 200.f, 40.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shockwave")
	float BlockStepDistance = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shockwave|Visuals")
	TObjectPtr<UNiagaraSystem> ShockwaveVFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shockwave|Visuals")
	FVector VFXScale = FVector(1.f, 1.f, 1.f);

protected:
	float CurrentTime;
	
	int32 LastSpawnedBlockIndex;
	
	UPROPERTY()
	TArray<AActor*> HitActors;
	FVector InitialBaseLocation;
	FVector LastBlockLocation;
	FVector ForwardDirection;
};
