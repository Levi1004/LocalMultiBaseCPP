// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/LMBPlayerController.h"
#include "UObject/ConstructorHelpers.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Pawn/LMBpawnPlayer.h"

ALMBPlayerController::ALMBPlayerController()
{
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMCObj(TEXT(" / Game / LMBCPP / Inputs / IMC_LMBPlayerInput.IMC_LMBPlayerInput"));
	if (IMCObj.Succeeded())
	{
		IMC_LMBPlayerInput = IMCObj.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UInputAction> Move1PObj(TEXT("/Game/LMBCPP/Inputs/IA_Move1P.IA_Move1P"));
	if (Move1PObj.Succeeded())
	{
		IA_Move1P = Move1PObj.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> Move2PObj(TEXT("/Game/LMBCPP/Inputs/IA_Move2P.IA_Move2P"));
	if (Move2PObj.Succeeded())
	{
		IA_Move2P = Move2PObj.Object;
	}

}

void ALMBPlayerController::BeginPlay()
{
	Super::BeginPlay();

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem && IMC_LMBPlayerInput)
	{
		Subsystem->AddMappingContext(IMC_LMBPlayerInput, 0);
	}
}

void ALMBPlayerController::AddPawnPlayer(ALMBpawnPlayer* NewPlayer)
{
	check(NewPlayer);
	//contains : 배열에 포함중인지 체크
	if (!PawnPlayers.Contains(NewPlayer))
	{
		PawnPlayers.Add(NewPlayer);
		UE_LOG(LogTemp, Log, TEXT("PawnPalyers에 %d 번 플레이어 추가"),NewPlayer->GetPlayerIndex());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PawnPalyers에 이미 추가 된 플레이어 입니다."));
	}
}
