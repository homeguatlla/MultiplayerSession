// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MSPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API AMSPlayerController : public APlayerController
{
	GENERATED_BODY()

	public:
	/** notify player about started match */
	UFUNCTION(reliable, client)
    void NotifyClientGameStarted(FName sessionName);
};
