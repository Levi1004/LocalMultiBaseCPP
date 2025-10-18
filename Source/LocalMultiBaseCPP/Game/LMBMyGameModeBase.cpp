// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/LMBMyGameModeBase.h"
#include "Player/LMBPlayerController.h"
#include "Pawn/LMBpawnPlayer.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/GameInstance.h"

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
	if (!CurrentWorld) return;

	for (int32 i = 0; i < MaxPlayerIndex; i++)
	{
		APlayerStart* FoundStart = FindPlayerStart(CurrentWorld, PlayerStartTags[i]);
		if (FoundStart)
		{
			SpawnLocalPlayer(CurrentWorld, FoundStart);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("PlayerStart with tag %s not found!"), *PlayerStartTags[i].ToString());
		}
	}
}

APlayerStart* ALMBMyGameModeBase::FindPlayerStart(UWorld* CurrentWorld, const FName& TargetTag)
{
	if (!CurrentWorld) return nullptr;

	for (TActorIterator<APlayerStart> It(CurrentWorld); It; ++It)
	{
		APlayerStart* Start = *It;
		if (Start && Start->PlayerStartTag == TargetTag)
		{
			return Start;
		}
	}
	return nullptr;
}

void ALMBMyGameModeBase::SpawnLocalPlayer(UWorld* World, APlayerStart* PlayerStart)
{
	if (!World || !PlayerStart) return;

	APlayerController* PlayerController = nullptr;

	if (CurrentPlayerIndex == 0)
	{
		PlayerController = World->GetFirstPlayerController();
	}
	else
	{
		PlayerControllerClass = APlayerController::StaticClass();
		ULocalPlayer* NewLocalPlayer = CreateLocalPlayer();
		PlayerController = NewLocalPlayer ? NewLocalPlayer->GetPlayerController(World) : nullptr;
	}

	if (!PlayerController) return;

	// 기존 Pawn 제거
	if (APawn* ExistingPawn = PlayerController->GetPawn())
	{
		ExistingPawn->Destroy();
	}

	SpawnAndPossessPawn(World, PlayerStart, PlayerController);
	CurrentPlayerIndex++;
}

ULocalPlayer* ALMBMyGameModeBase::CreateLocalPlayer()
{
	UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance) return nullptr;

	FString Error;
	return GameInstance->CreateLocalPlayer(-1, Error, true);
}

ALMBpawnPlayer* ALMBMyGameModeBase::SpawnAndPossessPawn(UWorld* World, APlayerStart* PlayerStart, APlayerController* PlayerController)
{
	if (!World || !PlayerStart || !PlayerController) return nullptr;

	TSubclassOf<ALMBpawnPlayer> ClassToSpawn = BP_PlayerClass ? BP_PlayerClass : LMBpawnPlayerClass;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Name = FName(*FString::Printf(TEXT("PlayerPawn%d"), CurrentPlayerIndex));

	ALMBpawnPlayer* NewPawn = World->SpawnActor<ALMBpawnPlayer>(
		ClassToSpawn,
		PlayerStart->GetActorLocation(),
		PlayerStart->GetActorRotation(),
		SpawnParams
	);

	if (!NewPawn)
	{
		UE_LOG(LogTemp, Error, TEXT("Pawn spawn failed for player %d"), CurrentPlayerIndex);
		return nullptr;
	}

	NewPawn->SetPlayerIndex(CurrentPlayerIndex);
	NewPawn->InitializePlayerStats(CurrentPlayerIndex);
	PlayerController->Possess(NewPawn);

	UE_LOG(LogTemp, Warning, TEXT("%dP (%s) spawned."), CurrentPlayerIndex + 1, *NewPawn->GetName());
	return NewPawn;
}

void ALMBMyGameModeBase::CheckGameOver_Implementation()
{
	bool bAllDead = true;

	for (TActorIterator<ALMBpawnPlayer> It(GetWorld()); It; ++It)
	{
		ALMBpawnPlayer* Player = *It;
		if (Player && !Player->IsDead())
		{
			bAllDead = false;
			break;
		}
	}

	if (bAllDead)
	{
		UE_LOG(LogTemp, Warning, TEXT("게임 오버! 모든 플레이어 사망"));

		// 레벨 이동 삭제 → 단순히 로그만 남기고, 이후 UI 처리나 다른 로직에서 대응 가능
	}
	
}