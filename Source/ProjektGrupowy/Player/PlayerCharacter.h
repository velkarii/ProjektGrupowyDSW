// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class PROJEKTGRUPOWY_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

    /** Czy gracz jest obecnie nieśmiertelny (np. podczas uniku). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    bool bIsInvulnerable = false;

    /** Włącza/wyłącza nieśmiertelność. */
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void SetInvulnerable(bool bEnabled);

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    class UCameraComponent* Camera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    class USpringArmComponent* SpringArm;
	
    UPROPERTY(EditAnywhere, Category = "EnhancedInput")
    class UInputMappingContext* InputMapping;

    UPROPERTY(EditAnywhere, Category = "EnhancedInput")
    class UInputAction* MoveAction;

    UPROPERTY(EditAnywhere, Category = "EnhancedInput")
    class UInputAction* LookAction;


public:

    UPROPERTY(EditAnywhere, Category = "EnhancedInput")
    class UInputAction* AttackAction;

    UPROPERTY(EditAnywhere, Category = "Animation")
    class UAnimMontage* AttackMontage;
	
    void Attack();
	
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetMovementLock(bool bLocked);
protected:
    // Movement
    void Move(const FInputActionValue& InputValue);
    void Look(const FInputActionValue& InputValue);
    FVector LastMovementInput;
};
