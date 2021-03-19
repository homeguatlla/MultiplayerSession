// Fill out your copyright notice in the Description page of Project Settings.
#include "BPSessionsOnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Tests/AutomationCommon.h"

ABPSessionsOnlineSubsystem::ABPSessionsOnlineSubsystem()
{
 	PrimaryActorTick.bCanEverTick = false;
	UE_LOG(LogNet, Display, TEXT("ABPOnlineSubsystem::ABPOnlineSubsystem"));
}

ABPSessionsOnlineSubsystem::~ABPSessionsOnlineSubsystem()
{
	UE_LOG(LogNet, Display, TEXT("ABPOnlineSubsystem::~ABPOnlineSubsystem"));
}

void ABPSessionsOnlineSubsystem::CreateSession()
{
	UE_LOG(LogNet, Display, TEXT("ABPOnlineSubsystem::CreateSession"));
	ULocalPlayer* const player = GetGameInstance()->GetFirstGamePlayer();
	m_OnlineSubsystem->CreateSession(player->GetPreferredUniqueNetId().GetUniqueNetId(), GameSessionName, true, true, 4);
}

void ABPSessionsOnlineSubsystem::DestroySession()
{
	UE_LOG(LogNet, Display, TEXT("ABPOnlineSubsystem::DestroySession"));
	const FString sessionName(LexToString(GameSessionName));	
	m_OnlineSubsystem->DestroySession(GameSessionName);
}

void ABPSessionsOnlineSubsystem::StartSession()
{
	UE_LOG(LogNet, Display, TEXT("ABPOnlineSubsystem::StartSession"));
	m_OnlineSubsystem->StartSession(GameSessionName);
}

void ABPSessionsOnlineSubsystem::EndSession()
{
	UE_LOG(LogNet, Display, TEXT("ABPOnlineSubsystem::EndSession"));
	m_OnlineSubsystem->EndSession(GameSessionName);
}

void ABPSessionsOnlineSubsystem::FindSessions()
{
	ULocalPlayer* const player = GetGameInstance()->GetFirstGamePlayer();
	m_OnlineSubsystem->FindSessions(player->GetPreferredUniqueNetId().GetUniqueNetId(), true, true);
}

void ABPSessionsOnlineSubsystem::OnCreateSessionCompleted_Implementation(bool wasSuccessful)
{
}

void ABPSessionsOnlineSubsystem::OnDestroySessionCompleted_Implementation(bool wasSuccessful)
{
}

void ABPSessionsOnlineSubsystem::OnStartSessionCompleted_Implementation(bool wasSuccessful)
{
}

void ABPSessionsOnlineSubsystem::OnEndSessionCompleted_Implementation(bool wasSuccessful)
{
}

void ABPSessionsOnlineSubsystem::OnFindSessionsCompleted_Implementation(const TArray<FString>& sessions)
{
}

void ABPSessionsOnlineSubsystem::BeginPlay()
{
	Super::BeginPlay();
	if(!IsTemplate(RF_Transient))
	{
		UE_LOG(LogNet, Display, TEXT("BPOnlineSubsystem::BeginPlay registering delegates"));
		
		m_OnlineSubsystem = MakeShareable(new SessionsOnlineSubsystem(GetWorld()));
		OnCreateSessionCompleteInternalDelegateHandle = m_OnlineSubsystem->OnCreateSessionCompleteDelegate().AddUObject(this, &ABPSessionsOnlineSubsystem::OnCreateSessionInternalCompleted);
		OnDestroySessionCompleteInternalDelegateHandle = m_OnlineSubsystem->OnDestroySessionCompleteDelegate().AddUObject(this, &ABPSessionsOnlineSubsystem::OnDestroySessionInternalCompleted);
		OnStartSessionCompleteInternalDelegateHandle = m_OnlineSubsystem->OnStartSessionCompleteDelegate().AddUObject(this, &ABPSessionsOnlineSubsystem::OnStartSessionInternalCompleted);
		OnEndSessionCompleteInternalDelegateHandle = m_OnlineSubsystem->OnEndSessionCompleteDelegate().AddUObject(this, &ABPSessionsOnlineSubsystem::OnEndSessionInternalCompleted);
		OnFindSessionsCompleteInternalDelegateHandle = m_OnlineSubsystem->OnFindSessionsCompleteDelegate().AddUObject(this, &ABPSessionsOnlineSubsystem::OnFindSessionsInternalCompleted);
	}
}

void ABPSessionsOnlineSubsystem::BeginDestroy()
{
	Super::BeginDestroy();
	if(!IsTemplate(RF_Transient))
	{
		UE_LOG(LogNet, Display, TEXT("ABPOnlineSubsystem::BeginDestroy"));
		if(OnCreateSessionCompleteInternalDelegateHandle.IsValid())
		{
			m_OnlineSubsystem->OnCreateSessionCompleteDelegate().Remove(OnCreateSessionCompleteInternalDelegateHandle);
		}
		if(OnDestroySessionCompleteInternalDelegateHandle.IsValid())
		{
			m_OnlineSubsystem->OnDestroySessionCompleteDelegate().Remove(OnDestroySessionCompleteInternalDelegateHandle);
		}
		if(OnStartSessionCompleteInternalDelegateHandle.IsValid())
		{
			m_OnlineSubsystem->OnStartSessionCompleteDelegate().Remove(OnStartSessionCompleteInternalDelegateHandle);
		}
		if(OnEndSessionCompleteInternalDelegateHandle.IsValid())
		{
			m_OnlineSubsystem->OnEndSessionCompleteDelegate().Remove(OnEndSessionCompleteInternalDelegateHandle);
		}
		if(OnFindSessionsCompleteInternalDelegateHandle.IsValid())
		{
			m_OnlineSubsystem->OnFindSessionsCompleteDelegate().Remove(OnFindSessionsCompleteInternalDelegateHandle);
		}
	}
}

void ABPSessionsOnlineSubsystem::OnCreateSessionInternalCompleted(FName sessionName, bool wasSuccessful)
{
	UE_LOG(LogNet, Display, TEXT("BPOnlineSubsystem::OnCreateSessionInternalCompleted"));
	const auto color = wasSuccessful ? FColor::Green : FColor::Red;
	GEngine->AddOnScreenDebugMessage(-1, 10.f, color,
		FString::Printf(TEXT("[ABPOnlineSubsystem][OnCreateAndStartSession] %s, %d"), *sessionName.ToString(), wasSuccessful));

	OnCreateSessionCompleted(wasSuccessful);
}

void ABPSessionsOnlineSubsystem::OnDestroySessionInternalCompleted(FName sessionName, bool wasSuccessful)
{
	UE_LOG(LogNet, Display, TEXT("BPOnlineSubsystem::OnDestroySessionInternalCompleted"));
	const auto color = wasSuccessful ? FColor::Green : FColor::Red;
	GEngine->AddOnScreenDebugMessage(-1, 10.f, color,
		FString::Printf(TEXT("[ABPOnlineSubsystem][OnDestroySession] %s, %d"), *sessionName.ToString(), wasSuccessful));
	
	OnDestroySessionCompleted(wasSuccessful);
}

void ABPSessionsOnlineSubsystem::OnStartSessionInternalCompleted(FName sessionName, bool wasSuccessful)
{
	UE_LOG(LogNet, Display, TEXT("BPOnlineSubsystem::OnStartSessionInternalCompleted"));
	const auto color = wasSuccessful ? FColor::Green : FColor::Red;
	GEngine->AddOnScreenDebugMessage(-1, 10.f, color,
		FString::Printf(TEXT("[ABPOnlineSubsystem][OnStartSessionInternalCompleted] %s, %d"), *sessionName.ToString(), wasSuccessful));
	
	OnStartSessionCompleted(wasSuccessful);
}

void ABPSessionsOnlineSubsystem::OnEndSessionInternalCompleted(FName sessionName, bool wasSuccessful)
{
	UE_LOG(LogNet, Display, TEXT("BPOnlineSubsystem::OnEndSessionInternalCompleted"));
	const auto color = wasSuccessful ? FColor::Green : FColor::Red;
	GEngine->AddOnScreenDebugMessage(-1, 10.f, color,
		FString::Printf(TEXT("[ABPOnlineSubsystem][OnEndSessionInternalCompleted] %s, %d"), *sessionName.ToString(), wasSuccessful));
	
	OnEndSessionCompleted(wasSuccessful);
}

void ABPSessionsOnlineSubsystem::OnFindSessionsInternalCompleted(TSharedPtr<class FOnlineSessionSearch> sessions, bool wasSuccessful)
{
	const auto color = wasSuccessful ? FColor::Green : FColor::Red;
	GEngine->AddOnScreenDebugMessage(-1, 10.f, color,
		FString::Printf(TEXT("[ABPOnlineSubsystem][OnFindSessions] bSuccess: %d"), wasSuccessful));
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue,
		FString::Printf(TEXT("[ABPOnlineSubsystem][OnFindSessions] Num Search Results: %d"), sessions->SearchResults.Num()));
	
	TArray<FString> sessionsIds;
	for(auto&& session : sessions->SearchResults)
	{
		UE_LOG(LogEditorAutomationTests, Display, TEXT("ABPOnlineSubsystem::OnFindSessionsInternalCompleted Session name %s"), *session.GetSessionIdStr());
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue,
			FString::Printf(TEXT("[ABPOnlineSubsystem][OnFindSessions] Sessionname: %s "), *session.GetSessionIdStr()));
	
		sessionsIds.Push(session.GetSessionIdStr());	
	}
	
	OnFindSessionsCompleted(sessionsIds);
}