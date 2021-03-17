// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "OnlineSubsystem.h"
#include "GameFramework/Actor.h"
#include "BPOnlineSubsystem.generated.h"

UCLASS()
class MULTIPLAYERSESSIONS_API ABPOnlineSubsystem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABPOnlineSubsystem();
	~ABPOnlineSubsystem();

	UFUNCTION(BlueprintCallable, Category="BP GameSession")
    void CreateSession();
	UFUNCTION(BlueprintCallable, Category="BP GameSession")
    void DestroySession();	
	UFUNCTION(BlueprintCallable, Category = "BP GameSession")
	void FindSessions();
    //UFUNCTION(BlueprintCallable, Category="BP GameSession")
    //void JoinSession();
    
	UFUNCTION(BlueprintNativeEvent, Category = "BP GameSession")
	void OnCreateAndStartSessionCompleted(bool bWasSuccessful);	
	UFUNCTION(BlueprintNativeEvent, Category = "BP GameSession")
    void OnDestroySessionCompleted(bool bWasSuccessful);
	UFUNCTION(BlueprintNativeEvent, Category = "BP GameSession")
    void OnFindSessionsCompleted(const TArray<FString>& sessions);
    
protected:
	void BeginPlay() override;
	void BeginDestroy() override;

private:
	void OnCreateAndStartSessionInternalCompleted(FName SessionName, bool bWasSuccessful);	
	void OnDestroySessionInternalCompleted(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsInternalCompleted(TSharedPtr<class FOnlineSessionSearch> Sessions, bool bWasSuccessful);

	FDelegateHandle OnCreateSessionCompleteInternalDelegateHandle;
	FDelegateHandle OnDestroySessionCompleteInternalDelegateHandle;
	FDelegateHandle OnFindSessionsCompleteInternalDelegateHandle;
	
	TSharedPtr<OnlineSubsystem> m_OnlineSubsystem;
};
