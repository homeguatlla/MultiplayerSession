// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/GameMode.h"

#include "MultiplayerSessionsGameMode.generated.h"

UCLASS(minimalapi)
class AMultiplayerSessionsGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AMultiplayerSessionsGameMode();
	TSubclassOf<AGameSession> GetGameSessionClass() const override;
	void SetMatchReadyToStart(bool isReady);

protected:
	void GenericPlayerInitialization(AController* C) override;
};



