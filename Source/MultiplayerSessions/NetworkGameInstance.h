// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "OnlineSubsystem.h"
#include "Engine/GameInstance.h"
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
	void InitializeOnlineSubsystem();
	void StartGameInstance() override;
	FGameInstancePIEResult StartPlayInEditorGameInstance(ULocalPlayer* localPlayer, const FGameInstancePIEParameters& params) override;
	
	UFUNCTION(BlueprintCallable, Category = "Network|Test")
    void StartOnlineGame();
	UFUNCTION(BlueprintCallable, Category = "Network|Test")
	void FindOnlineGames();
	UFUNCTION(BlueprintCallable, Category = "Network|Test")
	void JoinOnlineGame();
	UFUNCTION(BlueprintCallable, Category = "Network|Test")
    void DestroySessionAndLeaveGame();

private:
	TSharedPtr<OnlineSubsystem> m_OnlineSubsystem;
};
