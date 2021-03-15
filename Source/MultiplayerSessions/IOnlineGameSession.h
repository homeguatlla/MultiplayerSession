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
    virtual void StartOnlineGame() = 0;
	virtual void FindOnlineGames() = 0;
	virtual void JoinOnlineGame() = 0;
	virtual void DestroySessionAndLeaveGame() = 0;
};
