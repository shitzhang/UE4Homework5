// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShooterWeapon.h"
#include "ShooterProjectileWeapon.generated.h"

/**
 * 
 */
UCLASS()
class PROTOTYPE_API AShooterProjectileWeapon : public AShooterWeapon
{
	GENERATED_BODY()

protected:

	AShooterProjectileWeapon();

	virtual void Fire() override;

	UPROPERTY(EditDefaultsOnly, Category = "ProjectileWeapon")
	TSubclassOf<AActor> ProjectileClass;
	
};
