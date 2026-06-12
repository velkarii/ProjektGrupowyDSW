// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/BossEnemy.h"

ABossEnemy::ABossEnemy()
{
	EnemyType = EEnemyType::Boss;
	
	// Domyślne statystyki dla Bossa (zwiększone względem zwykłego przeciwnika)
	MaxHealth = 500.f;
	AttackDamage = 30.f;
}
