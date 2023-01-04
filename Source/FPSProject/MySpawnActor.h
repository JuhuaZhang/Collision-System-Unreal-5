// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MySpawnActor.generated.h"

UCLASS()
class FPSPROJECT_API AMySpawnActor : public AActor
{
	GENERATED_BODY()
	
public:	

	// restrict the spawing location
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* SpawnVloume;

	// Get a random number
	FVector GetSpawnPoint();

	void Spawning();
	// Sets default values for this actor's properties
	AMySpawnActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
