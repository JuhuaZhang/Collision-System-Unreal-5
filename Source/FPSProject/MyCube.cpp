// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCube.h"
#include "MyMovementComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
AMyCube::AMyCube()
{
	// Use a box as a simple collision representation
	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	CollisionComp->InitBoxExtent(FVector(20.0f, 20.0f, 20.0f));
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AMyCube::OnHit);		// set up a notification for when this component hits something blocking
	
	// DrawDebugSphere(GetWorld(), GetActorLocation(), CollisionComp->GetCollisionShape().GetSphereRadius(), 50, FColor::Red, true);

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// CollisionComp->SetSimulatePhysics(true);

	// Set as root component
	RootComponent = CollisionComp;

	// Use MyMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UMyMovementComponent>(TEXT("MyBoxComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = mySpeed;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->Mass = 500.f;

	if (!ProjectileMeshComponent){
		ProjectileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoxMeshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh(TEXT("'/Game/LevelPrototyping/Meshes/SM_ChamferCube.SM_ChamferCube'"));
		// static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh(TEXT("'/Game/LevelPrototyping/Meshes/SM_Cube.SM_Cube'"));
		if (Mesh.Succeeded()){
			ProjectileMeshComponent->SetStaticMesh(Mesh.Object);
		}
	}

	ProjectileMeshComponent->SetWorldScale3D(FVector(20.0f, 20.0f, 20.0f));
	// ProjectileMeshComponent->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	ProjectileMeshComponent->SetRelativeScale3D(FVector(0.4f, 0.4f, 0.4f));

	ProjectileMeshComponent->SetupAttachment(RootComponent);

	CollisionComp->SetRelativeScale3D(FVector(1.4f, 1.4f, 1.4f));
}

void AMyCube::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

}