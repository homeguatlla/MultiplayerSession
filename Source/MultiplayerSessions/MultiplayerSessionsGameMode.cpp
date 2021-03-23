// Copyright Epic Games, Inc. All Rights Reserved.

#include "MultiplayerSessionsGameMode.h"

#include "MSGameSession.h"
#include "MultiplayerSessionsCharacter.h"
#include "UObject/ConstructorHelpers.h"


AMultiplayerSessionsGameMode::AMultiplayerSessionsGameMode()
{
	//https://docs.unrealengine.com/en-US/InteractiveExperiences/Networking/Travelling/index.html
	bUseSeamlessTravel = true;
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

TSubclassOf<AGameSession> AMultiplayerSessionsGameMode::GetGameSessionClass() const
{
	return AMSGameSession::StaticClass();
}

void AMultiplayerSessionsGameMode::SetMatchReadyToStart(bool isReady)
{
	auto gameSession = Cast<AMSGameSession>(GameSession);
	gameSession->SetMatchReadyToStart(isReady);
}

void AMultiplayerSessionsGameMode::GenericPlayerInitialization(AController* C)
{
	Super::GenericPlayerInitialization(C);
	
	//Put initialization code here like spawn positions etc once
	//the seamless travel has finished for each player
}
