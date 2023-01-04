// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MySpawnPawn.generated.h"

UCLASS()
class FPSPROJECT_API AMySpawnPawn : public APawn
{
	GENERATED_BODY()

public:

	// restrict the spawing location
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* SpawnVloume;

	// Get a random number
	FVector GetSpawnPoint();

	// Spawning
	void Spawning();

	// Sets default values for this pawn's properties
	AMySpawnPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
