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
	UE_LOG(LogNet, Display, TEXT("OnlineSubsystem::OnlineSubsystem"));
	
	OnCreateSessionCompleteInternalDelegate = FOnCreateSessionCompleteDelegate::CreateRaw(this, &OnlineSubsystem::OnCreateSessionComplete);
	OnStartSessionCompleteInternalDelegate = FOnStartSessionCompleteDelegate::CreateRaw(this, &OnlineSubsystem::OnStartOnlineGameComplete);
	OnFindSessionsCompleteInternalDelegate = FOnFindSessionsCompleteDelegate::CreateRaw(this, &OnlineSubsystem::OnFindSessionsComplete);
	OnJoinSessionCompleteInternalDelegate = FOnJoinSessionCompleteDelegate::CreateRaw(this, &OnlineSubsystem::OnJoinSessionComplete);
	OnDestroySessionCompleteInternalDelegate = FOnDestroySessionCompleteDelegate::CreateRaw(this, &OnlineSubsystem::OnDestroySessionComplete);
}

OnlineSubsystem::~OnlineSubsystem()
{
	OnCreateSessionCompleteInternalDelegate.Unbind();
	OnStartSessionCompleteInternalDelegate.Unbind();
	OnFindSessionsCompleteInternalDelegate.Unbind();
	OnJoinSessionCompleteInternalDelegate.Unbind();
	OnDestroySessionCompleteInternalDelegate.Unbind();
	
	UE_LOG(LogNet, Display, TEXT("OnlineSubsystem::~OnlineSubsystem"));
}

bool OnlineSubsystem::CreateAndStartSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers)
{
	UE_LOG(LogNet, Display, TEXT("OnlineSubsystem::CreateAndStartSession"));
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

	//const FString mapName = "LobbyMap";//m_LobbyMap.ToString();
	//SessionSettings->Set(SETTING_MAPNAME, mapName, EOnlineDataAdvertisementType::ViaOnlineService);

	OnCreateSessionCompleteInternalDelegateHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteInternalDelegate);

	return Sessions->CreateSession(*UserId, SessionName, *SessionSettings);
}

void OnlineSubsystem::FindSessions(TSharedPtr<const FUniqueNetId> UserId, bool bIsLAN, bool bIsPresence)
{
	UE_LOG(LogNet, Display, TEXT("OnlineSubsystem::FindSessions"));
	IOnlineSessionPtr Sessions = GetSession();

	if (!Sessions || !UserId.IsValid())
	{
		OnFindSessionsComplete(false);
		return;
	}
	
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = bIsLAN;
	SessionSearch->MaxSearchResults = 20;
	SessionSearch->PingBucketSize = 50;
	
	if (bIsPresence)
	{
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, bIsPresence, EOnlineComparisonOp::Equals);
	}

	TSharedRef<FOnlineSessionSearch> SearchSettingsRef = SessionSearch.ToSharedRef();
	OnFindSessionsCompleteDelegateHandle = Sessions->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteInternalDelegate);
	Sessions->FindSessions(*UserId, SearchSettingsRef);
}

bool OnlineSubsystem::JoinSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FString& SessionId)
{
	UE_LOG(LogNet, Display, TEXT("OnlineSubsystem::JoinSession"));
	IOnlineSessionPtr Sessions = GetSession();

	if (!Sessions || !UserId.IsValid())
	{
		return false;
	}

	OnJoinSessionCompleteDelegateHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteInternalDelegate);

	FOnlineSessionSearchResult searchResult;
	if(FillWithSessionBySessionId(SessionId, searchResult))
	{
		return Sessions->JoinSession(*UserId, SessionName, searchResult);
	}
	
	return false;
}

void OnlineSubsystem::DestroySession(FName SessionName)
{
	UE_LOG(LogNet, Display, TEXT("OnlineSubsystem::DestroySession"));
	IOnlineSessionPtr Sessions = GetSession();

	if (!Sessions)
	{
		return;
	}
	
	OnDestroySessionCompleteDelegateHandle = Sessions->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteInternalDelegate);
	Sessions->DestroySession(SessionName);
}

IOnlineSessionPtr OnlineSubsystem::GetSession() const
{
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();

	if (!OnlineSub)
	{
		UE_LOG(LogNet, Error, TEXT("OnlineSubsystem::GetSession No OnlineSubsystem found!"));
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("No OnlineSubsytem found!"));
		return nullptr;
	}
	
	IOnlineSessionPtr sessions = OnlineSub->GetSessionInterface();

	if (sessions.IsValid())
	{
		return sessions;
	}
	UE_LOG(LogNet, Error, TEXT("OnlineSubsystem::GetSession Return NULLPTR"));
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
	UE_LOG(LogNet, Display, TEXT("OnlineSubsystem::OnCreateSessionComplete"));
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
	UE_LOG(LogNet, Display, TEXT("OnlineSubsystem::OnStartOnlineGameComplete"));
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
	UE_LOG(LogNet, Display, TEXT("OnlineSubsystem::OnFindSessionsComplete"));
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
	UE_LOG(LogNet, Display, TEXT("OnlineSubsystem::OnJoinSessionComplete"));
	FString travelURL;
	bool bWasSuccessful = false;
	
	IOnlineSessionPtr Sessions = GetSession();
	if (Sessions)
	{
		Sessions->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);

		if(m_JoinSessionComplete.IsBound())
		{
			Sessions->GetResolvedConnectString(SessionName, travelURL);
			bWasSuccessful = true;
		}
	}

	if(m_JoinSessionComplete.IsBound())
	{
		m_JoinSessionComplete.Broadcast(travelURL, bWasSuccessful);
	}
}

void OnlineSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogNet, Display, TEXT("OnlineSubsystem::OnDestroySessionComplete"));
	IOnlineSessionPtr Sessions = GetSession();
	if (Sessions)
	{
		Sessions->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
	}
	if(m_DestroySessionComplete.IsBound())
	{
		m_DestroySessionComplete.Broadcast(SessionName, bWasSuccessful);
	}
}