// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/MinionEnemy.h"
#include "combat/EnemyMeleeAttack.h"
#include "combat/ShockwaveAttack.h"

AMinionEnemy::AMinionEnemy()
{
	EnemyType = EEnemyType::Minion;
	
	// Domyślne statystyki dla Miniona
	MaxHealth = 100.f;
	AttackDamage = 10.f;

	// Domyślne ataki
	MeleeAttackClass = UEnemyMeleeAttack::StaticClass();
	SpecialAttackClass = UShockwaveAttack::StaticClass();
}
