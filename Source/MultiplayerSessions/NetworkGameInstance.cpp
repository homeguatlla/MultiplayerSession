// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkGameInstance.h"
#include "OnlineSessionSettings.h"
#include "GameFramework/GameSession.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"


const FName LobbyMap("LobbyMap");
const FName GameMap("GameMap");
const FString MapPath("/Game/ThirdPersonCPP/Maps/");

UNetworkGameInstance::UNetworkGameInstance(const FObjectInitializer& ObjectInitializer) :
Super(ObjectInitializer)
{
}

void UNetworkGameInstance::HandleNetworkFailure(UWorld * world, UNetDriver *netDriver, ENetworkFailure::Type failureType, const FString& errorString = TEXT("")) const
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red,
		FString::Printf(TEXT("HandleNetworkFailure: %s"), *errorString));
	UE_LOG(LogTemp, Error, TEXT("UNetworkGameInstance::HandleNetworkFailure error: %s"), *errorString);
}

void UNetworkGameInstance::TravelFailure(UWorld* world, ETravelFailure::Type failureType, const FString& errorString)
{
	UE_LOG(LogLoad, Fatal, TEXT("UNetworkGameInstance::TravelFailure  %s"), *errorString);
}

void UNetworkGameInstance::Init()
{
	Super::Init();

	m_IsLAN = FParse::Param(FCommandLine::Get(), TEXT("LAN"));
	
	InitializeOnlineSubsystem();
	if (GEngine->OnTravelFailure().IsBoundToObject(this) == false)
	{
		m_NetworkFailureDelegateHandle = GetEngine()->OnNetworkFailure().AddUObject(
			this, &UNetworkGameInstance::HandleNetworkFailure);
	}
	
	if (GEngine->OnTravelFailure().IsBoundToObject(this) == false)
	{
		m_TravelFailureDelegateHandle = GEngine->OnTravelFailure().AddUObject(
            this, &UNetworkGameInstance::TravelFailure);
	}
}

void UNetworkGameInstance::Shutdown()
{
	Super::Shutdown();
	
	UnregisterOnlineSubsystemDelegates();

	if (GEngine->OnNetworkFailure().IsBoundToObject(this) == true)
	{
		GEngine->OnNetworkFailure().Remove(m_NetworkFailureDelegateHandle);
	}
	
	if (GEngine->OnTravelFailure().IsBoundToObject(this) == true)
	{
		GEngine->OnTravelFailure().Remove(m_TravelFailureDelegateHandle);
	}
}
/*
void UNetworkGameInstance::StartGameInstance()
{
	Super::StartGameInstance();
	
	InitializeOnlineSubsystem();
	GetEngine()->OnNetworkFailure().AddUObject(this, &UNetworkGameInstance::HandleNetworkFailure);
}

#if WITH_EDITOR
FGameInstancePIEResult UNetworkGameInstance::StartPlayInEditorGameInstance(ULocalPlayer* localPlayer,
                                                                           const FGameInstancePIEParameters& params)
{
	InitializeOnlineSubsystem();
	GetEngine()->OnNetworkFailure().AddUObject(this, &UNetworkGameInstance::HandleNetworkFailure);
	return Super::StartPlayInEditorGameInstance(localPlayer, params);
}
#endif*/

void UNetworkGameInstance::CreateSession()
{
	ULocalPlayer* const Player = GetFirstGamePlayer();
	const bool isPresence = true;
	const int maxNumPlayers = 4;
	
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red,
		FString::Printf(TEXT("UNetworkGameInstance::CreateSession is LAN? %d"), IsLAN() ));
	UE_LOG(LogTemp, Warning, TEXT("UNetworkGameInstance::CreateSession is LAN? %d"), IsLAN() );

	m_OnlineSubsystem->CreateSession(
		Player->GetPreferredUniqueNetId().GetUniqueNetId(),
		GameSessionName,
		IsLAN(),
		isPresence,
		maxNumPlayers);
}

void UNetworkGameInstance::DestroySessionAndLeaveGame()
{
	const FString sessionName(LexToString(GameSessionName));
		
	m_OnlineSubsystem->DestroySession(GameSessionName);
}

void UNetworkGameInstance::StartSession()
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red,
		FString::Printf(TEXT("UNetworkGameInstance::StartSession")));
	UE_LOG(LogTemp, Warning, TEXT("UNetworkGameInstance::StartSession"));
	m_OnlineSubsystem->StartSession(GameSessionName);
}
void UNetworkGameInstance::EndSession()
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red,
		FString::Printf(TEXT("UNetworkGameInstance::StartSession")));
	UE_LOG(LogTemp, Warning, TEXT("UNetworkGameInstance::EndSession"));
	m_OnlineSubsystem->EndSession(GameSessionName);
}

void UNetworkGameInstance::FindSessions()
{
	ULocalPlayer* const player = GetFirstGamePlayer();

	m_SessionIdToFound.Empty();
	
	const bool isPresence = true;
	m_OnlineSubsystem->FindSessions(player->GetPreferredUniqueNetId().GetUniqueNetId(), IsLAN(), isPresence);
}

void UNetworkGameInstance::JoinSession()
{
	ULocalPlayer* const player = GetFirstGamePlayer();

	if(!m_SessionIdToFound.IsEmpty())
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Session connecting: %s"), *m_SessionIdToFound));
		m_OnlineSubsystem->JoinSession(player->GetPreferredUniqueNetId().GetUniqueNetId(), GameSessionName, m_SessionIdToFound);
	}
}

void UNetworkGameInstance::OnCreateSessionComplete(FName sessionName, bool wasSuccessful) const
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green,
		FString::Printf(TEXT("UNetworkGameInstance::OnCreateAndStartSessionComplete")));
		
	if (wasSuccessful)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green,
			FString::Printf(TEXT("UNetworkGameInstance::OnCreateAndStartSessionComplete Session: %s"), *sessionName.ToString()));

		//UGameplayStatics::OpenLevel(GetWorld(), GameMap, true, "listen");
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red,
			FString::Printf(TEXT("UNetworkGameInstance::OnCreateAndStartSessionComplete not successful")));
		UE_LOG(LogTemp, Warning, TEXT("UNetworkGameInstance::OnCreateAndStartSessionComplete not successful"));
	}
}

void UNetworkGameInstance::OnDestroySessionComplete(FName sessionName, bool wasSuccessful) const
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green,
		FString::Printf(TEXT("UNetworkGameInstance::OnDestroySessionComplete")));
	
	if (wasSuccessful)
	{
		UGameplayStatics::OpenLevel(GetWorld(), LobbyMap, true);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red,
			FString::Printf(TEXT("UNetworkGameInstance::OnDestroySessionComplete not successful")));
		UE_LOG(LogTemp, Warning, TEXT("UNetworkGameInstance::OnDestroySessionComplete not successful"));
	}
}

void UNetworkGameInstance::OnStartSessionComplete(FName sessionName, bool wasSuccessful) const
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green,
		FString::Printf(TEXT("UNetworkGameInstance::OnStartSessionComplete")));
	
	if (wasSuccessful)
	{
		const FString url = MapPath + GameMap.ToString() + "?listen";
		GetWorld()->ServerTravel(url, true);
		//UGameplayStatics::OpenLevel(GetWorld(), GameMap, true, "listen");
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red,
			FString::Printf(TEXT("UNetworkGameInstance::OnStartSessionComplete not successful")));
		UE_LOG(LogTemp, Warning, TEXT("UNetworkGameInstance::OnDestroySessionComplete not successful"));
	}
}

void UNetworkGameInstance::OnEndSessionComplete(FName sessionName, bool wasSuccessful) const
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green,
		FString::Printf(TEXT("UNetworkGameInstance::OnEndSessionComplete")));
	
	if (wasSuccessful)
	{
		UGameplayStatics::OpenLevel(GetWorld(), LobbyMap, true);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red,
			FString::Printf(TEXT("UNetworkGameInstance::OnEndSessionComplete not successful")));
		UE_LOG(LogTemp, Warning, TEXT("UNetworkGameInstance::OnDestroySessionComplete not successful"));
	}
}

void UNetworkGameInstance::OnFindSessionsComplete(TSharedPtr<class FOnlineSessionSearch> sessions, bool wasSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green,
		FString::Printf(TEXT("UNetworkGameInstance::OnFindSessionsComplete")));
	
	if(wasSuccessful)
	{
		FString playerName("unknown");
		
		for(auto&& session : sessions->SearchResults)
		{
			UE_LOG(LogTemp, Display, TEXT("UNetworkGameInstance::OnFindSessionsComplete owningUserId = %d"), session.Session.OwningUserId.Get());
			UE_LOG(LogTemp, Display, TEXT("UNetworkGameInstance::OnFindSessionsComplete owningUserName %s"), *session.Session.OwningUserName);
			auto player = GetPlayerControllerFromNetId(GetWorld(), *session.Session.OwningUserId);
			if(player)
			{
				UE_LOG(LogTemp, Display, TEXT("UNetworkGameInstance::OnFindSessionsComplete GetPlayerControllerFromNetId"));
				playerName = player->GetPlayerState<APlayerState>()->GetPlayerName();
			}
			else
			{
				player = GetFirstLocalPlayerController();
				if(player)
				{
					UE_LOG(LogTemp, Display, TEXT("UNetworkGameInstance::OnFindSessionsComplete firstlocalplayercontroller"));
					playerName = player->GetPlayerState<APlayerState>()->GetPlayerName();
				}
			}
			UE_LOG(LogTemp,
				Display,
				TEXT("UNetworkGameInstance::OnFindSessionsComplete session id = %s owner name = %s public connections available: %d"),
				*session.GetSessionIdStr(),
				*playerName,				
				session.Session.NumOpenPublicConnections);
			
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green,
				FString::Printf(TEXT("UNetworkGameInstance::OnFindSessionsComplete session id: %s, owner name = %s public connections available: %d"),
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
			UE_LOG(LogTemp, Display, TEXT("UNetworkGameInstance::OnFindSessionsComplete No sessions found"));			
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green,
				FString::Printf(TEXT("UNetworkGameInstance::OnFindSessionsComplete No sessions found")));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red,
			FString::Printf(TEXT("UNetworkGameInstance::OnFindSessionsComplete Not successful")));
		UE_LOG(LogTemp, Warning, TEXT("UNetworkGameInstance::OnFindSessionsComplete not successful"));
	}
}

void UNetworkGameInstance::OnJoinSessionComplete(const FString& travelURL, EOnJoinSessionCompleteResult::Type result)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("UNetworkGameInstance::OnJoinSessionComplete")));
	if(result == EOnJoinSessionCompleteResult::Type::Success)
	{
		UE_LOG(LogTemp, Display, TEXT("UNetworkGameInstance::OnJoinSessionComplete travel to = %s"), *travelURL);
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green,
			FString::Printf(TEXT("UNetworkGameInstance::OnJoinSessionComplete travel to = %s"), *travelURL));
		const auto playerController = GetFirstLocalPlayerController();
		if(playerController)
		{
			playerController->ClientTravel(travelURL, ETravelType::TRAVEL_Absolute);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red,
				FString::Printf(TEXT("UNetworkGameInstance::OnJoinSessionComplete playercontroller null")));
			UE_LOG(LogTemp, Warning, TEXT("UNetworkGameInstance::OnJoinSessionComplete playercontroller null"));
		}
	}
	else
	{
		const FString resultString = JoinSessionCompleteResultTypeToFString(result);
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red,
			FString::Printf(TEXT("UNetworkGameInstance::OnJoinSessionComplete %s"), *resultString));
		UE_LOG(LogTemp, Warning, TEXT("UNetworkGameInstance::OnJoinSessionComplete %s"), *resultString);
	}
}

FString UNetworkGameInstance::JoinSessionCompleteResultTypeToFString(EOnJoinSessionCompleteResult::Type type) const
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

bool UNetworkGameInstance::IsLAN() const
{
	return m_IsLAN || m_OnlineSubsystem->GetSubsystemName() == "NULL";
}

void UNetworkGameInstance::InitializeOnlineSubsystem()
{
	UE_LOG(LogTemp, Warning, TEXT("UNetworkGameInstance::InitializeOnlineSubsystem"));
	
	m_OnlineSubsystem =  MakeShareable(new SessionsOnlineSubsystem(GetWorld()));
	
	OnCreateSessionCompleteDelegateHandle = m_OnlineSubsystem->OnCreateSessionCompleteDelegate().AddUObject(this, &UNetworkGameInstance::OnCreateSessionComplete);
	OnDestroySessionCompleteDelegateHandle = m_OnlineSubsystem->OnDestroySessionCompleteDelegate().AddUObject(this, &UNetworkGameInstance::OnDestroySessionComplete);
	OnStartSessionCompleteDelegateHandle = m_OnlineSubsystem->OnStartSessionCompleteDelegate().AddUObject(this, &UNetworkGameInstance::OnStartSessionComplete);
	OnEndSessionCompleteDelegateHandle = m_OnlineSubsystem->OnEndSessionCompleteDelegate().AddUObject(this, &UNetworkGameInstance::OnEndSessionComplete);
	OnFindSessionsCompleteDelegateHandle = m_OnlineSubsystem->OnFindSessionsCompleteDelegate().AddUObject(this, &UNetworkGameInstance::OnFindSessionsComplete);
	OnJoinSessionCompleteDelegateHandle = m_OnlineSubsystem->OnJoinSessionCompleteDelegate().AddUObject(this, &UNetworkGameInstance::OnJoinSessionComplete);
}

void UNetworkGameInstance::UnregisterOnlineSubsystemDelegates() const
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
