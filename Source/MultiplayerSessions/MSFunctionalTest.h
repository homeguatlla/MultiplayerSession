// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "BPOnlineSubsystem.h"
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
	void OnCreateAndStartSessionCompleted(bool bWasSuccessful);	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MS Functional Test")
    void OnDestroySessionCompleted(bool bWasSuccessful);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MS Functional Test")
    void OnFindSessionsCompleted(const TArray<FString>& sessions);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MS Functional Test")
	ABPOnlineSubsystem* OnlineSubsystem;
};
