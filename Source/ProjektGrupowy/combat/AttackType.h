// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "NiagaraSystem.h"
#include "AttackType.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class PROJEKTGRUPOWY_API UAttackType : public UObject
{
	GENERATED_BODY()

public:
	virtual void InitializeAttack(USkeletalMeshComponent* InMeshComp, AActor* InOwner, TSet<TObjectPtr<AActor>>* InHitActors);

	virtual void ExecuteAttack() PURE_VIRTUAL(UAttackTypeBase::ExecuteAttack);

	UFUNCTION(BlueprintCallable, Category = "Attack")
	virtual bool IsFinished() const { return true; }

	UPROPERTY(BlueprintReadOnly, Category = "Attack")
	USkeletalMeshComponent* MeshComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Attack")
	AActor* Owner;

	/** Opcjonalny montaż animacji do odtworzenia podczas ataku. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	class UAnimMontage* AttackMontage;

	/** Opcjonalny efekt Niagara do zespawnowania na początku ataku. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	class UNiagaraSystem* StartVFX;

	TSet<TObjectPtr<AActor>>* HitActors;
};
