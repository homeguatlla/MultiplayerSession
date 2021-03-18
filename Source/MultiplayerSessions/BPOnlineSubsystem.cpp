// Fill out your copyright notice in the Description page of Project Settings.
#include "BPOnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Tests/AutomationCommon.h"

ABPOnlineSubsystem::ABPOnlineSubsystem()
{
 	PrimaryActorTick.bCanEverTick = false;
	UE_LOG(LogNet, Display, TEXT("ABPOnlineSubsystem::ABPOnlineSubsystem"));
}

ABPOnlineSubsystem::~ABPOnlineSubsystem()
{
	UE_LOG(LogNet, Display, TEXT("ABPOnlineSubsystem::~ABPOnlineSubsystem"));
}

void ABPOnlineSubsystem::CreateSession()
{
	UE_LOG(LogNet, Display, TEXT("ABPOnlineSubsystem::CreateSession"));
	ULocalPlayer* const player = GetGameInstance()->GetFirstGamePlayer();
	m_OnlineSubsystem->CreateSession(player->GetPreferredUniqueNetId().GetUniqueNetId(), GameSessionName, true, true, 4);
}

void ABPOnlineSubsystem::DestroySession()
{
	UE_LOG(LogNet, Display, TEXT("ABPOnlineSubsystem::DestroySession"));
	const FString sessionName(LexToString(GameSessionName));	
	m_OnlineSubsystem->DestroySession(GameSessionName);
}

void ABPOnlineSubsystem::StartSession()
{
	UE_LOG(LogNet, Display, TEXT("ABPOnlineSubsystem::StartSession"));
	m_OnlineSubsystem->StartSession(GameSessionName);
}

void ABPOnlineSubsystem::EndSession()
{
	UE_LOG(LogNet, Display, TEXT("ABPOnlineSubsystem::EndSession"));
	m_OnlineSubsystem->EndSession(GameSessionName);
}

void ABPOnlineSubsystem::FindSessions()
{
	ULocalPlayer* const player = GetGameInstance()->GetFirstGamePlayer();
	m_OnlineSubsystem->FindSessions(player->GetPreferredUniqueNetId().GetUniqueNetId(), true, true);
}

void ABPOnlineSubsystem::OnCreateSessionCompleted_Implementation(bool wasSuccessful)
{
}

void ABPOnlineSubsystem::OnDestroySessionCompleted_Implementation(bool wasSuccessful)
{
}

void ABPOnlineSubsystem::OnStartSessionCompleted_Implementation(bool wasSuccessful)
{
}

void ABPOnlineSubsystem::OnEndSessionCompleted_Implementation(bool wasSuccessful)
{
}

void ABPOnlineSubsystem::OnFindSessionsCompleted_Implementation(const TArray<FString>& sessions)
{
}

void ABPOnlineSubsystem::BeginPlay()
{
	Super::BeginPlay();
	if(!IsTemplate(RF_Transient))
	{
		UE_LOG(LogNet, Display, TEXT("BPOnlineSubsystem::BeginPlay registering delegates"));
		
		m_OnlineSubsystem = MakeShareable(new OnlineSubsystem(GetWorld()));
		OnCreateSessionCompleteInternalDelegateHandle = m_OnlineSubsystem->OnCreateSessionCompleteDelegate().AddUObject(this, &ABPOnlineSubsystem::OnCreateSessionInternalCompleted);
		OnDestroySessionCompleteInternalDelegateHandle = m_OnlineSubsystem->OnDestroySessionCompleteDelegate().AddUObject(this, &ABPOnlineSubsystem::OnDestroySessionInternalCompleted);
		OnStartSessionCompleteInternalDelegateHandle = m_OnlineSubsystem->OnStartSessionCompleteDelegate().AddUObject(this, &ABPOnlineSubsystem::OnStartSessionInternalCompleted);
		OnEndSessionCompleteInternalDelegateHandle = m_OnlineSubsystem->OnEndSessionCompleteDelegate().AddUObject(this, &ABPOnlineSubsystem::OnEndSessionInternalCompleted);
		OnFindSessionsCompleteInternalDelegateHandle = m_OnlineSubsystem->OnFindSessionsCompleteDelegate().AddUObject(this, &ABPOnlineSubsystem::OnFindSessionsInternalCompleted);
	}
}

void ABPOnlineSubsystem::BeginDestroy()
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

void ABPOnlineSubsystem::OnCreateSessionInternalCompleted(FName sessionName, bool wasSuccessful)
{
	UE_LOG(LogNet, Display, TEXT("BPOnlineSubsystem::OnCreateSessionInternalCompleted"));
	const auto color = wasSuccessful ? FColor::Green : FColor::Red;
	GEngine->AddOnScreenDebugMessage(-1, 10.f, color,
		FString::Printf(TEXT("[ABPOnlineSubsystem][OnCreateAndStartSession] %s, %d"), *sessionName.ToString(), wasSuccessful));

	OnCreateSessionCompleted(wasSuccessful);
}

void ABPOnlineSubsystem::OnDestroySessionInternalCompleted(FName sessionName, bool wasSuccessful)
{
	UE_LOG(LogNet, Display, TEXT("BPOnlineSubsystem::OnDestroySessionInternalCompleted"));
	const auto color = wasSuccessful ? FColor::Green : FColor::Red;
	GEngine->AddOnScreenDebugMessage(-1, 10.f, color,
		FString::Printf(TEXT("[ABPOnlineSubsystem][OnDestroySession] %s, %d"), *sessionName.ToString(), wasSuccessful));
	
	OnDestroySessionCompleted(wasSuccessful);
}

void ABPOnlineSubsystem::OnStartSessionInternalCompleted(FName sessionName, bool wasSuccessful)
{
	UE_LOG(LogNet, Display, TEXT("BPOnlineSubsystem::OnStartSessionInternalCompleted"));
	const auto color = wasSuccessful ? FColor::Green : FColor::Red;
	GEngine->AddOnScreenDebugMessage(-1, 10.f, color,
		FString::Printf(TEXT("[ABPOnlineSubsystem][OnStartSessionInternalCompleted] %s, %d"), *sessionName.ToString(), wasSuccessful));
	
	OnStartSessionCompleted(wasSuccessful);
}

void ABPOnlineSubsystem::OnEndSessionInternalCompleted(FName sessionName, bool wasSuccessful)
{
	UE_LOG(LogNet, Display, TEXT("BPOnlineSubsystem::OnEndSessionInternalCompleted"));
	const auto color = wasSuccessful ? FColor::Green : FColor::Red;
	GEngine->AddOnScreenDebugMessage(-1, 10.f, color,
		FString::Printf(TEXT("[ABPOnlineSubsystem][OnEndSessionInternalCompleted] %s, %d"), *sessionName.ToString(), wasSuccessful));
	
	OnEndSessionCompleted(wasSuccessful);
}

void ABPOnlineSubsystem::OnFindSessionsInternalCompleted(TSharedPtr<class FOnlineSessionSearch> sessions, bool wasSuccessful)
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