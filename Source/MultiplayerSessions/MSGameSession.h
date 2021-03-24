#pragma once
#include "CoreMinimal.h"
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
	
	void CreateSession(bool isLan);
	void FindSessions();
	void JoinSession();
	void DestroySessionAndLeaveGame();
	void StartGame();

	//Only used for functional test
	void StartSession();
	void EndSession();

private:
	void InitializeOnlineSubsystem();
	void UnregisterOnlineSubsystemDelegates() const;
	bool IsLAN() const;
	ULocalPlayer* GetLocalPlayer() const;
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
