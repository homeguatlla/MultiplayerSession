// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MultiplayerSessionsGameMode.generated.h"

UCLASS(minimalapi)
class AMultiplayerSessionsGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMultiplayerSessionsGameMode();
	TSubclassOf<AGameSession> GetGameSessionClass() const override;
};



