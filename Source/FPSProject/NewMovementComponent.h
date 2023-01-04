// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "GameFramework/MovementComponent.h"
#include "NewMovementComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = Movement, meta = (BlueprintSpawnableComponent), ShowCategories = (Velocity))
class FPSPROJECT_API UNewMovementComponent : public UMovementComponent
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
		float Mass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
		float InitialSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
		float MaxSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
		float Boundiness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
		float MinSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ProjectileSimulation)
		uint8 bSweepCollision : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "1", ClampMax = "25", UIMin = "1", UIMax = "25"), Category = ProjectileSimulation)
		int32 MaxSimulationIterations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
		FVector OriginalV;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
		FMatrix R; // rotation matrix

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void InitializeComponent() override;

	UFUNCTION(BlueprintCallable, Category = "Game|Components|ProjectileMovement")
		virtual void SetVelocityInLocalSpace(FVector NewVelocity);
	// float GetSimulationTimeStep(float RemainingTime, int32 Iterations) const;

	/** Compute the distance we should move in the given time, at a given a velocity. */
	virtual FVector ComputeNewVelocity(FVector& InVelocity, float DeltaTime, int32 Iterations) const;

	FVector UpdateLocation(FVector& InVelocity, float DeltaTime, int32 Iterations);


	int32 t = 0;
private:
	// Pending force for next tick.
	FVector PendingForce;

protected:
	/** Minimum delta time considered when ticking. Delta times below this are not considered. This is a very small non-zero positive value to avoid potential divide-by-zero in simulation code. */
	static const float MIN_TICK_TIME;

	
};