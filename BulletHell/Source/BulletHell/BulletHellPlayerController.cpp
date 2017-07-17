// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "BulletHell.h"
#include "BulletHellPlayerController.h"
#include "AI/Navigation/NavigationSystem.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "BulletHellCharacter.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "BulletHellCharacter.h"




ABulletHellPlayerController::ABulletHellPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void ABulletHellPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// keep updating the destination every tick while desired

	MoveToMouseCursor();

}

void ABulletHellPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &ABulletHellPlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &ABulletHellPlayerController::OnSetDestinationReleased);



	//
	InputComponent->BindAxis("MoveForward", this, &ThisClass::MoveForward);
	InputComponent->BindAxis("MoveSides", this, &ThisClass::MoveSides);

}

void ABulletHellPlayerController::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ABulletHellPlayerController::MoveToMouseCursor()
{
	//Player Position on Screen
	ABulletHellCharacter* MyPawn = Cast<ABulletHellCharacter>(GetPawn());
	float xMouse;
	float yMouse;
	GetMousePosition(xMouse, yMouse);
	//Mouse position relative to player ----- Same axis
	FVector PlayerPos = MyPawn->GetActorLocation();
	FVector2D PlayerInTheScreen;
	ProjectWorldLocationToScreen(PlayerPos, PlayerInTheScreen);
	//Get mOUSE RELATIVE TO THE PLAYER 
	FVector2D Result;
	Result.X = -(yMouse - PlayerInTheScreen.Y);
	Result.Y = xMouse - PlayerInTheScreen.X;

	//  Get angle to rotate

	float angle = FMath::RadiansToDegrees(FMath::Acos(Result.X / Result.Size()));
	if (Result.Y < 0)
		angle = 360 - angle;

	FRotator rot(0, angle, 0);

	MyPawn->SetActorRotation(rot);




}

void ABulletHellPlayerController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	FVector2D ScreenSpaceLocation(Location);

	// Trace to see what is under the touch location
	FHitResult HitResult;
	GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult);
	if (HitResult.bBlockingHit)
	{
		// We hit something, move there
		SetNewMoveDestination(HitResult.ImpactPoint);
	}
}

void ABulletHellPlayerController::SetNewMoveDestination(const FVector DestLocation)
{
	APawn* const MyPawn = GetPawn();
	if (MyPawn)
	{
		UNavigationSystem* const NavSys = GetWorld()->GetNavigationSystem();
		float const Distance = FVector::Dist(DestLocation, MyPawn->GetActorLocation());

		// We need to issue move command only if far enough in order for walk animation to play correctly
		if (NavSys && (Distance > 120.0f))
		{
			NavSys->SimpleMoveToLocation(this, DestLocation);
		}
	}
}

void ABulletHellPlayerController::OnSetDestinationPressed()
{
	// set flag to keep updating destination until released
	bMoveToMouseCursor = true;
}

void ABulletHellPlayerController::OnSetDestinationReleased()
{
	// clear flag to indicate we should stop updating the destination
	bMoveToMouseCursor = false;
}


void ABulletHellPlayerController::MoveForward(float Value)
{
	APawn* const MyPawn = GetPawn();
	if (MyPawn)
	{
		const FRotator Rotation = MyPawn->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		MyPawn->AddMovementInput(Direction, Value);
		//	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		//	MyPawn->AddMovementInput(FVector(1,0,0), Value);
	}


}

void ABulletHellPlayerController::MoveSides(float Value)
{
	APawn* const MyPawn = GetPawn();
	if (MyPawn)
	{
		const FRotator Rotation = MyPawn->GetControlRotation();
		const FRotator YawRotation(Rotation.Yaw, 0, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		MyPawn->AddMovementInput(Direction, Value);

		//	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		//MyPawn->AddMovementInput(FVector(0, 1, 0), Value);
	}

}