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
	
	UNetworkGameInstance(const FObjectInitializer& ObjectInitializer);
	void HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType,
	                          const FString& ErrorString);
	
	UFUNCTION(BlueprintCallable, Category="GameSession")
	void CreateSession() override;
	UFUNCTION(BlueprintCallable, Category="GameSession")
	void FindSessions() override;
	UFUNCTION(BlueprintCallable, Category="GameSession")
	void JoinSession() override;
	UFUNCTION(BlueprintCallable, Category="GameSession")
	void DestroySessionAndLeaveGame() override;
	void UnregisterOnlineSubsystemDelegates();

	void Shutdown() override;
	void Init() override;
		
private:
	/*void StartGameInstance() override;
#if WITH_EDITOR
	FGameInstancePIEResult StartPlayInEditorGameInstance(ULocalPlayer* localPlayer, const FGameInstancePIEParameters& params) override;
#endif*/
	void InitializeOnlineSubsystem();
	bool IsLAN() const;
	void OnCreateAndStartSessionComplete(FName SessionName, bool bWasSuccessful) const;
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful) const;
	void OnFindSessionsComplete(TSharedPtr<class FOnlineSessionSearch> Sessions, bool bWasSuccessful);
	void OnJoinSessionComplete(const FString& travelURL, bool bWasSuccessful);
	
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;
	
	TSharedPtr<OnlineSubsystem> m_OnlineSubsystem;
	FString m_SessionIdToFound;
	bool m_IsLAN;
};
