// Fill out your copyright notice in the Description page of Project Settings.


#include "MySpawnPawn.h"
#include "Components\BoxComponent.h"
#include "Engine\Classes\Kismet\KismetMathLibrary.h"
#include "Engine\Classes\Engine\World.h"
#include "MyCube.h"
#include <InputCoreTypes.h>
#include "Kismet/GameplayStatics.h"
// Sets default values
AMySpawnPawn::AMySpawnPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// The spawing space
	SpawnVloume = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnVloume"));
	SpawnVloume->InitBoxExtent(FVector(400.0f, 400.0f, 400.0f));
	RootComponent = SpawnVloume;
}

// Called when the game starts or when spawned
void AMySpawnPawn::BeginPlay()
{
	Super::BeginPlay();
	
	// Spawning();
}

// Called every frame
void AMySpawnPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	APlayerController* PlayerControl = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (PlayerControl->IsInputKeyDown(EKeys::LeftControl))
	{
		Spawning();
		// GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("LEFT CONRTOL"));
	}
}

// Called to bind functionality to input
void AMySpawnPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);
	
	// Bind spawing events
	PlayerInputComponent->BindAction("Spawning", IE_Pressed, this, &AMySpawnPawn::Spawning);

}

FVector AMySpawnPawn::GetSpawnPoint()
{
	FVector Extent = SpawnVloume->GetScaledBoxExtent();
	FVector Origin = SpawnVloume->GetComponentLocation();

	FVector Point = UKismetMathLibrary::RandomPointInBoundingBox(Origin, Extent);
	return Point;
}

void AMySpawnPawn::Spawning()
{
	UWorld* MyWorld = GetWorld();

	if (MyWorld)
	{
		FVector Location = GetSpawnPoint();
		MyWorld->SpawnActor<AActor>(AMyCube::StaticClass(), Location, FRotator(0.f));
	}
}