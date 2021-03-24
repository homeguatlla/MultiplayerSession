// Copyright Epic Games, Inc. All Rights Reserved.

#include "MultiplayerSessionsGameMode.h"

#include "MSGameSession.h"
#include "MultiplayerSessionsCharacter.h"
#include "NetworkGameInstance.h"
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

TSubclassOf<AGameSession> AMultiplayerSessionsGameMode::GetGameSessionClass() const
{
	return AMSGameSession::StaticClass();
}

AMSGameSession* AMultiplayerSessionsGameMode::GetGameSession() const
{
	return Cast<AMSGameSession>(GameSession);
}

void AMultiplayerSessionsGameMode::CreateSession()
{
	const auto gameInstance = Cast<UNetworkGameInstance>(GetWorld()->GetGameInstance());
	GetGameSession()->CreateSession(gameInstance->IsLan());
}

void AMultiplayerSessionsGameMode::DestroySessionAndLeaveGame()
{
	GetGameSession()->DestroySessionAndLeaveGame();
}

void AMultiplayerSessionsGameMode::StartGame()
{
	GetGameSession()->StartGame();
}

//Useless
void AMultiplayerSessionsGameMode::StartSession()
{
	GetGameSession()->StartSession();
}

void AMultiplayerSessionsGameMode::EndSession()
{
	GetGameSession()->EndSession();
}

void AMultiplayerSessionsGameMode::FindSessions()
{
	GetGameSession()->FindSessions();
}

void AMultiplayerSessionsGameMode::JoinSession()
{
	GetGameSession()->JoinSession();
}

void AMultiplayerSessionsGameMode::GenericPlayerInitialization(AController* C)
{
	Super::GenericPlayerInitialization(C);
	UE_LOG(LogTemp, Display, TEXT("AMultiplayerSessionsGameMode::GenericPlayerInitialization"));
	
	//Put initialization code here like spawn positions etc once
	//the seamless travel has finished for each player
}
