#pragma once
#include "Interfaces/OnlineSessionInterface.h"

class OnlineSubsystem
{
public:
	OnlineSubsystem(UWorld* world);
	virtual ~OnlineSubsystem();
	
	bool CreateAndStartSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers);
	void FindSessions(TSharedPtr<const FUniqueNetId> UserId, bool bIsLAN, bool bIsPresence);
	bool JoinSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName,  const FString& SessionId);
	void DestroySession(FName SessionName);

	/** Handles to various registered delegates */
	DECLARE_EVENT_TwoParams(OnlineSubsystem, FOnCreateAndStartSessionComplete, FName /*SessionName*/, bool /*bWasSuccessful*/);
	FOnCreateAndStartSessionComplete& OnCreateAndStartSessionCompleteDelegate() { return m_CreateAndStartSessionComplete; }
	DECLARE_EVENT_TwoParams(OnlineSubsystem, FOnDestroySessionComplete, FName /*SessionName*/, bool /*bWasSuccessful*/);
	FOnDestroySessionComplete& OnDestroySessionCompleteDelegate() { return m_DestroySessionComplete; }
	DECLARE_EVENT_TwoParams(OnlineSubsystem, FOnFindSessionsComplete, TSharedPtr<class FOnlineSessionSearch>, bool /*bWasSuccessful*/);
	FOnFindSessionsComplete& OnFindSessionsCompleteDelegate() { return m_FindSessionsComplete; }
	DECLARE_EVENT_TwoParams(OnlineSubsystem, FOnJoinSessionComplete, const FString& travelURL, bool /*bWasSuccessful*/);
	FOnJoinSessionComplete& OnJoinSessionCompleteDelegate() { return m_JoinSessionComplete; }

private:
	IOnlineSessionPtr GetSession() const;
	bool FillWithSessionBySessionId(const FString& sessionId, FOnlineSessionSearchResult& searchResult) const;	
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteInternalDelegate;
	FOnStartSessionCompleteDelegate OnStartSessionCompleteInternalDelegate;
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteInternalDelegate;
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteInternalDelegate;    
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteInternalDelegate;
	
	/** Handles to registered delegates for creating/starting a session */
	FDelegateHandle OnCreateSessionCompleteInternalDelegateHandle;
	FDelegateHandle OnStartSessionCompleteDelegateHandle;
	
	/** Handle to registered delegate for searching a session */
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;

	/** Handle to registered delegate for joining a session */
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;

	/** Handle to registered delegate for destroying a session */
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;

	FOnCreateAndStartSessionComplete m_CreateAndStartSessionComplete;
	FOnDestroySessionComplete m_DestroySessionComplete;
	FOnFindSessionsComplete m_FindSessionsComplete;
	FOnJoinSessionComplete m_JoinSessionComplete;
	
	TSharedPtr<class FOnlineSessionSettings> SessionSettings;
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	UWorld* m_World;
};
