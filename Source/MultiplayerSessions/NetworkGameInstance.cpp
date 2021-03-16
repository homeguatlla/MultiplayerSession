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
	m_OnlineSubsystem =  MakeShareable(new OnlineSubsystem(GetWorld()));
	OnCreateSessionCompleteDelegateHandle = m_OnlineSubsystem->OnCreateAndStartSessionCompleteDelegate().AddUObject(this, &UNetworkGameInstance::OnCreateAndStartSessionComplete);
	OnDestroySessionCompleteDelegateHandle = m_OnlineSubsystem->OnDestroySessionCompleteDelegate().AddUObject(this, &UNetworkGameInstance::OnDestroySessionComplete);
	OnFindSessionsCompleteDelegateHandle = m_OnlineSubsystem->OnFindSessionsCompleteDelegate().AddUObject(this, &UNetworkGameInstance::OnFindSessionsComplete);
}

void UNetworkGameInstance::StartGameInstance()
{
	Super::StartGameInstance();
	
	InitializeOnlineSubsystem();
	GetEngine()->OnNetworkFailure().AddUObject(this, &UNetworkGameInstance::HandleNetworkFailure);
}

FGameInstancePIEResult UNetworkGameInstance::StartPlayInEditorGameInstance(ULocalPlayer* localPlayer,
                                                                           const FGameInstancePIEParameters& params)
{
	InitializeOnlineSubsystem();
	GetEngine()->OnNetworkFailure().AddUObject(this, &UNetworkGameInstance::HandleNetworkFailure);
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

	m_SessionIdToFound.Empty();
	m_OnlineSubsystem->FindSessions(Player->GetPreferredUniqueNetId().GetUniqueNetId(), true, true);
}

void UNetworkGameInstance::JoinSession()
{
	ULocalPlayer* const Player = GetFirstGamePlayer();

	if(!m_SessionIdToFound.IsEmpty())
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Session connecting: %s"), *m_SessionIdToFound));
		m_OnlineSubsystem->JoinSession(Player->GetPreferredUniqueNetId().GetUniqueNetId(), GameSessionName, m_SessionIdToFound);
	}
}

void UNetworkGameInstance::DestroySessionAndLeaveGame()
{
	const FString sessionName(LexToString(GameSessionName));
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Destroying Session: %s"), *sessionName));
		
	m_OnlineSubsystem->DestroySession(GameSessionName);
}

void UNetworkGameInstance::OnCreateAndStartSessionComplete(FName SessionName, bool bWasSuccessful) const
{
	m_OnlineSubsystem->OnCreateAndStartSessionCompleteDelegate().Remove(OnCreateSessionCompleteDelegateHandle);
	
	if (bWasSuccessful)
	{
		UGameplayStatics::OpenLevel(GetWorld(), LobbyMap, true, "listen");
	}
}

void UNetworkGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful) const
{
	m_OnlineSubsystem->OnDestroySessionCompleteDelegate().Remove(OnDestroySessionCompleteDelegateHandle);
	
	if (bWasSuccessful)
	{
		UGameplayStatics::OpenLevel(GetWorld(), "ThirdPersonExampleMap", true);
	}
}

void UNetworkGameInstance::OnFindSessionsComplete(TSharedPtr<class FOnlineSessionSearch> Sessions, bool bWasSuccessful)
{
	m_OnlineSubsystem->OnFindSessionsCompleteDelegate().Remove(OnFindSessionsCompleteDelegateHandle);
	
	if(bWasSuccessful)
	{
		m_SessionIdToFound = Sessions->SearchResults[0].GetSessionIdStr();
	}
}