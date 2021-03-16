#include "OnlineSubsystem.h"
#include "Public/OnlineSubsystem.h"

#include "AutomatedAssetImportData.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Kismet/GameplayStatics.h"

OnlineSubsystem::OnlineSubsystem(UWorld* world) :
m_World{world}
{
	check(world);
	
	OnCreateSessionCompleteInternalDelegate = FOnCreateSessionCompleteDelegate::CreateRaw(this, &OnlineSubsystem::OnCreateSessionComplete);
	OnStartSessionCompleteInternalDelegate = FOnStartSessionCompleteDelegate::CreateRaw(this, &OnlineSubsystem::OnStartOnlineGameComplete);
	OnFindSessionsCompleteInternalDelegate = FOnFindSessionsCompleteDelegate::CreateRaw(this, &OnlineSubsystem::OnFindSessionsComplete);
	OnJoinSessionCompleteInternalDelegate = FOnJoinSessionCompleteDelegate::CreateRaw(this, &OnlineSubsystem::OnJoinSessionComplete);
	OnDestroySessionCompleteInternalDelegate = FOnDestroySessionCompleteDelegate::CreateRaw(this, &OnlineSubsystem::OnDestroySessionComplete);
}

bool OnlineSubsystem::CreateAndStartSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers)
{
	IOnlineSessionPtr Sessions = GetSession();

	if (!Sessions || !UserId.IsValid())
	{
		return false;
	}

	SessionSettings = MakeShareable(new FOnlineSessionSettings());

	SessionSettings->bIsLANMatch = bIsLAN;
	SessionSettings->bUsesPresence = bIsPresence;
	SessionSettings->NumPublicConnections = MaxNumPlayers;
	SessionSettings->NumPrivateConnections = 0;
	SessionSettings->bAllowInvites = true;
	SessionSettings->bAllowJoinInProgress = true;
	SessionSettings->bShouldAdvertise = true;
	SessionSettings->bAllowJoinViaPresence = true;
	SessionSettings->bAllowJoinViaPresenceFriendsOnly = false;

//	const FString mapName = m_LobbyMap.ToString();
//	SessionSettings->Set(SETTING_MAPNAME, mapName, EOnlineDataAdvertisementType::ViaOnlineService);

	// Set the delegate to the Handle of the SessionInterface
	OnCreateSessionCompleteInternalDelegateHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteInternalDelegate);

	// Our delegate should get called when this is complete (doesn't need to be successful!)
	return Sessions->CreateSession(*UserId, SessionName, *SessionSettings);
}

void OnlineSubsystem::FindSessions(TSharedPtr<const FUniqueNetId> UserId, bool bIsLAN, bool bIsPresence)
{
	IOnlineSessionPtr Sessions = GetSession();

	if (!Sessions || !UserId.IsValid())
	{
		OnFindSessionsComplete(false);
		return;
	}
		
	/*
	Fill in all the SearchSettings, like if we are searching for a LAN game and how many results we want to have!
	*/
	SessionSearch = MakeShareable(new FOnlineSessionSearch());

	SessionSearch->bIsLanQuery = bIsLAN;
	SessionSearch->MaxSearchResults = 20;
	SessionSearch->PingBucketSize = 50;
	
	// We only want to set this Query Setting if "bIsPresence" is true
	if (bIsPresence)
	{
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, bIsPresence, EOnlineComparisonOp::Equals);
	}

	TSharedRef<FOnlineSessionSearch> SearchSettingsRef = SessionSearch.ToSharedRef();

	// Set the Delegate to the Delegate Handle of the FindSession function
	OnFindSessionsCompleteDelegateHandle = Sessions->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteInternalDelegate);
	
	// Finally call the SessionInterface function. The Delegate gets called once this is finished
	Sessions->FindSessions(*UserId, SearchSettingsRef);
}

bool OnlineSubsystem::JoinSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FString& SessionId)
{
	IOnlineSessionPtr Sessions = GetSession();

	if (!Sessions || !UserId.IsValid())
	{
		return false;
	}

	// Set the Handle again
	OnJoinSessionCompleteDelegateHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteInternalDelegate);

	FOnlineSessionSearchResult searchResult;
	if(FillWithSessionBySessionId(SessionId, searchResult))
	{
		return Sessions->JoinSession(*UserId, SessionName, searchResult);
	}
	
	return false;
}

void OnlineSubsystem::DestroySession(FName SessionName) const
{
	IOnlineSessionPtr Sessions = GetSession();

	if (!Sessions)
	{
		return;
	}
	
	Sessions->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteInternalDelegate);
	Sessions->DestroySession(SessionName);
}

IOnlineSessionPtr OnlineSubsystem::GetSession() const
{
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();

	if (!OnlineSub)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("No OnlineSubsytem found!"));
		return nullptr;
	}
	
	IOnlineSessionPtr sessions = OnlineSub->GetSessionInterface();

	if (sessions.IsValid())
	{
		return sessions;
	}
	
	return nullptr;
}

bool OnlineSubsystem::FillWithSessionBySessionId(const FString& sessionId, FOnlineSessionSearchResult& searchResult) const
{
	for (auto&& session : SessionSearch->SearchResults)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Session: %s"), *SessionSearch->SearchResults[i].Session.OwningUserName));

		if (session.Session.GetSessionIdStr() == sessionId)
		{
			searchResult = session;
			return true;
		}
	}
	
	return false;
}

void OnlineSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	IOnlineSessionPtr Sessions = GetSession();

	if (Sessions)
	{
		Sessions->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteInternalDelegateHandle);
		if (bWasSuccessful)
		{
			OnStartSessionCompleteDelegateHandle = Sessions->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteInternalDelegate);
			Sessions->StartSession(SessionName);
			return;
		}
	}
		
	if(m_CreateAndStartSessionComplete.IsBound())
	{
		m_CreateAndStartSessionComplete.Broadcast(SessionName, false);
	}
}

void OnlineSubsystem::OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful)
{
	IOnlineSessionPtr Sessions = GetSession();

	if (Sessions)
	{
		Sessions->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);	
	}
	if(m_CreateAndStartSessionComplete.IsBound())
	{
		m_CreateAndStartSessionComplete.Broadcast(SessionName, bWasSuccessful);
	}
}

void OnlineSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
	IOnlineSessionPtr Sessions = GetSession();

	if (Sessions)
	{
		Sessions->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
	}
	if(m_FindSessionsComplete.IsBound())
	{
		m_FindSessionsComplete.Broadcast(SessionSearch, bWasSuccessful);
	}
}

void OnlineSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("OnJoinSessionComplete %s, %d"), *SessionName.ToString(), static_cast<int32>(Result)));

	// Get the OnlineSubsystem we want to work with
	IOnlineSessionPtr Sessions = GetSession();

	if (!Sessions)
	{
		return;
	}
		
	// Clear the Delegate again
	Sessions->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);

	// Get the first local PlayerController, so we can call "ClientTravel" to get to the Server Map
	// This is something the Blueprint Node "Join Session" does automatically!
	APlayerController * const PlayerController = m_World->GetGameInstance()->GetFirstLocalPlayerController();

	// We need a FString to use ClientTravel and we can let the SessionInterface contruct such a
	// String for us by giving him the SessionName and an empty String. We want to do this, because
	// Every OnlineSubsystem uses different TravelURLs
	FString TravelURL;

	if (PlayerController && Sessions->GetResolvedConnectString(SessionName, TravelURL))
	{
		// Finally call the ClienTravel. If you want, you could print the TravelURL to see
		// how it really looks like
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, FString::Printf(TEXT("OnJoinSessionComplete Travelling to %s"), *TravelURL));

		PlayerController->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
	}
}

void OnlineSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	IOnlineSessionPtr Sessions = GetSession();

	if (Sessions)
	{
		Sessions->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
	}

	UE_LOG(LogNet, Display, TEXT("OnlineSubsystem::OnDestroySessionComplete is Bound ? %d"),m_DestroySessionComplete.IsBound());
	if(m_DestroySessionComplete.IsBound())
	{
		m_DestroySessionComplete.Broadcast(SessionName, bWasSuccessful);
	}
}