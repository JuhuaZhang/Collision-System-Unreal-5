// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPSProjectCharacter.h"
#include "FPSProjectProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Engine\Classes\Kismet\KismetMathLibrary.h"
#include "Engine\Classes\Engine\World.h"
#include "MyCube.h"
#include "EngineUtils.h"
#include "Engine/Engine.h"
#include "NewProjectile.h"
#include "GameFramework/PlayerController.h"
//////////////////////////////////////////////////////////////////////////
// AFPSProjectCharacter

AFPSProjectCharacter::AFPSProjectCharacter()
{
	// Enable dynamic res
	// GEngine->GetDynamicResolutionState()->SetEnabled(true);
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	TurnRateGamepad = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	count = 200;

	nums = 0;
}

void AFPSProjectCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

}

//////////////////////////////////////////////////////////////////////////// Input

void AFPSProjectCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("PrimaryAction", IE_Pressed, this, &AFPSProjectCharacter::OnPrimaryAction);

	// Bind Spawing Cube
	PlayerInputComponent->BindAction("Spawning", IE_Pressed, this, &AFPSProjectCharacter::Spawning);
	PlayerInputComponent->BindAction("Destroy", IE_Pressed, this, &AFPSProjectCharacter::Destroy);
	PlayerInputComponent->BindAction("Action3", IE_Pressed, this, &AFPSProjectCharacter::Action3);
	PlayerInputComponent->BindAction("Action4", IE_Pressed, this, &AFPSProjectCharacter::Action4);
	PlayerInputComponent->BindAction("Action5", IE_Pressed, this, &AFPSProjectCharacter::Action5);
	PlayerInputComponent->BindAction("Action6", IE_Pressed, this, &AFPSProjectCharacter::Action6);
	PlayerInputComponent->BindAction("Action7", IE_Pressed, this, &AFPSProjectCharacter::Action7);
	PlayerInputComponent->BindAction("Action8", IE_Pressed, this, &AFPSProjectCharacter::Action8);
	PlayerInputComponent->BindAction("Action9", IE_Pressed, this, &AFPSProjectCharacter::Action9);
	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	// Bind movement events
	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &AFPSProjectCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &AFPSProjectCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "Mouse" versions handle devices that provide an absolute delta, such as a mouse.
	// "Gamepad" versions are for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &AFPSProjectCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &AFPSProjectCharacter::LookUpAtRate);
}

void AFPSProjectCharacter::OnPrimaryAction()
{
	// Trigger the OnItemUsed Event
	OnUseItem.Broadcast();
}

void AFPSProjectCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnPrimaryAction();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AFPSProjectCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

void AFPSProjectCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AFPSProjectCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AFPSProjectCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AFPSProjectCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

bool AFPSProjectCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AFPSProjectCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &AFPSProjectCharacter::EndTouch);

		return true;
	}
	
	return false;
}

void AFPSProjectCharacter::Spawning()
{
	FVector Extent = FVector(800.f, 800.f, 300.f);
	FVector Origin = FVector(2500.f, 2500.f, 500.f);
	FVector RandomPoint = UKismetMathLibrary::RandomPointInBoundingBox(Origin, Extent);
	
	FRotator RandomRotation = UKismetMathLibrary::RandomRotator();

	FRandomStream rand;
	rand.GenerateNewSeed();
	FVector randVector = rand.GetUnitVector();

	float initspeed = 100.0f;

	UWorld* MyWorld = GetWorld();
	
	if (MyWorld)
	{
		// AMyCube* SpawnCube = MyWorld->SpawnActor<AMyCube>(AMyCube::StaticClass(), RandomPoint, FRotator(0.f));
		AMyCube* SpawnCube = MyWorld->SpawnActor<AMyCube>(AMyCube::StaticClass(), RandomPoint, RandomRotation);
		SpawnCube->GetRootComponent()->ComponentVelocity = initspeed*randVector;
	}
	nums++;
}

void AFPSProjectCharacter::Destroy()
{
	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		if (ActorItr->GetClass() == AMyCube::StaticClass())
		{
			ActorItr->Destroy();
		}
	}
	nums = 0;
}

// Cube Fireworks
void AFPSProjectCharacter::Action3()
{
	FVector Origin = FVector(1500.f, 1500.f, 500.f);

	float initspeed = 100.0f;
	float initspace = 60.0f;

	UWorld* MyWorld = GetWorld();

	if (MyWorld)
	{
		// Init eight cubes

		AMyCube* SpawnCube_1 = MyWorld->SpawnActor<AMyCube>(AMyCube::StaticClass(), Origin - FVector(initspace, initspace, initspace), FRotator(0.f));
		SpawnCube_1->GetRootComponent()->ComponentVelocity = -FVector(initspeed, initspeed, initspeed) * 0.8;

		AMyCube* SpawnCube_2 = MyWorld->SpawnActor<AMyCube>(AMyCube::StaticClass(), Origin - FVector(-initspace, initspace, initspace), FRotator(0.f));
		SpawnCube_2->GetRootComponent()->ComponentVelocity = -FVector(-initspeed, initspeed, initspeed) * 0.8;

		AMyCube* SpawnCube_3 = MyWorld->SpawnActor<AMyCube>(AMyCube::StaticClass(), Origin - FVector(initspace, -initspace, initspace), FRotator(0.f));
		SpawnCube_3->GetRootComponent()->ComponentVelocity = -FVector(initspeed, -initspeed, initspeed) * 0.8;

		AMyCube* SpawnCube_4 = MyWorld->SpawnActor<AMyCube>(AMyCube::StaticClass(), Origin - FVector(initspace, initspace, -initspace), FRotator(0.f));
		SpawnCube_4->GetRootComponent()->ComponentVelocity = -FVector(initspeed, initspeed, -initspeed);

		AMyCube* SpawnCube_5 = MyWorld->SpawnActor<AMyCube>(AMyCube::StaticClass(), Origin - FVector(-initspace, -initspace, initspace), FRotator(0.f));
		SpawnCube_5->GetRootComponent()->ComponentVelocity = -FVector(-initspeed, -initspeed, initspeed) * 0.8;

		AMyCube* SpawnCube_6 = MyWorld->SpawnActor<AMyCube>(AMyCube::StaticClass(), Origin - FVector(-initspace, initspace, -initspace), FRotator(0.f));
		SpawnCube_6->GetRootComponent()->ComponentVelocity = -FVector(-initspeed, initspeed, -initspeed);

		AMyCube* SpawnCube_7 = MyWorld->SpawnActor<AMyCube>(AMyCube::StaticClass(), Origin - FVector(initspace, -initspace, -initspace), FRotator(0.f));
		SpawnCube_7->GetRootComponent()->ComponentVelocity = -FVector(initspeed, -initspeed, -initspeed);

		AMyCube* SpawnCube_8 = MyWorld->SpawnActor<AMyCube>(AMyCube::StaticClass(), Origin - FVector(-initspace, -initspace, -initspace), FRotator(0.f));
		SpawnCube_8->GetRootComponent()->ComponentVelocity = -FVector(-initspeed, -initspeed, -initspeed);
	}
	nums += 8;
}

//stop action 9
// key 6
void AFPSProjectCharacter::Action4()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	
	//for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	//{
	//	if (ActorItr->GetClass() == AMyCube::StaticClass())
	//	{	
	//		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, ActorItr->GetRootComponent()->ComponentVelocity.ToString());
	//		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Speed:"));

	//		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, ActorItr->GetActorLocation().ToString());
	//		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Location:"));

	//		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, ActorItr->GetName());
	//	}
	//}
	//FString IntAsString = FString::FromInt(nums);
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, IntAsString);
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Cube Nums:"));
}

// Delete the oldest cube
void AFPSProjectCharacter::Action5()
{
	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		if (ActorItr->GetClass() == AMyCube::StaticClass())
		{
			ActorItr->Destroy();
			nums--;
			break;
		}
	}
}

// 10 random cube
void AFPSProjectCharacter::Action6()
{
	for (int i = 0; i < 10; i++)
	{
		Spawning();
	}
}

// Set all componet Speed 0
void AFPSProjectCharacter::Action7()
{
	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		if (ActorItr->GetClass() == AMyCube::StaticClass())
		{
			ActorItr->GetRootComponent()->ComponentVelocity = FVector(0.f, 0.f, 0.f);
		}
	}
}

// Set all componet Speed 0

void AFPSProjectCharacter::Action8()
{
	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		if (ActorItr->GetClass() == AMyCube::StaticClass())
		{
			ActorItr->GetRootComponent()->ComponentVelocity = FVector(0.f, 0.f, 0.f);
			
			FVector location = ActorItr->GetActorLocation();
			
			location.Z = 0;
			
			ActorItr->SetActorLocation(location);
		}
	}
}

void AFPSProjectCharacter::newSpawning()
{
	count++;
	nums++;
	FVector Extent = FVector(800.f, 800.f, 300.f);
	FVector Origin = FVector(2500.f, 2500.f, 500.f);
	FVector RandomPoint = UKismetMathLibrary::RandomPointInBoundingBox(Origin, Extent);

	FRotator RandomRotation = UKismetMathLibrary::RandomRotator();

	FRandomStream rand;
	rand.GenerateNewSeed();
	FVector randVector = rand.GetUnitVector();

	float initspeed = 100.0f;

	UWorld* MyWorld = GetWorld();

	if (MyWorld)
	{
		// AMyCube* SpawnCube = MyWorld->SpawnActor<AMyCube>(AMyCube::StaticClass(), RandomPoint, FRotator(0.f));
		AMyCube* SpawnCube = MyWorld->SpawnActor<AMyCube>(AMyCube::StaticClass(), RandomPoint, RandomRotation);
		SpawnCube->GetRootComponent()->ComponentVelocity = initspeed * randVector;
	}

	if (count == 1500)
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	}
}

// Create 200 Cubes 
// key:9
void AFPSProjectCharacter::Action9()
{
	//for (int i = 0; i < 200; i++)
	//{
	//	Spawning();
	//}

	//int iter = 0;
	count = 0;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AFPSProjectCharacter::newSpawning, 0.05, true);
	//GetWorldTimerManager().SetTimer(TimerHandle, this, &AFPSProjectCharacter::Spawning, 0.1, false);

	//for (int i = 0; i < 200; i++)
	//{
	//	Spawning();
	//	FTimerHandle TimerHandle;
	//	GetWorld()->GetTimerManager().SetTimer(TimerHandle, 0.1, false);
	//	// GetWorldTimerManager().SetTimer(TimerHandle, this, &AFPSProjectCharacter::Spawning, 0.1, false);
	//}
	
}