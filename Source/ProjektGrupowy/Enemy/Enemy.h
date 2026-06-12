// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UENUM(BlueprintType)
enum class EEnemyType : uint8
{
	Minion,
	Boss,
	NPC
};

UCLASS()
class PROJEKTGRUPOWY_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	void PlayAttackMontage();
	void WieldSword();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void TeleportToTarget(AActor* Target, float Distance, bool bFaceTarget);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void TeleportAwayFromTarget(AActor* Target, float MinDistance, float MaxDistance, bool bFaceTarget);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Settings")
	EEnemyType EnemyType = EEnemyType::Minion;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float CurrentHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float AttackDamage = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	TSubclassOf<class UAttackType> MeleeAttackClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	TSubclassOf<class UAttackType> SpecialAttackClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	TSubclassOf<AActor> BP_Weapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
	TSubclassOf<AActor> PatrolRoute;

	bool IsWieldingSword = false;

protected:
	AEnemy();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void TakeDamageAmount(float DamageAmount);

	UFUNCTION(BlueprintNativeEvent, Category = "Stats")
	void OnDeath();

};
