// Fill out your copyright notice in the Description page of Project Settings.
#include "BPOnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Tests/AutomationCommon.h"

// Sets default values
ABPOnlineSubsystem::ABPOnlineSubsystem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void ABPOnlineSubsystem::CreateSession()
{
	ULocalPlayer* const Player = GetGameInstance()->GetFirstGamePlayer();
	m_OnlineSubsystem->CreateAndStartSession(Player->GetPreferredUniqueNetId().GetUniqueNetId(), GameSessionName, true, true, 4);
}

void ABPOnlineSubsystem::DestroySession()
{
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

// Called when the game starts or when spawned
void ABPOnlineSubsystem::BeginPlay()
{
	Super::BeginPlay();
	m_OnlineSubsystem = MakeShareable(new OnlineSubsystem(GetWorld()));
	OnCreateSessionCompleteInternalDelegateHandle = m_OnlineSubsystem->OnCreateAndStartSessionCompleteDelegate().AddUObject(this, &ABPOnlineSubsystem::OnCreateAndStartSessionInternalCompleted);
	OnDestroySessionCompleteInternalDelegateHandle = m_OnlineSubsystem->OnDestroySessionCompleteDelegate().AddUObject(this, &ABPOnlineSubsystem::OnDestroySessionInternalCompleted);
	OnFindSessionsCompleteInternalDelegateHandle = m_OnlineSubsystem->OnFindSessionsCompleteDelegate().AddUObject(this, &ABPOnlineSubsystem::OnFindSessionsInternalCompleted);
}

void ABPOnlineSubsystem::OnCreateAndStartSessionInternalCompleted(FName SessionName, bool bWasSuccessful)
{
	const auto color = bWasSuccessful ? FColor::Green : FColor::Red;
	GEngine->AddOnScreenDebugMessage(-1, 10.f, color, FString::Printf(TEXT("[ABPOnlineSubsystem][OnCreateAndStartSession] %s, %d"), *SessionName.ToString(), bWasSuccessful));

	m_OnlineSubsystem->OnCreateAndStartSessionCompleteDelegate().Remove(OnCreateSessionCompleteInternalDelegateHandle);
	
	OnCreateAndStartSessionCompleted(bWasSuccessful);
}

void ABPOnlineSubsystem::OnDestroySessionInternalCompleted(FName SessionName, bool bWasSuccessful)
{
	const auto color = bWasSuccessful ? FColor::Green : FColor::Red;
	GEngine->AddOnScreenDebugMessage(-1, 10.f, color, FString::Printf(TEXT("[ABPOnlineSubsystem][OnDestroySession] %s, %d"), *SessionName.ToString(), bWasSuccessful));

	m_OnlineSubsystem->OnDestroySessionCompleteDelegate().Remove(OnDestroySessionCompleteInternalDelegateHandle);
	
	OnDestroySessionCompleted(bWasSuccessful);
}

void ABPOnlineSubsystem::OnFindSessionsInternalCompleted(TSharedPtr<class FOnlineSessionSearch> Sessions, bool bWasSuccessful)
{
	const auto color = bWasSuccessful ? FColor::Green : FColor::Red;
	GEngine->AddOnScreenDebugMessage(-1, 10.f, color, FString::Printf(TEXT("[ABPOnlineSubsystem][OnFindSessions] bSuccess: %d"), bWasSuccessful));

	// Just debugging the Number of Search results. Can be displayed in UMG or something later on
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, FString::Printf(TEXT("[ABPOnlineSubsystem][OnFindSessions] Num Search Results: %d"), Sessions->SearchResults.Num()));
	
	TArray<FString> sessionsIds;
	for(auto&& session : Sessions->SearchResults)
	{
		UE_LOG(LogEditorAutomationTests, Display, TEXT("ABPOnlineSubsystem::OnFindSessionsInternalCompleted Session name %s"), *session.GetSessionIdStr());
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, FString::Printf(TEXT("[ABPOnlineSubsystem][OnFindSessions] Sessionname: %s "), *session.GetSessionIdStr()));
	
		sessionsIds.Push(session.GetSessionIdStr());	
	}

	m_OnlineSubsystem->OnFindSessionsCompleteDelegate().Remove(OnFindSessionsCompleteInternalDelegateHandle);
	
	OnFindSessionsCompleted(sessionsIds);
}