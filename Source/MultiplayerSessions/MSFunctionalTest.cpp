// Fill out your copyright notice in the Description page of Project Settings.


#include "MSFunctionalTest.h"

void AMSFunctionalTest::OnCreateSessionCompleted_Implementation(bool wasSuccessful)
{
	if(OnlineSubsystem)
	{
		OnlineSubsystem->OnCreateSessionCompleted(wasSuccessful);
	}
}

void AMSFunctionalTest::OnDestroySessionCompleted_Implementation(bool wasSuccessful)
{
	if(OnlineSubsystem)
	{
		OnlineSubsystem->OnDestroySessionCompleted(wasSuccessful);
	}
}

void AMSFunctionalTest::OnStartSessionCompleted_Implementation(bool wasSuccessful)
{
	if(OnlineSubsystem)
	{
		OnlineSubsystem->OnStartSessionCompleted(wasSuccessful);
	}
}

void AMSFunctionalTest::OnEndSessionCompleted_Implementation(bool wasSuccessful)
{
	if(OnlineSubsystem)
	{
		OnlineSubsystem->OnEndSessionCompleted(wasSuccessful);
	}
}

void AMSFunctionalTest::OnFindSessionsCompleted_Implementation(const TArray<FString>& sessions)
{
	if(OnlineSubsystem)
	{
		OnlineSubsystem->OnFindSessionsCompleted(sessions);
	}
}


