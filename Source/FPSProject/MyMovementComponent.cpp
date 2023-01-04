// Fill out your copyright notice in the Description page of Project Settings.


#include "MyMovementComponent.h"
#include "EngineDefines.h"
#include "GameFramework/DamageType.h"
#include "Engine/World.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/WorldSettings.h"
#include "ProfilingDebugging/CsvProfiler.h"
#include "MyCube.h"
#include "FPSProjectProjectile.h"

CSV_DECLARE_CATEGORY_MODULE_EXTERN(CORE_API, Basic);

const float UMyMovementComponent::MIN_TICK_TIME = 1e-6f;

UMyMovementComponent::UMyMovementComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bSweepCollision = true;
	Velocity = FVector(1.f, 0.f, 0.f);
	MaxSimulationIterations = 4;
	Boundiness = 0.75;
	MinSpeed = 20;
}

void UMyMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_MyMovementComponent_TickComponent);
	CSV_SCOPED_TIMING_STAT_EXCLUSIVE(MyMovementComponent);

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsValid(UpdatedComponent))
	{
		return;
	}

	AActor* ActorOwner = UpdatedComponent->GetOwner();
	if (!ActorOwner)
	{
		return;
	}

	float TimeTick = DeltaTime;
	FHitResult Hit(1.f);

	// Initial move state
	Hit.Time = 1.f;

	Velocity = UpdatedComponent->ComponentVelocity;
	const FVector OldVelocity = Velocity;
	Velocity = ComputeNewVelocity(OldVelocity, TimeTick);
	const FVector MoveDelta = (Velocity + OldVelocity) * TimeTick * 0.5;

	FQuat NewRotation = (!OldVelocity.IsNearlyZero(0.01f)) ? OldVelocity.ToOrientationQuat() : UpdatedComponent->GetComponentQuat();

	SafeMoveUpdatedComponent(MoveDelta, NewRotation, bSweepCollision, Hit);
		
	if (!Hit.bBlockingHit) {
			// There's no hit, do nothing.
	}
	else
	{
		const FVector Normal = ConstrainNormalToPlane(Hit.Normal);
		const float VDotNormal = (Velocity | Normal);

		FVector v_2 = Hit.GetActor()->GetVelocity();
		const float v_2DotNormal = (v_2 | Normal);

		Boundiness = 0.9;
			
		// if hit these 2 kinds, which use MyMovementComponent
		if (Hit.GetActor()->GetClass() == AMyCube::StaticClass() || Hit.GetActor()->GetClass() == AFPSProjectProjectile::StaticClass()) {
			Velocity = Boundiness * -1.5 * Normal * (VDotNormal - v_2DotNormal) + Velocity;
			Hit.GetActor()->GetRootComponent()->ComponentVelocity += 0.5 * Normal * (VDotNormal - v_2DotNormal);
		}
		else{
			FVector res = Boundiness * Normal * VDotNormal;

			if (res.Size() <= 50){
				Velocity = -1 * Normal * VDotNormal + Velocity;
			}
			else{
				Velocity = -1 * Normal * VDotNormal + Velocity - res;
			}
		}

		// limit to max
		Velocity = Velocity.GetClampedToMaxSize(MaxSpeed);
		const FVector MoveDeltaOnHit = Velocity * TimeTick;

		SafeMoveUpdatedComponent(MoveDeltaOnHit, NewRotation, bSweepCollision, Hit);
	}

	UpdateComponentVelocity();
}


void UMyMovementComponent::SetVelocityInLocalSpace(FVector NewVelocity)
{
	if (UpdatedComponent)
	{
		Velocity = UpdatedComponent->GetComponentToWorld().TransformVectorNoScale(NewVelocity);
	}
}


void UMyMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (Velocity.SizeSquared() > 0.f)
	{
		// InitialSpeed > 0 overrides initial velocity magnitude.
		if (InitialSpeed > 0.f)
		{
			Velocity = Velocity.GetSafeNormal() * InitialSpeed;
		}

		SetVelocityInLocalSpace(Velocity);

		UpdateComponentVelocity();

		if (UpdatedPrimitive && UpdatedPrimitive->IsSimulatingPhysics())
		{
			UpdatedPrimitive->SetPhysicsLinearVelocity(Velocity);
		}
	}
}


FVector UMyMovementComponent::ComputeNewVelocity(const FVector& InVelocity, float DeltaTime) const
{
	FVector Acceleration(FVector::ZeroVector);
	
	Acceleration.Z += GetGravityZ();
	
	FVector NewVelocity = InVelocity + Acceleration * DeltaTime;

	//FVector MoveDelta = (NewVelocity + InVelocity) * DeltaTime * 0.5;

	return NewVelocity;
}

// OnHit and add Force