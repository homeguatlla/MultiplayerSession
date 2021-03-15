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
	void InitializeOnlineSubsystem();
	void StartGameInstance() override;
	FGameInstancePIEResult StartPlayInEditorGameInstance(ULocalPlayer* localPlayer, const FGameInstancePIEParameters& params) override;

	UFUNCTION(BlueprintCallable, Category="GameSession")
	void StartOnlineGame() override;
	UFUNCTION(BlueprintCallable, Category="GameSession")
	void FindOnlineGames() override;
	UFUNCTION(BlueprintCallable, Category="GameSession")
	void JoinOnlineGame() override;
	UFUNCTION(BlueprintCallable, Category="GameSession")
	void DestroySessionAndLeaveGame() override;	

private:
	TSharedPtr<OnlineSubsystem> m_OnlineSubsystem;
};
