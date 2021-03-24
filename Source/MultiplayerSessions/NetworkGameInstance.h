#pragma once

#include "CoreMinimal.h"
#include "IOnlineGameSession.h"
#include "MSGameSession.h"
#include "Engine/GameInstance.h"
#include "NetworkGameInstance.generated.h"


UCLASS()
class MULTIPLAYERSESSIONS_API UNetworkGameInstance : public UGameInstance, public IIOnlineGameSession
{
	GENERATED_BODY()

public:
	
	UNetworkGameInstance(const FObjectInitializer& objectInitializer);
	
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
    void StartGame();
	
	void Shutdown() override;
	AMSGameSession* GetGameSession() const;
	void Init() override;
		
private:
	void HandleNetworkFailure(UWorld* world, UNetDriver* netDriver, ENetworkFailure::Type failureType,
                              const FString& errorString) const;
	void TravelFailure(UWorld* world, ETravelFailure::Type failureType, const FString& errorString);

	FDelegateHandle m_NetworkFailureDelegateHandle;
	FDelegateHandle m_TravelFailureDelegateHandle;
	bool m_IsLAN;
};
