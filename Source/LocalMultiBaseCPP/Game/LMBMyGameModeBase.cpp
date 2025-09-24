// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/LMBMyGameModeBase.h"
#include "Player/LMBPlayerController.h"
#include "Pawn/LMBpawnPlayer.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"


ALMBMyGameModeBase::ALMBMyGameModeBase()
{
	DefaultPawnClass = nullptr;
	PlayerControllerClass = ALMBPlayerController::StaticClass();
	LMBpawnPlayerClass = ALMBpawnPlayer::StaticClass();
}


void ALMBMyGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	UWorld* CurrentWorld = GetWorld();
	check(CurrentWorld);
	
	for (int32 i = 0; i < MaxPlayerIndex; i++)
	{
		APlayerStart* Foundstart = FindPlayerStart(CurrentWorld, PlayerStartTags[i]);
	}
}

APlayerStart* ALMBMyGameModeBase::FindPlayerStart(UWorld* CurrentWorld, const FName& TargetTag)
{
	check(CurrentWorld);

	for (TActorIterator<APlayerStart> Starts(CurrentWorld); Starts; ++Starts)
	{
		APlayerStart* FoundStart = *Starts;
		if (FoundStart && FoundStart->PlayerStartTag == TargetTag)
		{
			return FoundStart;
		}
	}


	return nullptr;
}

