﻿#pragma once
#include "Interfaces/OnlineSessionInterface.h"

class OnlineSubsystem
{
public:
	OnlineSubsystem(UWorld* world);
	virtual ~OnlineSubsystem();

	/*
	 * bIsPresence: Setting the way that a user's online status will appear to other users, such as "Online", "Away", "Playing a game", and so on.
	 */
	bool CreateSession(TSharedPtr<const FUniqueNetId> userId, FName sessionName, bool isLan, bool isPresence, int32 maxNumPlayers);
	void StartSession(FName sessionName);
	void EndSession(FName sessionName);
	void FindSessions(TSharedPtr<const FUniqueNetId> userId, bool isLan, bool isPresence);
	bool JoinSession(TSharedPtr<const FUniqueNetId> userId, FName sessionName,  const FString& sessionId);
	void DestroySession(FName sessionName);
	FString GetSubsystemName() const;

	/** Handles to various registered delegates */
	DECLARE_EVENT_TwoParams(OnlineSubsystem, FOnCreateSessionComplete, FName /*SessionName*/, bool /*bWasSuccessful*/);
	FOnCreateSessionComplete& OnCreateSessionCompleteDelegate() { return m_CreateSessionComplete; }
	DECLARE_EVENT_TwoParams(OnlineSubsystem, FOnStartSessionComplete, FName /*SessionName*/, bool /*bWasSuccessful*/);
	FOnStartSessionComplete& OnStartSessionCompleteDelegate() { return m_StartSessionComplete; }
	DECLARE_EVENT_TwoParams(OnlineSubsystem, FOnEndSessionComplete, FName /*SessionName*/, bool /*bWasSuccessful*/);
	FOnEndSessionComplete& OnEndSessionCompleteDelegate() { return m_EndSessionComplete; }
	DECLARE_EVENT_TwoParams(OnlineSubsystem, FOnDestroySessionComplete, FName /*SessionName*/, bool /*bWasSuccessful*/);
	FOnDestroySessionComplete& OnDestroySessionCompleteDelegate() { return m_DestroySessionComplete; }
	DECLARE_EVENT_TwoParams(OnlineSubsystem, FOnFindSessionsComplete, TSharedPtr<class FOnlineSessionSearch>, bool /*bWasSuccessful*/);
	FOnFindSessionsComplete& OnFindSessionsCompleteDelegate() { return m_FindSessionsComplete; }
	DECLARE_EVENT_TwoParams(OnlineSubsystem, FOnJoinSessionComplete, const FString& travelURL, EOnJoinSessionCompleteResult::Type /*result*/);
	FOnJoinSessionComplete& OnJoinSessionCompleteDelegate() { return m_JoinSessionComplete; }

private:
	IOnlineSessionPtr GetSession() const;
	bool FillWithSessionBySessionId(const FString& sessionId, FOnlineSessionSearchResult& searchResult) const;	
	void OnCreateSessionComplete(FName sessionName, bool wasSuccessful);
	void OnStartOnlineGameComplete(FName sessionName, bool wasSuccessful);
	void OnEndOnlineGameComplete(FName sessionName, bool wasSuccessful);
	void OnFindSessionsComplete(bool wasSuccessful);
	void OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result);
	void OnDestroySessionComplete(FName sessionName, bool wasSuccessful);

	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteInternalDelegate;
	FOnStartSessionCompleteDelegate OnStartSessionCompleteInternalDelegate;
	FOnEndSessionCompleteDelegate OnEndSessionCompleteInternalDelegate;
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteInternalDelegate;
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteInternalDelegate;    
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteInternalDelegate;
	
	FDelegateHandle OnCreateSessionCompleteInternalDelegateHandle;
	FDelegateHandle OnStartSessionCompleteDelegateHandle;
	FDelegateHandle OnEndSessionCompleteDelegateHandle;
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;

	FOnCreateSessionComplete m_CreateSessionComplete;
	FOnStartSessionComplete m_StartSessionComplete;
	FOnEndSessionComplete m_EndSessionComplete;
	FOnDestroySessionComplete m_DestroySessionComplete;
	FOnFindSessionsComplete m_FindSessionsComplete;
	FOnJoinSessionComplete m_JoinSessionComplete;
	
	TSharedPtr<class FOnlineSessionSettings> m_SessionSettings;
	TSharedPtr<class FOnlineSessionSearch> m_SessionSearch;
	UWorld* m_World;
};
