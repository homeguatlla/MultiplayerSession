// Fill out your copyright notice in the Description page of Project Settings.


#include "MSPlayerController.h"

#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"

void AMSPlayerController::NotifyClientGameStarted_Implementation(FName sessionName)
{
	IOnlineSubsystem* onlineSub = Online::GetSubsystem(GetWorld());
	if (onlineSub)
	{
		IOnlineSessionPtr sessions = onlineSub->GetSessionInterface();
		if (sessions.IsValid() && (sessions->GetNamedSession(sessionName) != nullptr))
		{
			UE_LOG(LogOnline, Display, TEXT("AMSPlayerController::NotifyClientGameStarted_Implementation Starting session %s on client"), *sessionName.ToString());
			sessions->StartSession(sessionName);
		}
	}
	else
	{
		UE_LOG(LogOnline, Error, TEXT("AMSPlayerController::NotifyClientGameStarted_Implementation Error no onlinesubsystem created yet"));
	}
}
