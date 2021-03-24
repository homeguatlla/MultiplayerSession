#include "Misc/AutomationTest.h"

#include "MultiplayerSessions/NetworkGameInstance.h"

UWorld* GetWorld()
{
	if (GEngine)
	{
		if (FWorldContext* World = GEngine->GetWorldContextFromPIEInstance(0))
		{
			return World->World();
		}
	}
	return nullptr;
}
 
UGameInstance* GetGameInstance()
{
	if (UWorld* World = GetWorld())
	{
		return World->GetGameInstance();
	}
 
	return nullptr;
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNetworkGameInstanceCreateSession, "Multiplayer.Sessions.CreateSession",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FNetworkGameInstanceCreateSession::RunTest(const FString& Parameters)
{
	if (GEngine)
	{
		if (GEngine->GetWorldContexts().Num() > 0)
		{
			const auto worldContext = GEngine->GetWorldContexts()[0];
			const auto world = worldContext.World();
			UNetworkGameInstance* gameInstance = NewObject<UNetworkGameInstance>();
			gameInstance->Init();
			world->SetGameInstance(gameInstance);
			
			if(gameInstance)
			{
				FString outError;
				gameInstance->CreateInitialPlayer(outError);
				if(gameInstance->GetNumLocalPlayers() > 0)
				{
					//gameInstance->CreateSession();	
				}
			}
		}
	}
	return true;
/*
	
    UNetworkGameInstance* gameInstance = NewObject<UNetworkGameInstance>();
    gameInstance->Init();
	FEditorAutomationTestUtilities
	//ULocalPlayer* player = NewObject<ULocalPlayer>();
    //gameInstance->AddLocalPlayer(player, 0);
    FString outError;
    gameInstance->CreateInitialPlayer(outError);
    gameInstance->StartOnlineGame();
    
    return true;*/
}