// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkGameInstance.h"
#include "OnlineSessionSettings.h"
#include "Kismet/GameplayStatics.h"


const FName LobbyMap("LobbyMap");

UNetworkGameInstance::UNetworkGameInstance(const FObjectInitializer& ObjectInitializer) :
Super(ObjectInitializer)
{
}

void UNetworkGameInstance::HandleNetworkFailure(UWorld * World, UNetDriver *NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString = TEXT(""))
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("HandleNetworkFailure: %s"), *ErrorString));
}

void UNetworkGameInstance::InitializeOnlineSubsystem()
{
	m_OnlineSubsystem = MakeShareable(new OnlineSubsystem(GetWorld()));
	m_OnlineSubsystem->OnCreateAndStartSessionCompleteDelegate().AddUObject(this, &UNetworkGameInstance::OnCreateAndStartSessionComplete);
}

void UNetworkGameInstance::StartGameInstance()
{
	Super::StartGameInstance();
	
	InitializeOnlineSubsystem();
	//GetEngine()->OnNetworkFailure().AddUObject(this, &UNetworkGameInstance::HandleNetworkFailure);
}

FGameInstancePIEResult UNetworkGameInstance::StartPlayInEditorGameInstance(ULocalPlayer* localPlayer,
                                                                           const FGameInstancePIEParameters& params)
{
	InitializeOnlineSubsystem();
	//GetEngine()->OnNetworkFailure().AddUObject(this, &UNetworkGameInstance::HandleNetworkFailure);
	return Super::StartPlayInEditorGameInstance(localPlayer, params);
}

void UNetworkGameInstance::CreateSession()
{
	// Creating a local player where we can get the UserID from
	ULocalPlayer* const Player = GetFirstGamePlayer();
	
	// Call our custom HostSession function. GameSessionName is a GameInstance variable	
	m_OnlineSubsystem->CreateAndStartSession(Player->GetPreferredUniqueNetId().GetUniqueNetId(), GameSessionName, true, true, 4);
}

void UNetworkGameInstance::FindSessions()
{
	ULocalPlayer* const Player = GetFirstGamePlayer();

	m_OnlineSubsystem->FindSessions(Player->GetPreferredUniqueNetId().GetUniqueNetId(), true, true);
}

void UNetworkGameInstance::JoinSession()
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

void UNetworkGameInstance::OnCreateAndStartSessionCompleteEvent_Implementation()
{
}

void UNetworkGameInstance::OnCreateAndStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		Execute_OnCreateAndStartSessionCompleteEvent(this);
		//Execute_OnCreateAndStartSessionCompleteEvent(this);
		UGameplayStatics::OpenLevel(GetWorld(), LobbyMap, true, "listen");
	}
}
