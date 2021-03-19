#include "OnlineSubsystem.h"
#include "Public/OnlineSubsystem.h"

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
	OnEndSessionCompleteInternalDelegate = FOnEndSessionCompleteDelegate::CreateRaw(this, &OnlineSubsystem::OnEndOnlineGameComplete);
	OnFindSessionsCompleteInternalDelegate = FOnFindSessionsCompleteDelegate::CreateRaw(this, &OnlineSubsystem::OnFindSessionsComplete);
	OnJoinSessionCompleteInternalDelegate = FOnJoinSessionCompleteDelegate::CreateRaw(this, &OnlineSubsystem::OnJoinSessionComplete);
	OnDestroySessionCompleteInternalDelegate = FOnDestroySessionCompleteDelegate::CreateRaw(this, &OnlineSubsystem::OnDestroySessionComplete);
}

OnlineSubsystem::~OnlineSubsystem()
{
	OnCreateSessionCompleteInternalDelegate.Unbind();
	OnStartSessionCompleteInternalDelegate.Unbind();
	OnEndSessionCompleteInternalDelegate.Unbind();
	OnFindSessionsCompleteInternalDelegate.Unbind();
	OnJoinSessionCompleteInternalDelegate.Unbind();
	OnDestroySessionCompleteInternalDelegate.Unbind();
	
	UE_LOG(LogNet, Display, TEXT("OnlineSubsystem::~OnlineSubsystem"));
}

bool OnlineSubsystem::CreateSession(TSharedPtr<const FUniqueNetId> userId, FName sessionName, bool isLan, bool isPresence, int32 maxNumPlayers)
{
	UE_LOG(LogNet, Display, TEXT("OnlineSubsystem::CreateSession"));
	IOnlineSessionPtr sessions = GetSession();

	if (!sessions || !userId.IsValid())
	{
		return false;
	}

	m_SessionSettings = MakeShareable(new FOnlineSessionSettings());

	m_SessionSettings->bIsLANMatch = isLan;
	m_SessionSettings->bUsesPresence = isPresence;
	m_SessionSettings->NumPublicConnections = maxNumPlayers;
	m_SessionSettings->NumPrivateConnections = 0;
	m_SessionSettings->bAllowInvites = true;
	m_SessionSettings->bAllowJoinInProgress = true;
	m_SessionSettings->bShouldAdvertise = true;
	m_SessionSettings->bAllowJoinViaPresence = true;
	m_SessionSettings->bAllowJoinViaPresenceFriendsOnly = false;

	OnCreateSessionCompleteInternalDelegateHandle = sessions->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteInternalDelegate);

	return sessions->CreateSession(*userId, sessionName, *m_SessionSettings);
}

void OnlineSubsystem::DestroySession(FName sessionName)
{
	UE_LOG(LogNet, Display, TEXT("OnlineSubsystem::DestroySession"));
	IOnlineSessionPtr sessions = GetSession();

	if (!sessions)
	{
		return;
	}
	
	OnDestroySessionCompleteDelegateHandle = sessions->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteInternalDelegate);
	sessions->DestroySession(sessionName);
}

void OnlineSubsystem::StartSession(FName sessionName)
{
	UE_LOG(LogNet, Display, TEXT("OnlineSubsystem::StartSession"));
	IOnlineSessionPtr sessions = GetSession();

	if (sessions)
	{
		OnStartSessionCompleteDelegateHandle = sessions->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteInternalDelegate);
		sessions->StartSession(sessionName);
	}
}

void OnlineSubsystem::EndSession(FName sessionName)
{
	UE_LOG(LogNet, Display, TEXT("OnlineSubsystem::EndSession"));
	IOnlineSessionPtr sessions = GetSession();

	if (sessions)
	{
		OnEndSessionCompleteDelegateHandle = sessions->AddOnEndSessionCompleteDelegate_Handle(OnEndSessionCompleteInternalDelegate);
		sessions->EndSession(sessionName);
	}
}

void OnlineSubsystem::FindSessions(TSharedPtr<const FUniqueNetId> userId, bool isLan, bool isPresence)
{
	UE_LOG(LogNet, Display, TEXT("OnlineSubsystem::FindSessions"));
	IOnlineSessionPtr sessions = GetSession();

	if (!sessions || !userId.IsValid())
	{
		OnFindSessionsComplete(false);
		return;
	}
	
	m_SessionSearch = MakeShareable(new FOnlineSessionSearch());
	m_SessionSearch->bIsLanQuery = isLan;
	m_SessionSearch->MaxSearchResults = 20;
	m_SessionSearch->PingBucketSize = 50;
	
	if (isPresence)
	{
		m_SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, isPresence, EOnlineComparisonOp::Equals);
	}

	const TSharedRef<FOnlineSessionSearch> SearchSettingsRef = m_SessionSearch.ToSharedRef();
	OnFindSessionsCompleteDelegateHandle = sessions->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteInternalDelegate);
	sessions->FindSessions(*userId, SearchSettingsRef);
}

bool OnlineSubsystem::JoinSession(TSharedPtr<const FUniqueNetId> userId, FName sessionName, const FString& sessionId)
{
	UE_LOG(LogNet, Display, TEXT("OnlineSubsystem::JoinSession"));
	IOnlineSessionPtr sessions = GetSession();

	if (!sessions || !userId.IsValid())
	{
		return false;
	}

	OnJoinSessionCompleteDelegateHandle = sessions->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteInternalDelegate);

	FOnlineSessionSearchResult searchResult;
	if(FillWithSessionBySessionId(sessionId, searchResult))
	{
		return sessions->JoinSession(*userId, sessionName, searchResult);
	}
	
	return false;
}

FString OnlineSubsystem::GetSubsystemName() const
{
	IOnlineSubsystem* const onlineSub = IOnlineSubsystem::Get();
	return onlineSub->GetSubsystemName().ToString();
}

IOnlineSessionPtr OnlineSubsystem::GetSession() const
{
	IOnlineSubsystem* const onlineSub = IOnlineSubsystem::Get();

	if (!onlineSub)
	{
		UE_LOG(LogNet, Error, TEXT("OnlineSubsystem::GetSession No OnlineSubsystem found!"));
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("No OnlineSubsytem found!"));
		return nullptr;
	}
	
	IOnlineSessionPtr sessions = onlineSub->GetSessionInterface();

	if (sessions.IsValid())
	{
		return sessions;
	}
	UE_LOG(LogNet, Error, TEXT("OnlineSubsystem::GetSession Return NULLPTR"));
	return nullptr;
}

bool OnlineSubsystem::FillWithSessionBySessionId(const FString& sessionId, FOnlineSessionSearchResult& searchResult) const
{
	check(m_SessionSearch);
	
	for (auto&& session : m_SessionSearch->SearchResults)
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

void OnlineSubsystem::OnCreateSessionComplete(FName sessionName, bool wasSuccessful)
{
	UE_LOG(LogNet, Display, TEXT("OnlineSubsystem::OnCreateSessionComplete"));
	IOnlineSessionPtr sessions = GetSession();

	if (sessions)
	{
		sessions->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteInternalDelegateHandle);
	}
		
	if(m_CreateSessionComplete.IsBound())
	{
		m_CreateSessionComplete.Broadcast(sessionName, wasSuccessful);
	}
}

void OnlineSubsystem::OnDestroySessionComplete(FName sessionName, bool wasSuccessful)
{
	UE_LOG(LogNet, Display, TEXT("OnlineSubsystem::OnDestroySessionComplete"));
	IOnlineSessionPtr sessions = GetSession();
	if (sessions)
	{
		sessions->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
	}
	if(m_DestroySessionComplete.IsBound())
	{
		m_DestroySessionComplete.Broadcast(sessionName, wasSuccessful);
	}
}

void OnlineSubsystem::OnStartOnlineGameComplete(FName sessionName, bool wasSuccessful)
{
	UE_LOG(LogNet, Display, TEXT("OnlineSubsystem::OnStartOnlineGameComplete"));
	IOnlineSessionPtr sessions = GetSession();

	if (sessions)
	{
		sessions->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);	
	}
	if(m_StartSessionComplete.IsBound())
	{
		m_StartSessionComplete.Broadcast(sessionName, wasSuccessful);
	}
}

void OnlineSubsystem::OnEndOnlineGameComplete(FName sessionName, bool wasSuccessful)
{
	UE_LOG(LogNet, Display, TEXT("OnlineSubsystem::OnEndOnlineGameComplete"));
	IOnlineSessionPtr sessions = GetSession();

	if (sessions)
	{
		sessions->ClearOnEndSessionCompleteDelegate_Handle(OnEndSessionCompleteDelegateHandle);	
	}
	if(m_EndSessionComplete.IsBound())
	{
		m_EndSessionComplete.Broadcast(sessionName, wasSuccessful);
	}
}

void OnlineSubsystem::OnFindSessionsComplete(bool wasSuccessful)
{
	UE_LOG(LogNet, Display, TEXT("OnlineSubsystem::OnFindSessionsComplete"));
	IOnlineSessionPtr sessions = GetSession();

	if (sessions)
	{
		sessions->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
	}
	if(m_FindSessionsComplete.IsBound())
	{
		m_FindSessionsComplete.Broadcast(m_SessionSearch, wasSuccessful);
	}
}

void OnlineSubsystem::OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result)
{
	UE_LOG(LogNet, Display, TEXT("OnlineSubsystem::OnJoinSessionComplete"));
	FString travelURL;
	
	IOnlineSessionPtr sessions = GetSession();
	if (sessions)
	{
		sessions->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
		if(result == EOnJoinSessionCompleteResult::Success)
		{
			sessions->GetResolvedConnectString(sessionName, travelURL);
		}
	}

	if(m_JoinSessionComplete.IsBound())
	{
		m_JoinSessionComplete.Broadcast(travelURL, result);
	}
}