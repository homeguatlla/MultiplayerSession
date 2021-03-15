#include "OnlineSubsystem.h"
#include "Public/OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Kismet/GameplayStatics.h"

OnlineSubsystem::OnlineSubsystem(UWorld* world, FName lobbyMap) :
m_World{world},
m_LobbyMap{lobbyMap}
{
	check(world);
	
	OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateRaw(this, &OnlineSubsystem::OnCreateSessionComplete);
	OnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateRaw(this, &OnlineSubsystem::OnStartOnlineGameComplete);
	OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateRaw(this, &OnlineSubsystem::OnFindSessionsComplete);
	OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateRaw(this, &OnlineSubsystem::OnJoinSessionComplete);
	OnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateRaw(this, &OnlineSubsystem::OnDestroySessionComplete);
}

IOnlineSessionPtr OnlineSubsystem::GetSession() const
{
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();

	if (!OnlineSub)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("No OnlineSubsytem found!"));
		return nullptr;
	}
	
	// Get the Session Interface, so we can call the "CreateSession" function on it
	IOnlineSessionPtr sessions = OnlineSub->GetSessionInterface();

	if (sessions.IsValid())
	{
		return sessions;
	}
	
	return nullptr;
}

bool OnlineSubsystem::HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers)
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

	const FString mapName = m_LobbyMap.ToString();
	SessionSettings->Set(SETTING_MAPNAME, mapName, EOnlineDataAdvertisementType::ViaOnlineService);

	// Set the delegate to the Handle of the SessionInterface
	OnCreateSessionCompleteDelegateHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);

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
	OnFindSessionsCompleteDelegateHandle = Sessions->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);
	
	// Finally call the SessionInterface function. The Delegate gets called once this is finished
	Sessions->FindSessions(*UserId, SearchSettingsRef);
}

bool OnlineSubsystem::JoinSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FOnlineSessionSearchResult& SearchResult)
{
	IOnlineSessionPtr Sessions = GetSession();

	if (!Sessions || !UserId.IsValid())
	{
		OnFindSessionsComplete(false);
		return false;
	}

	// Set the Handle again
	OnJoinSessionCompleteDelegateHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);
	
	// Call the "JoinSession" Function with the passed "SearchResult". The "SessionSearch->SearchResults" can be used to get such a
	// "FOnlineSessionSearchResult" and pass it. Pretty straight forward!
	return Sessions->JoinSession(*UserId, SessionName, SearchResult);
}

void OnlineSubsystem::DestroySessionAndLeaveGame(FName SessionName) const
{
	IOnlineSessionPtr Sessions = GetSession();

	if (!Sessions)
	{
		return;
	}
	
	Sessions->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);
	Sessions->DestroySession(SessionName);
}

bool OnlineSubsystem::FillWithSession(ULocalPlayer* player, FOnlineSessionSearchResult& searchResult) const
{
	check(player);

	for (int32 i = 0; i < SessionSearch->SearchResults.Num(); i++)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Session: %s"), *SessionSearch->SearchResults[i].Session.OwningUserName));

		// To avoid something crazy, we filter sessions from ourself
		if (SessionSearch->SearchResults[i].Session.OwningUserId != player->GetPreferredUniqueNetId())
		{
			searchResult = SessionSearch->SearchResults[i];
			return true;
		}
	}

	return false;
}

void OnlineSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	const auto color = bWasSuccessful ? FColor::Green : FColor::Red;
	GEngine->AddOnScreenDebugMessage(-1, 10.f, color, FString::Printf(TEXT("OnCreateSessionComplete %s, %d"), *SessionName.ToString(), bWasSuccessful));

	IOnlineSessionPtr Sessions = GetSession();

	if (!Sessions)
	{
		return;
	}
		
	// Clear the SessionComplete delegate handle, since we finished this call
	Sessions->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
	if (bWasSuccessful)
	{
		// Set the StartSession delegate handle
		OnStartSessionCompleteDelegateHandle = Sessions->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegate);

		// Our StartSessionComplete delegate should get called after this
		Sessions->StartSession(SessionName);
	}
}

void OnlineSubsystem::OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful)
{
	const auto color = bWasSuccessful ? FColor::Green : FColor::Red;
	GEngine->AddOnScreenDebugMessage(-1, 10.f, color, FString::Printf(TEXT("OnStartSessionComplete %s, %d"), *SessionName.ToString(), bWasSuccessful));

	IOnlineSessionPtr Sessions = GetSession();

	if (!Sessions)
	{
		return;
	}
		
	// Clear the delegate, since we are done with this call
	Sessions->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);

	// If the start was successful, we can open a LobbyMap if we want. Make sure to use "listen" as a parameter!
	if (bWasSuccessful)
	{
		UGameplayStatics::OpenLevel(m_World, m_LobbyMap, true, "listen");
	}
}

void OnlineSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
	const auto color = bWasSuccessful ? FColor::Green : FColor::Red;
	GEngine->AddOnScreenDebugMessage(-1, 10.f, color, FString::Printf(TEXT("OFindSessionsComplete bSuccess: %d"), bWasSuccessful));

	// Get OnlineSubsystem we want to work with
	IOnlineSessionPtr Sessions = GetSession();

	if (!Sessions)
	{
		return;
	}
	
	// Clear the Delegate handle, since we finished this call
	Sessions->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);

	// Just debugging the Number of Search results. Can be displayed in UMG or something later on
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, FString::Printf(TEXT("Num Search Results: %d"), SessionSearch->SearchResults.Num()));

	// If we have found at least 1 session, we just going to debug them. You could add them to a list of UMG Widgets, like it is done in the BP version!
	if (SessionSearch->SearchResults.Num() > 0)
	{
		// "SessionSearch->SearchResults" is an Array that contains all the information. You can access the Session in this and get a lot of information.
		// This can be customized later on with your own classes to add more information that can be set and displayed
		for (int32 SearchIdx = 0; SearchIdx < SessionSearch->SearchResults.Num(); SearchIdx++)
		{
			// OwningUserName is just the SessionName for now. I guess you can create your own Host Settings class and GameSession Class and add a proper GameServer Name here.
			// This is something you can't do in Blueprint for example!
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, FString::Printf(TEXT("Session Number: %d | Sessionname: %s "), SearchIdx, *(SessionSearch->SearchResults[SearchIdx].Session.OwningUserName)));
		}
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
	const auto color = bWasSuccessful ? FColor::Green : FColor::Red;
	GEngine->AddOnScreenDebugMessage(-1, 10.f, color, FString::Printf(TEXT("OnDestroySessionComplete %s, %d"), *SessionName.ToString(), bWasSuccessful));

	IOnlineSessionPtr Sessions = GetSession();

	if (!Sessions)
	{
		return;
	}
	
	// Clear the Delegate
	Sessions->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);

	// If it was successful, we just load another level (could be a MainMenu!)
	if (bWasSuccessful)
	{
		UGameplayStatics::OpenLevel(m_World, "ThirdPersonExampleMap", true);
	}
}