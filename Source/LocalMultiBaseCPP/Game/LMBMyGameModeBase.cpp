// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/LMBMyGameModeBase.h"
#include "Player/LMBPlayerController.h"
#include "Pawn/LMBpawnPlayer.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/Button.h"
#include "Blueprint/UserWidget.h"
#include "Engine/GameInstance.h"


ALMBMyGameModeBase::ALMBMyGameModeBase()
{
	DefaultPawnClass = nullptr;
	PlayerControllerClass = ALMBPlayerController::StaticClass();
	LMBpawnPlayerClass = ALMBpawnPlayer::StaticClass();
}


void ALMBMyGameModeBase::BeginPlay() // 캐릭터 스폰
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

ALMBpawnPlayer* ALMBMyGameModeBase::SpawnAndPossessPawn(UWorld* World, APlayerStart* PlayerStart, APlayerController* PlayerController)
{
	if (!World || !PlayerStart || !PlayerController)
		return nullptr;

	// BP_PlayerClass가 지정되어 있으면 그걸로 스폰
	TSubclassOf<ALMBpawnPlayer> ClassToSpawn = BP_PlayerClass ? BP_PlayerClass : LMBpawnPlayerClass;

	ALMBpawnPlayer* NewPawn = World->SpawnActor<ALMBpawnPlayer>(
		ClassToSpawn,
		PlayerStart->GetActorLocation(),
		PlayerStart->GetActorRotation()
	);

	if (!NewPawn)
	{
		UE_LOG(LogTemp, Error, TEXT("Pawn spawn failed for player %d"), CurrentPlayerIndex);
		return nullptr;
	}

	NewPawn->SetPlayerIndex(CurrentPlayerIndex);
	NewPawn->InitializePlayerStats(CurrentPlayerIndex);
	PlayerController->Possess(NewPawn);

	UE_LOG(LogTemp, Warning, TEXT("%dP (%s)이 스폰되었습니다."), CurrentPlayerIndex + 1, *NewPawn->GetName());
	return NewPawn;
}

void ALMBMyGameModeBase::CheckGameOver()
{
	bool bAllDead = true;

	// 씬에 존재하는 모든 플레이어 Pawn 체크
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

		// 이미 UI가 띄워져 있다면 제거
		if (ActiveWidget)
		{
			ActiveWidget->RemoveFromParent();
			ActiveWidget = nullptr;
		}

		if (GameOverWidgetClass)
		{
			ActiveWidget = CreateWidget<UUserWidget>(GetWorld(), GameOverWidgetClass);
			if (ActiveWidget)
			{
				ActiveWidget->AddToViewport();
			}
		}

		// 게임 로직 정지 (옵션)
		UGameplayStatics::SetGamePaused(GetWorld(), true);
	}
}

void ALMBMyGameModeBase::ShowGameOverWidget()
{
	if (!GameOverWidgetClass) return;

	GameOverWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), GameOverWidgetClass);
	if (GameOverWidgetInstance)
	{
		GameOverWidgetInstance->AddToViewport();
		UE_LOG(LogTemp, Warning, TEXT("게임 오버 위젯 표시 완료"));

		// 버튼 바인딩 시도
		if (UButton* RestartButton = Cast<UButton>(GameOverWidgetInstance->GetWidgetFromName(TEXT("RestartButton"))))
		{
			RestartButton->OnClicked.AddDynamic(this, &ALMBMyGameModeBase::OnRestartClicked);
		}

		// 게임 정지 (선택)
		UGameplayStatics::SetGamePaused(GetWorld(), true);
	}
}

void ALMBMyGameModeBase::OnRestartClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("다시 시작 클릭"));
	UGameplayStatics::SetGamePaused(GetWorld(), false);

	FName CurrentLevel = *UGameplayStatics::GetCurrentLevelName(GetWorld());
	UGameplayStatics::OpenLevel(GetWorld(), CurrentLevel);
}
