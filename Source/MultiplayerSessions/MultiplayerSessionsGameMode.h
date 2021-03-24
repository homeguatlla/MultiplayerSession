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

	/** Max number of players allowed to play in a multiplayer game together */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Multiplayer")
	uint8 MaxNumPlayers;

protected:
	void GenericPlayerInitialization(AController* C) override;
};



