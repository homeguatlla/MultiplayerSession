#pragma once

#include "CoreMinimal.h"

#include "UObject/Interface.h"
#include "IOnlineGameSession.generated.h"


USTRUCT(BlueprintType)
struct FOnlineSessionFindResult
{
	GENERATED_BODY()

	FOnlineSessionFindResult() = default;
	FOnlineSessionFindResult(const FString& sessionIdParam, const FString& serverNameParam, int32 numOpenPublicConnectionsParam) :
	sessionId(sessionIdParam),
	serverName(serverNameParam),
	numOpenPublicConnections(numOpenPublicConnectionsParam) {}

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Online Session Find Result")
	FString sessionId;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Online Session Find Result")
	FString serverName;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Online Session Find Result")
	int32 numOpenPublicConnections;
};

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
	virtual void StartSession() = 0;
	virtual void EndSession() = 0;
	virtual void FindSessions() = 0;
	virtual void JoinSession(const FString& sessionId) = 0;
	virtual void DestroySessionAndLeaveGame() = 0;
	virtual TArray<FOnlineSessionFindResult> GetAvailableSessions() const = 0;

	virtual void StartGame() = 0;
};
