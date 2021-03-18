// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "IOnlineGameSession.h"
#include "OnlineSubsystem.h"
#include "Engine/GameInstance.h"
#include "NetworkGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UNetworkGameInstance : public UGameInstance, public IIOnlineGameSession
{
	GENERATED_BODY()

public:
	
	UNetworkGameInstance(const FObjectInitializer& objectInitializer);
	void HandleNetworkFailure(UWorld* world, UNetDriver* netDriver, ENetworkFailure::Type failureType,
	                          const FString& errorString) const;
	
	UFUNCTION(BlueprintCallable, Category="GameSession")
	void CreateSession() override;
	UFUNCTION(BlueprintCallable, Category="GameSession")
    void StartSession() override;
	UFUNCTION(BlueprintCallable, Category="GameSession")
	void EndSession() override;
	UFUNCTION(BlueprintCallable, Category="GameSession")
	void FindSessions() override;
	UFUNCTION(BlueprintCallable, Category="GameSession")
	void JoinSession() override;
	UFUNCTION(BlueprintCallable, Category="GameSession")
	void DestroySessionAndLeaveGame() override;
	
	void UnregisterOnlineSubsystemDelegates() const;
	void Shutdown() override;
	void Init() override;
		
private:
	/*void StartGameInstance() override;
#if WITH_EDITOR
	FGameInstancePIEResult StartPlayInEditorGameInstance(ULocalPlayer* localPlayer, const FGameInstancePIEParameters& params) override;
#endif*/
	void InitializeOnlineSubsystem();
	bool IsLAN() const;
	void OnCreateSessionComplete(FName sessionName, bool wasSuccessful) const;
	void OnDestroySessionComplete(FName sessionName, bool wasSuccessful) const;
	void OnStartSessionComplete(FName sessionName, bool wasSuccessful) const;
	void OnEndSessionComplete(FName sessionName, bool wasSuccessful) const;
	void OnFindSessionsComplete(TSharedPtr<class FOnlineSessionSearch> sessions, bool wasSuccessful);
	void OnJoinSessionComplete(const FString& travelURL, EOnJoinSessionCompleteResult::Type result);
	FString JoinSessionCompleteResultTypeToFString(EOnJoinSessionCompleteResult::Type type) const;

	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;
	FDelegateHandle OnStartSessionCompleteDelegateHandle;
	FDelegateHandle OnEndSessionCompleteDelegateHandle;
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;
	
	TSharedPtr<OnlineSubsystem> m_OnlineSubsystem;
	FString m_SessionIdToFound;
	bool m_IsLAN;
};
