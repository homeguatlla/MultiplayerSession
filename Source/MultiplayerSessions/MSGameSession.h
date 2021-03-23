// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "MSGameSession.generated.h"

class SessionsOnlineSubsystem;
/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API AMSGameSession : public AGameSession
{
	GENERATED_BODY()

	explicit AMSGameSession(const FObjectInitializer& objectInitializer);
public:
	void BeginPlay() override;
	void BeginDestroy() override;
	
	bool HandleStartMatchRequest() override;
	void HandleMatchIsWaitingToStart() override;
	void HandleMatchHasStarted() override;
	void HandleMatchHasEnded() override;
	
	void CreateSession(bool isLan);
	void StartSession();
	void EndSession();
	void FindSessions();
	void JoinSession();
	void DestroySessionAndLeaveGame();
	void SetMatchReadyToStart(bool isReady);
	void StartGame();

private:
	void InitializeOnlineSubsystem();
	void UnregisterOnlineSubsystemDelegates() const;
	bool IsLAN() const;
	ULocalPlayer* GetLocalPlayer() const;
	void NotifyClientsGameStarted() const;
	FString JoinSessionCompleteResultTypeToFString(EOnJoinSessionCompleteResult::Type type) const;

	void OnCreateSessionComplete(FName sessionName, bool wasSuccessful) const;
	void OnDestroySessionComplete(FName sessionName, bool wasSuccessful) const;
	void OnStartSessionComplete(FName sessionName, bool wasSuccessful) override;
	void OnEndSessionComplete(FName sessionName, bool wasSuccessful) override;
	void OnFindSessionsComplete(TSharedPtr<class FOnlineSessionSearch> sessions, bool wasSuccessful);
	void OnJoinSessionComplete(const FString& travelURL, EOnJoinSessionCompleteResult::Type result);
	
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;
	FDelegateHandle OnStartSessionCompleteDelegateHandle;
	FDelegateHandle OnEndSessionCompleteDelegateHandle;
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;
	
	TSharedPtr<SessionsOnlineSubsystem> m_OnlineSubsystem;
	FString m_SessionIdToFound;
	bool m_IsLAN;
	bool m_IsMatchReadyToStart;
};