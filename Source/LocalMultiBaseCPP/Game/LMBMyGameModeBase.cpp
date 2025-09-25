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
		SpawnLocalPlayer(CurrentWorld, Foundstart);
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

void ALMBMyGameModeBase::SpawnLocalPlayer(UWorld* World, APlayerStart* PlayerStart)
{
	APlayerController* PlayerController = nullptr;

	if (CurrentPlayerIndex == 0)
	{
		// 처음 생성하는 플레이어라면
		PlayerController = World->GetFirstPlayerController();
	}
	else
	{
		// 처음 생성하는 플레이어가 아니라면
		// 생성할 때 언리얼 기본 플레이어 컨트롤러 할당
		PlayerControllerClass = APlayerController::StaticClass();

		ULocalPlayer* NewLocalPlayer = CreateLocalPlayer();

		PlayerController = NewLocalPlayer->GetPlayerController(World);
	}

	check(PlayerController);

	SpawnAndPossessPawn(World, PlayerStart, PlayerController);

	CurrentPlayerIndex++;
}

ULocalPlayer* ALMBMyGameModeBase::CreateLocalPlayer()
{
	// 현재 내 게임 전체에 영향을 끼치는 클래스
	// 전역적으로 접근이 가능함
	UGameInstance* GameInstance = GetGameInstance();

	FString Error;
	// UserId 여기에 -1을 넣으면 자동으로 최신 UserId를 넣어줌
	return GameInstance->CreateLocalPlayer(-1, Error, true);

	return nullptr;
}

ALMBpawnPlayer* ALMBMyGameModeBase::SpawnAndPossessPawn(
	                                                    UWorld* World, 
	                                                    APlayerStart* PlayerStart, 
	                                                    APlayerController* PlayerController)
{
	ALMBpawnPlayer* NewPawn = World->SpawnActor<ALMBpawnPlayer>(
		                              LMBpawnPlayerClass, 
		                              PlayerStart->GetActorLocation(), 
		                              PlayerStart->GetActorRotation()
	                                  );
	check(NewPawn);
	
	NewPawn->SetPlayerIndex(CurrentPlayerIndex);

	// 플레이어 컨트롤러에 새로 생성한 Pawn을 빙의
	PlayerController->Possess(NewPawn);

	UE_LOG(LogTemp, Warning, TEXT("%d 번 가 스폰 됐습니다."), CurrentPlayerIndex);
    return nullptr;
}

