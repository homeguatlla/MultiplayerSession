// Fill out your copyright notice in the Description page of Project Settings.


#include "MSFunctionalTest.h"

void AMSFunctionalTest::OnCreateAndStartSessionCompleted_Implementation(bool bWasSuccessful)
{
	if(OnlineSubsystem)
	{
		OnlineSubsystem->OnCreateAndStartSessionCompleted(bWasSuccessful);
	}
}

void AMSFunctionalTest::OnDestroySessionCompleted_Implementation(bool bWasSuccessful)
{
	if(OnlineSubsystem)
	{
		OnlineSubsystem->OnDestroySessionCompleted(bWasSuccessful);
	}
}

void AMSFunctionalTest::OnFindSessionsCompleted_Implementation(const TArray<FString>& sessions)
{
	if(OnlineSubsystem)
	{
		OnlineSubsystem->OnFindSessionsCompleted(sessions);
	}
}
