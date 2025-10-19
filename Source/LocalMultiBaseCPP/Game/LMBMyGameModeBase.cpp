// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/LMBMyGameModeBase.h"
#include "Player/LMBPlayerController.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
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

	UWorld* World = GetWorld();
	if (!World) return;

	// 기존 Pawn 제거
	DestroyAllPawnsBeforeLevelChange();

	// 추가 LocalPlayer 제거 (첫 번째 PlayerController 제외)
	DestroyExtraLocalPlayers();

	CurrentPlayerIndex = 0;

	for (int32 i = 0; i < MaxPlayerIndex; ++i)
	{
		APlayerStart* Start = FindPlayerStart(World, PlayerStartTags.IsValidIndex(i) ? PlayerStartTags[i] : NAME_None);
		if (Start)
			SpawnLocalPlayer(World, Start);
		else
			UE_LOG(LogTemp, Warning, TEXT("PlayerStart with tag %s not found!"), *PlayerStartTags[i].ToString());
	}
}

APlayerStart* ALMBMyGameModeBase::FindPlayerStart(UWorld* World, const FName& TargetTag)
{
	for (TActorIterator<APlayerStart> It(World); It; ++It)
	{
		APlayerStart* Start = *It;
		if (Start && Start->PlayerStartTag == TargetTag)
			return Start;
	}
	return nullptr;
}

void ALMBMyGameModeBase::SpawnLocalPlayer(UWorld* World, APlayerStart* PlayerStart)
{
	if (!World || !PlayerStart) return;

	APlayerController* PC = nullptr;

	if (CurrentPlayerIndex == 0)
	{
		PC = World->GetFirstPlayerController();
	}
	else
	{
		ULocalPlayer* NewLP = CreateLocalPlayer();
		if (NewLP)
			PC = NewLP->GetPlayerController(World);
	}

	if (!PC) return;

	if (APawn* Existing = PC->GetPawn())
		Existing->Destroy();

	ALMBpawnPlayer* Pawn = SpawnAndPossessPawn(World, PlayerStart, PC);
	if (Pawn)
	{
		AlivePlayers.Add(Pawn);  // 살아있는 배열에 추가
	}

	CurrentPlayerIndex++;
}

ULocalPlayer* ALMBMyGameModeBase::CreateLocalPlayer()
{
	UGameInstance* GI = GetGameInstance();
	if (!GI) return nullptr;

	FString Error;
	return GI->CreateLocalPlayer(-1, Error, true);
}

ALMBpawnPlayer* ALMBMyGameModeBase::SpawnAndPossessPawn(UWorld* World, APlayerStart* Start, APlayerController* PC)
{
	if (!World || !Start || !PC) return nullptr;

	TSubclassOf<ALMBpawnPlayer> ClassToSpawn = BP_PlayerClass ? BP_PlayerClass : LMBpawnPlayerClass;

	FActorSpawnParameters Params;
	Params.Name = FName(*FString::Printf(TEXT("PlayerPawn%d"), CurrentPlayerIndex));

	ALMBpawnPlayer* Pawn = World->SpawnActor<ALMBpawnPlayer>(
		ClassToSpawn,
		Start->GetActorLocation(),
		Start->GetActorRotation(),
		Params
	);

	if (!Pawn)
	{
		UE_LOG(LogTemp, Error, TEXT("Pawn spawn failed for player %d"), CurrentPlayerIndex);
		return nullptr;
	}

	Pawn->SetPlayerIndex(CurrentPlayerIndex);
	Pawn->InitializePlayerStats(CurrentPlayerIndex);

	PC->Possess(Pawn);

	UE_LOG(LogTemp, Warning, TEXT("%dP (%s) spawned."), CurrentPlayerIndex + 1, *Pawn->GetName());
	return Pawn;
}

void ALMBMyGameModeBase::DestroyAllPawnsBeforeLevelChange()
{
	UWorld* World = GetWorld();
	if (!World) return;

	TArray<AActor*> FoundPawns;
	UGameplayStatics::GetAllActorsOfClass(World, APawn::StaticClass(), FoundPawns);

	for (AActor* Actor : FoundPawns)
	{
		UE_LOG(LogTemp, Warning, TEXT("이전 Pawn 제거: %s"), *Actor->GetName());
		Actor->Destroy();
	}
}

void ALMBMyGameModeBase::DestroyExtraLocalPlayers()
{
	UGameInstance* GI = GetGameInstance();
	if (!GI) return;

	TArray<ULocalPlayer*> LocalPlayersCopy = GI->GetLocalPlayers();
	for (int32 i = LocalPlayersCopy.Num() - 1; i >= 1; --i)
	{
		if (ULocalPlayer* LP = LocalPlayersCopy[i])
		{
			if (LP->PlayerController)
				LP->PlayerController->UnPossess();

			GI->RemoveLocalPlayer(LP);
			UE_LOG(LogTemp, Warning, TEXT("LocalPlayer 제거: %d"), i);
		}
	}
}

void ALMBMyGameModeBase::NotifyPlayerDied(ALMBpawnBase* DeadPawn)
{
	AlivePlayers.Remove(DeadPawn);
	UE_LOG(LogTemp, Warning, TEXT("남은 플레이어 수: %d"), AlivePlayers.Num());
	if (AlivePlayers.Num() <= 0)
	{
		OnGameOver();
	}
}
void ALMBMyGameModeBase::OnGameOver()
{
	UE_LOG(LogTemp, Warning, TEXT("=== 게임오버 ==="));


}