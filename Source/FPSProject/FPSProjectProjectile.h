// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FPSProjectProjectile.generated.h"

class USphereComponent;
// class UProjectileMovementComponent;
class UMyMovementComponent;
class UNewMovementComponent;

UCLASS(config=Game)
class AFPSProjectProjectile : public AActor
{
	GENERATED_BODY()

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	USphereComponent* CollisionComp;

	/** Projectile movement component */	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UMyMovementComponent* ProjectileMovement;
	// UNewMovementComponent* ProjectileMovement;

public:
	AFPSProjectProjectile();

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns CollisionComp subobject **/
	USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	//  UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
	UMyMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
	// UNewMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
};

