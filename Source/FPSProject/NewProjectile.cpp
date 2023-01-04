// Fill out your copyright notice in the Description page of Project Settings.


#include "NewProjectile.h"
#include "MyMovementComponent.h"
#include "NewMovementComponent.h"
#include "Components/SphereComponent.h"

ANewProjectile::ANewProjectile()
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ANewProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// CollisionComp->SetSimulatePhysics(true);

	// Set as root component
	RootComponent = CollisionComp;

	// Use MyMovementComponent to govern this projectile's movement
	// ProjectileMovement = CreateDefaultSubobject<UMyMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement = CreateDefaultSubobject<UNewMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 800.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->Mass = 300.f;

	// Die after 3 seconds by default
	InitialLifeSpan = 10.0f;
}

void ANewProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Hit"));

	//// Only add impulse and destroy projectile if we hit a physics
	// if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	// {
		// OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

		// Destroy();
	//}
}