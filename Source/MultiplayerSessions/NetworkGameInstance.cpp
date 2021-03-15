// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkGameInstance.h"
#include "OnlineSessionSettings.h"


const FName LobbyMap("LobbyMap");

UNetworkGameInstance::UNetworkGameInstance(const FObjectInitializer& ObjectInitializer) :
Super(ObjectInitializer)
{
}

void UNetworkGameInstance::InitializeOnlineSubsystem()
{
	auto world = GetWorld();
	m_OnlineSubsystem = MakeShareable(new OnlineSubsystem(world, LobbyMap));
}

void UNetworkGameInstance::StartGameInstance()
{
	Super::StartGameInstance();
	
	InitializeOnlineSubsystem();
}

FGameInstancePIEResult UNetworkGameInstance::StartPlayInEditorGameInstance(ULocalPlayer* localPlayer,
                                                                           const FGameInstancePIEParameters& params)
{
	InitializeOnlineSubsystem();

	return Super::StartPlayInEditorGameInstance(localPlayer, params);
}

void UNetworkGameInstance::StartOnlineGame()
{
	// Creating a local player where we can get the UserID from
	ULocalPlayer* const Player = GetFirstGamePlayer();
	
	// Call our custom HostSession function. GameSessionName is a GameInstance variable	
	m_OnlineSubsystem->HostSession(Player->GetPreferredUniqueNetId().GetUniqueNetId(), GameSessionName, true, true, 4);
}

void UNetworkGameInstance::FindOnlineGames()
{
	ULocalPlayer* const Player = GetFirstGamePlayer();

	m_OnlineSubsystem->FindSessions(Player->GetPreferredUniqueNetId().GetUniqueNetId(), true, true);
}

void UNetworkGameInstance::JoinOnlineGame()
{
	ULocalPlayer* const Player = GetFirstGamePlayer();

	FOnlineSessionSearchResult SearchResult;
	if(m_OnlineSubsystem->FillWithSession(Player, SearchResult))
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Session connecting: %s"), *SearchResult.Session.OwningUserName));
		m_OnlineSubsystem->JoinSession(Player->GetPreferredUniqueNetId().GetUniqueNetId(), GameSessionName, SearchResult);
	}
}

void UNetworkGameInstance::DestroySessionAndLeaveGame()
{
	const FString sessionName(LexToString(GameSessionName));
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Destroying Session: %s"), *sessionName));
		
	m_OnlineSubsystem->DestroySessionAndLeaveGame(GameSessionName);
}