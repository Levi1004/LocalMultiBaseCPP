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

void ALMBMyGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	UWorld* CurrentWorld = GetWorld();
	check(CurrentWorld);

	// 플레이어 스폰
	for (int32 i = 0; i < MaxPlayerIndex; i++)
	{
		APlayerStart* FoundStart = FindPlayerStart(CurrentWorld, PlayerStartTags[i]);
		SpawnLocalPlayer(CurrentWorld, FoundStart);
	}
}

APlayerStart* ALMBMyGameModeBase::FindPlayerStart(UWorld* CurrentWorld, const FName& TargetTag)
{
	check(CurrentWorld);

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

	// 기존 Pawn Destroy (중복 방지)
	if (PlayerController->GetPawn())
	{
		PlayerController->GetPawn()->Destroy();
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
	SpawnParams.Name = FName(*FString::Printf(TEXT("PlayerPawn%d"), CurrentPlayerIndex + 1));

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

	UE_LOG(LogTemp, Warning, TEXT("%dP (%s)이 스폰되었습니다."), CurrentPlayerIndex + 1, *NewPawn->GetName());
	return NewPawn;
}

void ALMBMyGameModeBase::CheckGameOver()
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

		ShowGameOverWidget();

		// 게임 일시정지
		UGameplayStatics::SetGamePaused(GetWorld(), true);
	}
}

void ALMBMyGameModeBase::ShowGameOverWidget()
{
	if (!GameOverWidgetClass) return;

	if (ActiveWidget)
	{
		ActiveWidget->RemoveFromParent();
		ActiveWidget = nullptr;
	}

	ActiveWidget = CreateWidget<UUserWidget>(GetWorld(), GameOverWidgetClass);
	if (!ActiveWidget) return;

	ActiveWidget->AddToViewport();

	// 버튼 바인딩
	if (UButton* RestartButton = Cast<UButton>(ActiveWidget->GetWidgetFromName(TEXT("RestartButton"))))
	{
		RestartButton->OnClicked.AddDynamic(this, &ALMBMyGameModeBase::OnGoToMainMenuClicked);
	}
}

void ALMBMyGameModeBase::OnGoToMainMenuClicked()
{
	UGameplayStatics::SetGamePaused(GetWorld(), false);

	// MainMenu 레벨로 이동
	UGameplayStatics::OpenLevel(GetWorld(), FName("MainMenu"));
}