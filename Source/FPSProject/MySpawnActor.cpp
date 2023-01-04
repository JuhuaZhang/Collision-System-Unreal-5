// Fill out your copyright notice in the Description page of Project Settings.


#include "MySpawnActor.h"
#include "Components\BoxComponent.h"
#include "Engine\Classes\Kismet\KismetMathLibrary.h"
#include "Engine\Classes\Engine\World.h"
#include "MyCube.h"


// Sets default values
AMySpawnActor::AMySpawnActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// The spawing place
	SpawnVloume = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnVloume"));
	SpawnVloume->InitBoxExtent(FVector(200.0f, 200.0f, 200.0f));
	RootComponent = SpawnVloume;

}

// Called when the game starts or when spawned
void AMySpawnActor::BeginPlay()
{
	Super::BeginPlay();
	
	Spawning();

}

// Called every frame
void AMySpawnActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


FVector AMySpawnActor::GetSpawnPoint()
{
	FVector Extent = SpawnVloume->GetScaledBoxExtent();
	FVector Origin = SpawnVloume->GetComponentLocation();

	FVector Point = UKismetMathLibrary::RandomPointInBoundingBox(Origin, Extent);
	return Point;
}



void AMySpawnActor::Spawning()
{
		UWorld* MyWorld = GetWorld();

		if (MyWorld)
		{
			FVector Location = GetSpawnPoint();
			MyWorld->SpawnActor<AActor>(AMyCube::StaticClass(), Location, FRotator(0.f));
		}
}

/*

void AMySpawnActor::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Spawning", IE_Pressed, this, &AMySpawnActor::Spawning);
}

*/
