// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UCLASS()
class PROJEKTGRUPOWY_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	void PlayAttackMontage();
	void WieldSword();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	TSubclassOf<AActor> BP_Weapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
	TSubclassOf<AActor> PatrolRoute;

	bool IsWieldingSword = false;

protected:
	AEnemy();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

};
