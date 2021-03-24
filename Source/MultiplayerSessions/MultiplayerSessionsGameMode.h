#pragma once

#include "CoreMinimal.h"

#include "IOnlineGameSession.h"
#include "GameFramework/GameMode.h"
#include "MultiplayerSessionsGameMode.generated.h"

class AMSGameSession;
UCLASS(minimalapi)
class AMultiplayerSessionsGameMode : public AGameMode, public IIOnlineGameSession
{
	GENERATED_BODY()

public:
	AMultiplayerSessionsGameMode();
	TSubclassOf<AGameSession> GetGameSessionClass() const override;	

	//The IOnlineGameSession methods can only be called by server in multiplayer or singleplayer
	//Once a player (starts game as singleplayer) is connected to a server (being a client) he cannot
	//call these methods anymore. Maybe we would like to allow the client calls destroySessionAndLeaveGame
	//in this case, we can put this method into the gameinstance or go through server with an rpc
	UFUNCTION(BlueprintCallable, Category="GameSession")
    void CreateSession() override;
	
	void StartSession() override;
	
	UFUNCTION(BlueprintCallable, Category="GameSession")
    void EndSession() override;
	UFUNCTION(BlueprintCallable, Category="GameSession")
    void FindSessions() override;
	UFUNCTION(BlueprintCallable, Category="GameSession")
    void JoinSession() override;
	UFUNCTION(BlueprintCallable, Category="GameSession")
    void DestroySessionAndLeaveGame() override;
	UFUNCTION(BlueprintCallable, Category="Game")
    void StartGame() override;
	
	/** Max number of players allowed to play in a multiplayer game together */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Multiplayer")
	uint8 MaxNumPlayers;

protected:
	void GenericPlayerInitialization(AController* C) override;

private:
	AMSGameSession* GetGameSession() const;
};



