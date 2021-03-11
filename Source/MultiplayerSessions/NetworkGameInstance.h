// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "NetworkGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UNetworkGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	
	UNetworkGameInstance(const FObjectInitializer& ObjectInitializer);	
	bool HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers);
	void FindSessions(TSharedPtr<const FUniqueNetId> UserId, bool bIsLAN, bool bIsPresence);
	bool JoinSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FOnlineSessionSearchResult& SearchResult);

	UFUNCTION(BlueprintCallable, Category = "Network|Test")
    void StartOnlineGame();
	UFUNCTION(BlueprintCallable, Category = "Network|Test")
	void FindOnlineGames();
	UFUNCTION(BlueprintCallable, Category = "Network|Test")
	void JoinOnlineGame();
	UFUNCTION(BlueprintCallable, Category = "Network|Test")
    void DestroySessionAndLeaveGame();
	
private:

	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	
	/* Delegate called when session created */
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	/* Delegate called when session started */
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;
	/** Handles to registered delegates for creating/starting a session */
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnStartSessionCompleteDelegateHandle;
	
	/** Delegate for searching for sessions */
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	/** Handle to registered delegate for searching a session */
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;

	/** Delegate for joining a session */
    FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;    
    /** Handle to registered delegate for joining a session */
    FDelegateHandle OnJoinSessionCompleteDelegateHandle;

	/** Delegate for destroying a session */
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;
	/** Handle to registered delegate for destroying a session */
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;
   
	TSharedPtr<class FOnlineSessionSettings> SessionSettings;
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;
};
