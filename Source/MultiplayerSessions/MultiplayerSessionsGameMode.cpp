// Copyright Epic Games, Inc. All Rights Reserved.

#include "MultiplayerSessionsGameMode.h"

#include "MSGameSession.h"
#include "MultiplayerSessionsCharacter.h"
#include "NetworkGameInstance.h"
#include "OnlineSessionSettings.h"
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
	GetGameSession()->CreateSession(gameInstance->IsLan(), MaxNumPlayers, DefaultPlayerName.ToString());
}

void AMultiplayerSessionsGameMode::DestroySessionAndLeaveGame()
{
	GetGameSession()->DestroySessionAndLeaveGame();
}

TArray<FOnlineSessionFindResult> AMultiplayerSessionsGameMode::GetAvailableSessions() const
{
	TArray<FOnlineSessionFindResult> result;

	auto sessions = GetGameSession()->GetAvailableSessions();
	UE_LOG(LogTemp, Display, TEXT("AMultiplayerSessionsGameMode::GetAvailableSessions"));
			
	if(sessions.IsValid())
	{
		UE_LOG(LogTemp, Display, TEXT("AMultiplayerSessionsGameMode::GetAvailableSessions available sessions"));
	
		for(auto&& availableSession : sessions->SearchResults)
		{
			UE_LOG(LogTemp, Display, TEXT("AMultiplayerSessionsGameMode::GetAvailableSessions %s"), *availableSession.GetSessionIdStr());
			result.Push(FOnlineSessionFindResult(availableSession.GetSessionIdStr(), availableSession.Session.NumOpenPublicConnections));
		}
	}
	return result;
}

void AMultiplayerSessionsGameMode::StartGame()
{
	GetGameSession()->StartGame();
}

//This is not used because StartSession and EndSession are
//called automatically by the engine once the map is loaded
//but, let them here because are part of the session
//implementation
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

void AMultiplayerSessionsGameMode::JoinSession(const FString& sessionId)
{
	GetGameSession()->JoinSession(sessionId);
}

void AMultiplayerSessionsGameMode::GenericPlayerInitialization(AController* C)
{
	Super::GenericPlayerInitialization(C);
	UE_LOG(LogTemp, Display, TEXT("AMultiplayerSessionsGameMode::GenericPlayerInitialization"));
	
	//Put initialization code here like spawn positions etc once
	//the seamless travel has finished for each player
}
