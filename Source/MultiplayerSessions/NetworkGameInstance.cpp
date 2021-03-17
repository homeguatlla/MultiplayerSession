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
	UE_LOG(LogTemp, Error, TEXT("UNetworkGameInstance::HandleNetworkFailure error: %s"), *ErrorString);
}

void UNetworkGameInstance::InitializeOnlineSubsystem()
{
	m_OnlineSubsystem =  MakeShareable(new OnlineSubsystem(GetWorld()));
	OnCreateSessionCompleteDelegateHandle = m_OnlineSubsystem->OnCreateAndStartSessionCompleteDelegate().AddUObject(this, &UNetworkGameInstance::OnCreateAndStartSessionComplete);
	OnDestroySessionCompleteDelegateHandle = m_OnlineSubsystem->OnDestroySessionCompleteDelegate().AddUObject(this, &UNetworkGameInstance::OnDestroySessionComplete);
	OnFindSessionsCompleteDelegateHandle = m_OnlineSubsystem->OnFindSessionsCompleteDelegate().AddUObject(this, &UNetworkGameInstance::OnFindSessionsComplete);
	OnJoinSessionCompleteDelegateHandle = m_OnlineSubsystem->OnJoinSessionCompleteDelegate().AddUObject(this, &UNetworkGameInstance::OnJoinSessionComplete);
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
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UNetworkGameInstance::OnCreateAndStartSessionComplete not successful"));
	}
}

void UNetworkGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful) const
{
	m_OnlineSubsystem->OnDestroySessionCompleteDelegate().Remove(OnDestroySessionCompleteDelegateHandle);
	
	if (bWasSuccessful)
	{
		UGameplayStatics::OpenLevel(GetWorld(), "ThirdPersonExampleMap", true);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UNetworkGameInstance::OnDestroySessionComplete not successful"));
	}
}

void UNetworkGameInstance::OnFindSessionsComplete(TSharedPtr<class FOnlineSessionSearch> Sessions, bool bWasSuccessful)
{
	m_OnlineSubsystem->OnFindSessionsCompleteDelegate().Remove(OnFindSessionsCompleteDelegateHandle);
	
	if(bWasSuccessful)
	{
		for(auto&& session : Sessions->SearchResults)
		{
			UE_LOG(LogTemp, Display, TEXT("UNetworkGameInstance::OnFindSessionsComplete session id = %s"), *session.GetSessionIdStr());
		}
		m_SessionIdToFound = Sessions->SearchResults[0].GetSessionIdStr();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UNetworkGameInstance::OnFindSessionsComplete not successful"));
	}
}

void UNetworkGameInstance::OnJoinSessionComplete(const FString& travelURL, bool bWasSuccessful)
{
	m_OnlineSubsystem->OnJoinSessionCompleteDelegate().Remove(OnJoinSessionCompleteDelegateHandle);
	//ULocalPlayer* const Player = GetFirstGamePlayer();
	if(bWasSuccessful)
	{
		UE_LOG(LogTemp, Display, TEXT("UNetworkGameInstance::OnJoinSessionComplete travel to = %s"), *travelURL);
		auto playerController = GetFirstLocalPlayerController();
		if(playerController)
		{
			playerController->ClientTravel(travelURL, ETravelType::TRAVEL_Absolute);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UNetworkGameInstance::OnJoinSessionComplete playercontroller null"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UNetworkGameInstance::OnJoinSessionComplete not successful"));
	}
}
