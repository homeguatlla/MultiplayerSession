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
	ULocalPlayer* const Player = GetGameInstance()->GetFirstGamePlayer();
	m_OnlineSubsystem->CreateAndStartSession(Player->GetPreferredUniqueNetId().GetUniqueNetId(), GameSessionName, true, true, 4);
}

void ABPOnlineSubsystem::DestroySession()
{
	UE_LOG(LogNet, Display, TEXT("ABPOnlineSubsystem::DestroySession"));
	const FString sessionName(LexToString(GameSessionName));	
	m_OnlineSubsystem->DestroySession(GameSessionName);
}

void ABPOnlineSubsystem::FindSessions()
{
	ULocalPlayer* const Player = GetGameInstance()->GetFirstGamePlayer();
	m_OnlineSubsystem->FindSessions(Player->GetPreferredUniqueNetId().GetUniqueNetId(), true, true);
}

void ABPOnlineSubsystem::OnCreateAndStartSessionCompleted_Implementation(bool bWasSuccessful)
{
}

void ABPOnlineSubsystem::OnDestroySessionCompleted_Implementation(bool bWasSuccessful)
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
		OnCreateSessionCompleteInternalDelegateHandle = m_OnlineSubsystem->OnCreateAndStartSessionCompleteDelegate().AddUObject(this, &ABPOnlineSubsystem::OnCreateAndStartSessionInternalCompleted);
		OnDestroySessionCompleteInternalDelegateHandle = m_OnlineSubsystem->OnDestroySessionCompleteDelegate().AddUObject(this, &ABPOnlineSubsystem::OnDestroySessionInternalCompleted);
		OnFindSessionsCompleteInternalDelegateHandle = m_OnlineSubsystem->OnFindSessionsCompleteDelegate().AddUObject(this, &ABPOnlineSubsystem::OnFindSessionsInternalCompleted);
	}
}

void ABPOnlineSubsystem::BeginDestroy()
{
	Super::BeginDestroy();
	if(!IsTemplate(RF_Transient))
	{
		UE_LOG(LogNet, Display, TEXT("ABPOnlineSubsystem::BeginDestroy"));
		m_OnlineSubsystem->OnCreateAndStartSessionCompleteDelegate().Remove(OnCreateSessionCompleteInternalDelegateHandle);
		m_OnlineSubsystem->OnDestroySessionCompleteDelegate().Remove(OnDestroySessionCompleteInternalDelegateHandle);
		m_OnlineSubsystem->OnFindSessionsCompleteDelegate().Remove(OnFindSessionsCompleteInternalDelegateHandle);
	}
}

void ABPOnlineSubsystem::OnCreateAndStartSessionInternalCompleted(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogNet, Display, TEXT("BPOnlineSubsystem::OnCreateAndStartSessionInternalCompleted"));
	const auto color = bWasSuccessful ? FColor::Green : FColor::Red;
	GEngine->AddOnScreenDebugMessage(-1, 10.f, color, FString::Printf(TEXT("[ABPOnlineSubsystem][OnCreateAndStartSession] %s, %d"), *SessionName.ToString(), bWasSuccessful));

	OnCreateAndStartSessionCompleted(bWasSuccessful);
}

void ABPOnlineSubsystem::OnDestroySessionInternalCompleted(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogNet, Display, TEXT("BPOnlineSubsystem::OnDestroySessionInternalCompleted"));
	const auto color = bWasSuccessful ? FColor::Green : FColor::Red;
	GEngine->AddOnScreenDebugMessage(-1, 10.f, color, FString::Printf(TEXT("[ABPOnlineSubsystem][OnDestroySession] %s, %d"), *SessionName.ToString(), bWasSuccessful));
	
	OnDestroySessionCompleted(bWasSuccessful);
}

void ABPOnlineSubsystem::OnFindSessionsInternalCompleted(TSharedPtr<class FOnlineSessionSearch> Sessions, bool bWasSuccessful)
{
	const auto color = bWasSuccessful ? FColor::Green : FColor::Red;
	GEngine->AddOnScreenDebugMessage(-1, 10.f, color, FString::Printf(TEXT("[ABPOnlineSubsystem][OnFindSessions] bSuccess: %d"), bWasSuccessful));
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, FString::Printf(TEXT("[ABPOnlineSubsystem][OnFindSessions] Num Search Results: %d"), Sessions->SearchResults.Num()));
	
	TArray<FString> sessionsIds;
	for(auto&& session : Sessions->SearchResults)
	{
		UE_LOG(LogEditorAutomationTests, Display, TEXT("ABPOnlineSubsystem::OnFindSessionsInternalCompleted Session name %s"), *session.GetSessionIdStr());
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, FString::Printf(TEXT("[ABPOnlineSubsystem][OnFindSessions] Sessionname: %s "), *session.GetSessionIdStr()));
	
		sessionsIds.Push(session.GetSessionIdStr());	
	}
	
	OnFindSessionsCompleted(sessionsIds);
}