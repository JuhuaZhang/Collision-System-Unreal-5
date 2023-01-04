// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NewProjectile.generated.h"


class USphereComponent;
// class UProjectileMovementComponent;
class UMyMovementComponent;
class UNewMovementComponent;

UCLASS(config = Game)
class FPSPROJECT_API ANewProjectile : public AActor
{
	GENERATED_BODY()

		/** Sphere collision component */
		UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
		USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		// UMyMovementComponent* ProjectileMovement;
		UNewMovementComponent* ProjectileMovement;

public:
	ANewProjectile();

	/** called when projectile hits something */
	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns CollisionComp subobject **/
	USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	//  UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
	// UMyMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
	UNewMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
};

