// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttackType.h"
#include "HitboxComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJEKTGRUPOWY_API UHitboxComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHitboxComponent();

	void StartDetection(USkeletalMeshComponent* MeshComp, AActor* Owner, TSubclassOf<UAttackType> AttackType);
	void StopDetection();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY()
	TObjectPtr<UAttackType> CurrentAttack;

	TSet<TObjectPtr<AActor>> HitActors;
};
