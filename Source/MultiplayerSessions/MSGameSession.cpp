// Fill out your copyright notice in the Description page of Project Settings.


#include "MSGameSession.h"


#include "OnlineSessionSettings.h"
#include "SessionsOnlineSubsystem.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

const FName LobbyMap("LobbyMap");
const FName GameMap("GameMap");
const FString MapPath("/Game/ThirdPersonCPP/Maps/");
const int32 MaxNumPlayers = 4;

void AMSGameSession::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Display, TEXT("AMSGameSession::BeginPlay"));
	
	InitializeOnlineSubsystem();
}

void AMSGameSession::BeginDestroy()
{
	Super::BeginDestroy();
	UE_LOG(LogTemp, Display, TEXT("AMSGameSession::BeginDestroy"));
	
	UnregisterOnlineSubsystemDelegates();
}

bool AMSGameSession::HandleStartMatchRequest()
{
	UE_LOG(LogTemp, Display, TEXT("AMSGameSession::HandleStartMatchRequest"));
	return Super::HandleStartMatchRequest();
}

void AMSGameSession::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();
	UE_LOG(LogTemp, Display, TEXT("AMSGameSession::HandleMatchIsWaitingToStart"));
}

void AMSGameSession::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
	UE_LOG(LogTemp, Display, TEXT("AMSGameSession::HandleMatchHasStarted"));
}

void AMSGameSession::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();
	UE_LOG(LogTemp, Display, TEXT("AMSGameSession::HandleMatchHasEnded"));
}

void AMSGameSession::CreateSession(bool isLan)
{
	ULocalPlayer* const Player = GetLocalPlayer();
	const bool isPresence = true;
	m_IsLAN = isLan;
	
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red,
		FString::Printf(TEXT("UNetworkGameInstance::CreateSession is LAN? %d"), IsLAN() ));
	UE_LOG(LogTemp, Warning, TEXT("UNetworkGameInstance::CreateSession is LAN? %d"), IsLAN() );

	m_OnlineSubsystem->CreateSession(
		Player->GetPreferredUniqueNetId().GetUniqueNetId(),
		GameSessionName,
		IsLAN(),
		isPresence,
		MaxNumPlayers);
}

void AMSGameSession::DestroySessionAndLeaveGame()
{
	
	const FString sessionName(LexToString(GameSessionName));
		
	m_OnlineSubsystem->DestroySession(GameSessionName);
}

void AMSGameSession::StartSession()
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red,
		FString::Printf(TEXT("UNetworkGameInstance::StartSession")));
	UE_LOG(LogTemp, Warning, TEXT("UNetworkGameInstance::StartSession"));
	m_OnlineSubsystem->StartSession(GameSessionName);
}
void AMSGameSession::EndSession()
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red,
		FString::Printf(TEXT("UNetworkGameInstance::StartSession")));
	UE_LOG(LogTemp, Warning, TEXT("UNetworkGameInstance::EndSession"));
	m_OnlineSubsystem->EndSession(GameSessionName);
}

void AMSGameSession::FindSessions()
{
	ULocalPlayer* const player = GetLocalPlayer();

	m_SessionIdToFound.Empty();
	
	const bool isPresence = true;
	m_OnlineSubsystem->FindSessions(player->GetPreferredUniqueNetId().GetUniqueNetId(), IsLAN(), isPresence);
}

void AMSGameSession::JoinSession()
{
	ULocalPlayer* const player = GetLocalPlayer();

	if(!m_SessionIdToFound.IsEmpty())
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Session connecting: %s"), *m_SessionIdToFound));
		m_OnlineSubsystem->JoinSession(player->GetPreferredUniqueNetId().GetUniqueNetId(), GameSessionName, m_SessionIdToFound);
	}
}

void AMSGameSession::OnCreateSessionComplete(FName sessionName, bool wasSuccessful) const
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green,
		FString::Printf(TEXT("AMSGameSession::OnCreateAndStartSessionComplete")));
		
	if (wasSuccessful)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green,
			FString::Printf(TEXT("AMSGameSession::OnCreateAndStartSessionComplete Session: %s"), *sessionName.ToString()));

		//UGameplayStatics::OpenLevel(GetWorld(), GameMap, true, "listen");
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red,
			FString::Printf(TEXT("AMSGameSession::OnCreateAndStartSessionComplete not successful")));
		UE_LOG(LogTemp, Warning, TEXT("AMSGameSession::OnCreateAndStartSessionComplete not successful"));
	}
}

void AMSGameSession::OnDestroySessionComplete(FName sessionName, bool wasSuccessful) const
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green,
		FString::Printf(TEXT("AMSGameSession::OnDestroySessionComplete")));
	
	if (wasSuccessful)
	{
		UGameplayStatics::OpenLevel(GetWorld(), LobbyMap, true);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red,
			FString::Printf(TEXT("AMSGameSession::OnDestroySessionComplete not successful")));
		UE_LOG(LogTemp, Warning, TEXT("AMSGameSession::OnDestroySessionComplete not successful"));
	}
}

void AMSGameSession::OnStartSessionComplete(FName sessionName, bool wasSuccessful) const
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green,
		FString::Printf(TEXT("AMSGameSession::OnStartSessionComplete")));
	
	if (wasSuccessful)
	{
		const FString url = MapPath + GameMap.ToString() + "?listen";
		GetWorld()->ServerTravel(url, true);
		//UGameplayStatics::OpenLevel(GetWorld(), GameMap, true, "listen");
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red,
			FString::Printf(TEXT("AMSGameSession::OnStartSessionComplete not successful")));
		UE_LOG(LogTemp, Warning, TEXT("AMSGameSession::OnDestroySessionComplete not successful"));
	}
}

void AMSGameSession::OnEndSessionComplete(FName sessionName, bool wasSuccessful) const
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green,
		FString::Printf(TEXT("AMSGameSession::OnEndSessionComplete")));
	
	if (wasSuccessful)
	{
		UGameplayStatics::OpenLevel(GetWorld(), LobbyMap, true);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red,
			FString::Printf(TEXT("AMSGameSession::OnEndSessionComplete not successful")));
		UE_LOG(LogTemp, Warning, TEXT("AMSGameSession::OnDestroySessionComplete not successful"));
	}
}

void AMSGameSession::OnFindSessionsComplete(TSharedPtr<class FOnlineSessionSearch> sessions, bool wasSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green,
		FString::Printf(TEXT("AMSGameSession::OnFindSessionsComplete")));
	
	if(wasSuccessful)
	{
		FString playerName("unknown");
		
		for(auto&& session : sessions->SearchResults)
		{
			UE_LOG(LogTemp, Display, TEXT("AMSGameSession::OnFindSessionsComplete owningUserId = %d"), session.Session.OwningUserId.Get());
			UE_LOG(LogTemp, Display, TEXT("AMSGameSession::OnFindSessionsComplete owningUserName %s"), *session.Session.OwningUserName);
			auto player = GetPlayerControllerFromNetId(GetWorld(), *session.Session.OwningUserId);
			if(player)
			{
				UE_LOG(LogTemp, Display, TEXT("AMSGameSession::OnFindSessionsComplete GetPlayerControllerFromNetId"));
				playerName = player->GetPlayerState<APlayerState>()->GetPlayerName();
			}
			else
			{
				player = GetWorld()->GetFirstPlayerController();//GetFirstLocalPlayerController();
				if(player)
				{
					UE_LOG(LogTemp, Display, TEXT("AMSGameSession::OnFindSessionsComplete firstlocalplayercontroller"));
					playerName = player->GetPlayerState<APlayerState>()->GetPlayerName();
				}
			}
			UE_LOG(LogTemp,
				Display,
				TEXT("AMSGameSession::OnFindSessionsComplete session id = %s owner name = %s public connections available: %d"),
				*session.GetSessionIdStr(),
				*playerName,				
				session.Session.NumOpenPublicConnections);
			
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green,
				FString::Printf(TEXT("AMSGameSession::OnFindSessionsComplete session id: %s, owner name = %s public connections available: %d"),
					*session.GetSessionIdStr(),
					*playerName,
					session.Session.NumOpenPublicConnections));
		}
		if(sessions->SearchResults.Num() > 0)
		{
			m_SessionIdToFound = sessions->SearchResults[0].GetSessionIdStr();
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("AMSGameSession::OnFindSessionsComplete No sessions found"));			
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green,
				FString::Printf(TEXT("AMSGameSession::OnFindSessionsComplete No sessions found")));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red,
			FString::Printf(TEXT("AMSGameSession::OnFindSessionsComplete Not successful")));
		UE_LOG(LogTemp, Warning, TEXT("AMSGameSession::OnFindSessionsComplete not successful"));
	}
}

void AMSGameSession::OnJoinSessionComplete(const FString& travelURL, EOnJoinSessionCompleteResult::Type result)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("AMSGameSession::OnJoinSessionComplete")));
	if(result == EOnJoinSessionCompleteResult::Type::Success)
	{
		UE_LOG(LogTemp, Display, TEXT("AMSGameSession::OnJoinSessionComplete travel to = %s"), *travelURL);
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green,
			FString::Printf(TEXT("AMSGameSession::OnJoinSessionComplete travel to = %s"), *travelURL));
		const auto playerController = GetWorld()->GetFirstPlayerController(); //GetFirstLocalPlayerController();
		if(playerController)
		{
			playerController->ClientTravel(travelURL, ETravelType::TRAVEL_Absolute);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red,
				FString::Printf(TEXT("AMSGameSession::OnJoinSessionComplete playercontroller null")));
			UE_LOG(LogTemp, Warning, TEXT("AMSGameSession::OnJoinSessionComplete playercontroller null"));
		}
	}
	else
	{
		const FString resultString = JoinSessionCompleteResultTypeToFString(result);
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red,
			FString::Printf(TEXT("AMSGameSession::OnJoinSessionComplete %s"), *resultString));
		UE_LOG(LogTemp, Warning, TEXT("AMSGameSession::OnJoinSessionComplete %s"), *resultString);
	}
}

FString AMSGameSession::JoinSessionCompleteResultTypeToFString(EOnJoinSessionCompleteResult::Type type) const
{
	switch (type)
	{
		case EOnJoinSessionCompleteResult::Success:
			return "Success";
		case EOnJoinSessionCompleteResult::SessionIsFull:
			return "SessionIsFull";
		case EOnJoinSessionCompleteResult::SessionDoesNotExist:
			return "SessionDoesNotExist";
		case EOnJoinSessionCompleteResult::CouldNotRetrieveAddress:
			return "CouldNotRetrieveAddress";
		case EOnJoinSessionCompleteResult::AlreadyInSession:
			return "AlreadyInSession";
		case EOnJoinSessionCompleteResult::UnknownError:
		default:
			return "UnknownError";
	}
}

bool AMSGameSession::IsLAN() const
{
	return m_IsLAN || m_OnlineSubsystem->GetSubsystemName() == "NULL";
}

void AMSGameSession::InitializeOnlineSubsystem()
{
	UE_LOG(LogTemp, Warning, TEXT("AMSGameSession::InitializeOnlineSubsystem"));
	
	m_OnlineSubsystem =  MakeShareable(new SessionsOnlineSubsystem(GetWorld()));
	
	OnCreateSessionCompleteDelegateHandle = m_OnlineSubsystem->OnCreateSessionCompleteDelegate().AddUObject(this, &AMSGameSession::OnCreateSessionComplete);
	OnDestroySessionCompleteDelegateHandle = m_OnlineSubsystem->OnDestroySessionCompleteDelegate().AddUObject(this, &AMSGameSession::OnDestroySessionComplete);
	OnStartSessionCompleteDelegateHandle = m_OnlineSubsystem->OnStartSessionCompleteDelegate().AddUObject(this, &AMSGameSession::OnStartSessionComplete);
	OnEndSessionCompleteDelegateHandle = m_OnlineSubsystem->OnEndSessionCompleteDelegate().AddUObject(this, &AMSGameSession::OnEndSessionComplete);
	OnFindSessionsCompleteDelegateHandle = m_OnlineSubsystem->OnFindSessionsCompleteDelegate().AddUObject(this, &AMSGameSession::OnFindSessionsComplete);
	OnJoinSessionCompleteDelegateHandle = m_OnlineSubsystem->OnJoinSessionCompleteDelegate().AddUObject(this, &AMSGameSession::OnJoinSessionComplete);
}

void AMSGameSession::UnregisterOnlineSubsystemDelegates() const
{
	if(OnCreateSessionCompleteDelegateHandle.IsValid())
	{
		m_OnlineSubsystem->OnCreateSessionCompleteDelegate().Remove(OnCreateSessionCompleteDelegateHandle);
	}
	if(OnDestroySessionCompleteDelegateHandle.IsValid())
	{
		m_OnlineSubsystem->OnDestroySessionCompleteDelegate().Remove(OnDestroySessionCompleteDelegateHandle);
	}

	if(OnStartSessionCompleteDelegateHandle.IsValid())
	{
		m_OnlineSubsystem->OnStartSessionCompleteDelegate().Remove(OnStartSessionCompleteDelegateHandle);
	}

	if(OnEndSessionCompleteDelegateHandle.IsValid())
	{
		m_OnlineSubsystem->OnEndSessionCompleteDelegate().Remove(OnEndSessionCompleteDelegateHandle);
	}
	
	if(OnFindSessionsCompleteDelegateHandle.IsValid())
	{
		m_OnlineSubsystem->OnFindSessionsCompleteDelegate().Remove(OnFindSessionsCompleteDelegateHandle);
	}
	if(OnJoinSessionCompleteDelegateHandle.IsValid())
	{
		m_OnlineSubsystem->OnJoinSessionCompleteDelegate().Remove(OnJoinSessionCompleteDelegateHandle);
	}
}

ULocalPlayer* AMSGameSession::GetLocalPlayer() const
{
	return GetWorld()->GetFirstPlayerController()->GetLocalPlayer();
}