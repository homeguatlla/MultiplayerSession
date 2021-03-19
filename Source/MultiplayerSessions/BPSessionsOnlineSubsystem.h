// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "SessionsOnlineSubsystem.h"
#include "GameFramework/Actor.h"
#include "BPSessionsOnlineSubsystem.generated.h"

UCLASS()
class MULTIPLAYERSESSIONS_API ABPSessionsOnlineSubsystem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABPSessionsOnlineSubsystem();
	~ABPSessionsOnlineSubsystem();

	UFUNCTION(BlueprintCallable, Category="BP GameSession")
    void CreateSession();
	UFUNCTION(BlueprintCallable, Category="BP GameSession")
    void DestroySession();
	UFUNCTION(BlueprintCallable, Category="BP GameSession")
	void StartSession();
	UFUNCTION(BlueprintCallable, Category="BP GameSession")
	void EndSession();
	UFUNCTION(BlueprintCallable, Category = "BP GameSession")
	void FindSessions();
    //UFUNCTION(BlueprintCallable, Category="BP GameSession")
    //void JoinSession();
    
	UFUNCTION(BlueprintNativeEvent, Category = "BP GameSession")
	void OnCreateSessionCompleted(bool wasSuccessful);	
	UFUNCTION(BlueprintNativeEvent, Category = "BP GameSession")
    void OnDestroySessionCompleted(bool wasSuccessful);
	UFUNCTION(BlueprintNativeEvent, Category = "BP GameSession")
    void OnStartSessionCompleted(bool wasSuccessful);	
	UFUNCTION(BlueprintNativeEvent, Category = "BP GameSession")
	void OnEndSessionCompleted(bool wasSuccessful);	
	UFUNCTION(BlueprintNativeEvent, Category = "BP GameSession")
    void OnFindSessionsCompleted(const TArray<FString>& sessions);
    
protected:
	void BeginPlay() override;
	void BeginDestroy() override;

private:
	void OnCreateSessionInternalCompleted(FName sessionName, bool wasSuccessful);
	void OnDestroySessionInternalCompleted(FName sessionName, bool wasSuccessful);
	void OnStartSessionInternalCompleted(FName sessionName, bool wasSuccessful);	
	void OnEndSessionInternalCompleted(FName sessionName, bool wasSuccessful);	
	void OnFindSessionsInternalCompleted(TSharedPtr<class FOnlineSessionSearch> sessions, bool wasSuccessful);

	FDelegateHandle OnCreateSessionCompleteInternalDelegateHandle;
	FDelegateHandle OnDestroySessionCompleteInternalDelegateHandle;
	FDelegateHandle OnStartSessionCompleteInternalDelegateHandle;
	FDelegateHandle OnEndSessionCompleteInternalDelegateHandle;	
	FDelegateHandle OnFindSessionsCompleteInternalDelegateHandle;
	
	TSharedPtr<SessionsOnlineSubsystem> m_OnlineSubsystem;
};
