// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IOnlineGameSession.generated.h"

UINTERFACE()
class UIOnlineGameSession : public UInterface
{
	GENERATED_BODY()
};

class MULTIPLAYERSESSIONS_API IIOnlineGameSession
{
	GENERATED_BODY()
	
public:
    virtual void CreateSession() = 0;
	virtual void FindSessions() = 0;
	virtual void JoinSession() = 0;
	virtual void DestroySessionAndLeaveGame() = 0;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "GameSession")
    void OnCreateAndStartSessionCompleteEvent();
};
