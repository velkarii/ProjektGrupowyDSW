// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Enemy.h"
#include "MinionEnemy.generated.h"

/**
 * Klasa reprezentująca zwykłego przeciwnika (Minion).
 */
UCLASS()
class PROJEKTGRUPOWY_API AMinionEnemy : public AEnemy
{
	GENERATED_BODY()

public:
	AMinionEnemy();
	
};
