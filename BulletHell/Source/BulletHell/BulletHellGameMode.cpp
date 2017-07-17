// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "BulletHell.h"
#include "BulletHellGameMode.h"
#include "BulletHellPlayerController.h"
#include "BulletHellCharacter.h"

ABulletHellGameMode::ABulletHellGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ABulletHellPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}