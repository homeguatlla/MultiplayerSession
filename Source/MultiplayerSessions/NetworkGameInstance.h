#pragma once

#include "CoreMinimal.h"
#include "IOnlineGameSession.h"
#include "MSGameSession.h"
#include "Engine/GameInstance.h"
#include "NetworkGameInstance.generated.h"


UCLASS()
class MULTIPLAYERSESSIONS_API UNetworkGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	
	UNetworkGameInstance(const FObjectInitializer& objectInitializer);
	
	void Shutdown() override;
	void Init() override;

	bool IsLan() const { return m_IsLAN; }
		
private:
	void HandleNetworkFailure(UWorld* world, UNetDriver* netDriver, ENetworkFailure::Type failureType,
                              const FString& errorString) const;
	void TravelFailure(UWorld* world, ETravelFailure::Type failureType, const FString& errorString);

	FDelegateHandle m_NetworkFailureDelegateHandle;
	FDelegateHandle m_TravelFailureDelegateHandle;
	bool m_IsLAN;
};
