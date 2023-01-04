// Fill out your copyright notice in the Description page of Project Settings.


#include "NewMovementComponent.h"
#include "EngineDefines.h"
#include "GameFramework/DamageType.h"
#include "Engine/World.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/WorldSettings.h"
#include "ProfilingDebugging/CsvProfiler.h"
#include "MyCube.h"
#include "FPSProjectProjectile.h"
#include "Math/UnrealMathUtility.h"
#include "Math/RotationMatrix.h"

CSV_DECLARE_CATEGORY_MODULE_EXTERN(CORE_API, Basic);

const float UNewMovementComponent::MIN_TICK_TIME = 1e-6f;

UNewMovementComponent::UNewMovementComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bSweepCollision = true;
	Velocity = FVector(1.f, 0.f, 0.f);
	MaxSimulationIterations = 4;
	Boundiness = 0.75;
	MinSpeed = 20;
	t = 0;
}

void UNewMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_NewMovementComponent_TickComponent);
	CSV_SCOPED_TIMING_STAT_EXCLUSIVE(NewMovementComponent);

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

	if (UpdatedComponent->IsSimulatingPhysics())
	{
		return;
	}

	float RemainingTime = DeltaTime;
	int32 NumImpacts = 0;
	int32 NumBounces = 0;
	int32 LoopCount = 0;
	int32 Iterations = 0;
	FHitResult Hit(1.f);
	int flag = 0;
	R = FRotationMatrix::MakeFromZ(Velocity);

	while (RemainingTime >= MIN_TICK_TIME && (Iterations < MaxSimulationIterations) && IsValid(ActorOwner))
	{
		LoopCount++;
		Iterations++;

		// subdivide long ticks to more closely follow parabolic trajectory
		const float InitialTimeRemaining = RemainingTime;
		const float TimeTick = RemainingTime;
		RemainingTime -= TimeTick;

		// Initial move state
		Hit.Time = 1.f;

		Velocity = UpdatedComponent->ComponentVelocity;
		if (flag == 0)
		{
			OriginalV = Velocity;
			flag = 1;
		}
		const FVector OldVelocity = Velocity;
		Velocity = ComputeNewVelocity(OriginalV, TimeTick, Iterations);
		FVector DeltaLocation = UpdateLocation(OriginalV, TimeTick, t);
		t+= 10;
		const FVector MoveDelta = (Velocity + OldVelocity) * TimeTick * 0.5 + DeltaLocation;
		
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

			FString com_type_1 = "StaticMeshComponent0";

			// GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, *GetNameSafe(Hit.GetActor()));

			/*

			if (*GetNameSafe(Hit.GetComponent()) == com_type_1) {
				Velocity = Boundiness * -2 * Normal * VDotNormal + Velocity;
			}
			else {
				Velocity = Boundiness * -1.5 * Normal * (VDotNormal - v_2DotNormal) + Velocity;
				Hit.GetActor()->GetRootComponent()->ComponentVelocity += 0.5 * Normal * (VDotNormal - v_2DotNormal);
			}

			*/

			// if hit these 2 kinds

			if (Hit.GetActor()->GetClass() == AMyCube::StaticClass() || Hit.GetActor()->GetClass() == AFPSProjectProjectile::StaticClass()) {
				Velocity = Boundiness * -1.5 * Normal * (VDotNormal - v_2DotNormal) + Velocity;
				Hit.GetActor()->GetRootComponent()->ComponentVelocity += 0.5 * Normal * (VDotNormal - v_2DotNormal);
			}
			else
			{
				FVector res = Boundiness * Normal * VDotNormal;

				if (res.Size() <= 50)
				{
					Velocity = -1 * Normal * VDotNormal + Velocity;
				}
				else
				{
					Velocity = -1 * Normal * VDotNormal + Velocity - res;
				}


			}

			// limit to max
			Velocity = Velocity.GetClampedToMaxSize(MaxSpeed);
			
			flag = 0;
			const FVector MoveDeltaOnHit = Velocity * TimeTick;

			SafeMoveUpdatedComponent(MoveDeltaOnHit, NewRotation, bSweepCollision, Hit);

			UpdateComponentVelocity();
		}
	}

	UpdateComponentVelocity();
}


void UNewMovementComponent::SetVelocityInLocalSpace(FVector NewVelocity)
{
	if (UpdatedComponent)
	{
		Velocity = UpdatedComponent->GetComponentToWorld().TransformVectorNoScale(NewVelocity);
	}
}


void UNewMovementComponent::InitializeComponent()
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
	// R = FRotationMatrix::MakeFromZ(Velocity);
}


FVector UNewMovementComponent::ComputeNewVelocity(FVector& InVelocity, float DeltaTime, int32 Iterations) const
{
	FVector Acceleration(FVector::ZeroVector);
	
	//Acceleration.X = InVelocity.X - 5;
	//Acceleration.Y = InVelocity.Y;
	//Acceleration.Z = InVelocity.Z;

	//FVector Delta = (FVector::DotProduct(Acceleration, InVelocity)) * InVelocity.GetSafeNormal();
	//Acceleration = Acceleration - Delta;
	//Acceleration.Normalize();

	// Acceleration.X = - FMath::Sin(Iterations * DeltaTime);
	// GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, Acceleration.ToString());
	// Acceleration = R.TransformPosition(Acceleration);

	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, Acceleration.ToString());
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, InVelocity.ToString());
	//Acceleration = FVector::CrossProduct(Acceleration, InVelocity);
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, Acceleration.ToString());
	//Acceleration.Normalize();

	//APlayerController* const MyPlayer = Cast<APlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
	//if (MyPlayer != NULL)
	//{
	//	MyPlayer->SetPause(true);
	//}

	//Acceleration = 20* FMath::Cos(Iterations * DeltaTime) * Acceleration;
	//// GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, InVelocity.ToString());
	
	
	FVector NewVelocity = InVelocity + Acceleration;

	// FVector MoveDelta = (NewVelocity + InVelocity) * DeltaTime * 0.5;

	return NewVelocity;
}

FVector UNewMovementComponent::UpdateLocation(FVector& InVelocity, float DeltaTime, int32 Iterations)
{
	FVector F1(FVector::ZeroVector);

	F1.X = InVelocity.X - 5;
	F1.Y = InVelocity.Y;
	F1.Z = InVelocity.Z ;
	F1 = FVector::CrossProduct(F1, InVelocity);
	F1.Normalize();

	FVector DeltaLoc = - 3 * FMath::Sin(Iterations * DeltaTime) * F1;

	FVector F2 = FVector::CrossProduct(F1, InVelocity);
	F2.Normalize();

	DeltaLoc += 3 * FMath::Cos(Iterations * DeltaTime) * F2;

	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("%lld"), Iterations));
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, DeltaLoc.ToString());

	return DeltaLoc;
}