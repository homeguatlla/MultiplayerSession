#pragma once
#include "CoreMinimal.h"

#include "MultiplayerSessionsGameMode.h"
#include "GameFramework/GameSession.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MSGameSession.generated.h"

class SessionsOnlineSubsystem;

UCLASS()
class MULTIPLAYERSESSIONS_API AMSGameSession : public AGameSession
{
	GENERATED_BODY()

	explicit AMSGameSession(const FObjectInitializer& objectInitializer);
public:
	void BeginPlay() override;
	void BeginDestroy() override;
	
	void CreateSession(bool isLan, uint8 maxNumPlayers, const FString& defaultPlayerName);
	void FindSessions();
	void JoinSession(const FString& sessionId);
	void DestroySessionAndLeaveGame();
	void StartGame();
	TSharedPtr<class FOnlineSessionSearch> GetAvailableSessions() const { return m_Sessions; }

	//Only used for functional test
	void StartSession();
	void EndSession();
	
private:
	void InitializeOnlineSubsystem();
	void UnregisterOnlineSubsystemDelegates() const;
	bool IsLAN() const;
	ULocalPlayer* GetLocalPlayer() const;
	APlayerController* GetPlayerControllerFromUserId(const FUniqueNetId& userId) const;
	FString JoinSessionCompleteResultTypeToFString(EOnJoinSessionCompleteResult::Type type) const;

	AMultiplayerSessionsGameMode* GetGameMode() const;
	
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
	TSharedPtr<class FOnlineSessionSearch> m_Sessions;
	bool m_IsLAN;
	bool m_IsMatchReadyToStart;
	FString m_DefaultPlayerName;
};
