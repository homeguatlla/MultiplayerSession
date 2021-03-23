// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkGameInstance.h"

#include "MSGameSession.h"
#include "MultiplayerSessionsGameMode.h"
#include "GameFramework/GameModeBase.h"

const FName LobbyMap("LobbyMap");
const FName GameMap("GameMap");
const FString MapPath("/Game/ThirdPersonCPP/Maps/");
const int32 MaxNumPlayers = 4;

UNetworkGameInstance::UNetworkGameInstance(const FObjectInitializer& ObjectInitializer) :
Super(ObjectInitializer)
{
}

void UNetworkGameInstance::Init()
{
	Super::Init();

    m_IsLAN = FParse::Param(FCommandLine::Get(), TEXT("LAN"));
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
	
	if (GEngine->OnNetworkFailure().IsBoundToObject(this) == true)
	{
		GEngine->OnNetworkFailure().Remove(m_NetworkFailureDelegateHandle);
	}
	
	if (GEngine->OnTravelFailure().IsBoundToObject(this) == true)
	{
		GEngine->OnTravelFailure().Remove(m_TravelFailureDelegateHandle);
	}
}
AMSGameSession* UNetworkGameInstance::GetGameSession() const
{
	UWorld* const World = GetWorld();
	if (World)
	{
		AGameModeBase* const Game = World->GetAuthGameMode();
		if (Game)
		{
			return Cast<AMSGameSession>(Game->GameSession);
		}
	}

	return nullptr;
}

void UNetworkGameInstance::CreateSession()
{
	GetGameSession()->CreateSession(m_IsLAN);
}

void UNetworkGameInstance::DestroySessionAndLeaveGame()
{
	GetGameSession()->DestroySessionAndLeaveGame();
}

void UNetworkGameInstance::StartSession()
{
	/*
	auto* const gameMode = GetWorld() != NULL ? Cast<AMultiplayerSessionsGameMode>(GetWorld()->GetAuthGameMode()) : NULL;
	if (gameMode)
	{
		UE_LOG(LogTemp, Warning, TEXT("UNetworkGameInstance::StartSession StartMatch"));
		
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UNetworkGameInstance::StartSession Error game mode null"));
	}*/
	GetGameSession()->StartGame();
}

void UNetworkGameInstance::EndSession()
{
	GetGameSession()->EndSession();
}

void UNetworkGameInstance::FindSessions()
{
	GetGameSession()->FindSessions();
}

void UNetworkGameInstance::JoinSession()
{
	GetGameSession()->JoinSession();
}

void UNetworkGameInstance::HandleNetworkFailure(UWorld * world, UNetDriver *netDriver, ENetworkFailure::Type failureType, const FString& errorString = TEXT("")) const
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red,
        FString::Printf(TEXT("HandleNetworkFailure: %s"), *errorString));
	UE_LOG(LogTemp, Error, TEXT("UNetworkGameInstance::HandleNetworkFailure error: %s"), *errorString);
}

void UNetworkGameInstance::TravelFailure(UWorld* world, ETravelFailure::Type failureType, const FString& errorString)
{
	UE_LOG(LogLoad, Error, TEXT("UNetworkGameInstance::TravelFailure  %s"), *errorString);
}