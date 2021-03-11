// Copyright Epic Games, Inc. All Rights Reserved.

#include "MultiplayerSessionsGameMode.h"
#include "MultiplayerSessionsCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMultiplayerSessionsGameMode::AMultiplayerSessionsGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
