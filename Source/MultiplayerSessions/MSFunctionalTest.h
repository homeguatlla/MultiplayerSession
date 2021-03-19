// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "BPSessionsOnlineSubsystem.h"
#include "FunctionalTest.h"
#include "MSFunctionalTest.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API AMSFunctionalTest : public AFunctionalTest
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MS Functional Test")
	void OnCreateSessionCompleted(bool wasSuccessful);	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MS Functional Test")
    void OnDestroySessionCompleted(bool wasSuccessful);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MS Functional Test")
    void OnStartSessionCompleted(bool wasSuccessful);	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MS Functional Test")
    void OnEndSessionCompleted(bool wasSuccessful);	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MS Functional Test")
    void OnFindSessionsCompleted(const TArray<FString>& sessions);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MS Functional Test")
	ABPSessionsOnlineSubsystem* OnlineSubsystem;
};
