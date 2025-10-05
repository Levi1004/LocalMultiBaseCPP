// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/LMBPlayerController.h"
#include "UObject/ConstructorHelpers.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Pawn/LMBpawnPlayer.h"

ALMBPlayerController::ALMBPlayerController()
{
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMCObj(TEXT("/Game/LMBCPP/Inputs/IMC_LMBPlayerInput.IMC_LMBPlayerInput"));
	if (IMCObj.Succeeded())
	{
		IMC_LMBPlayerInput = IMCObj.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("할당되지 않았습니다."));
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

	static ConstructorHelpers::FObjectFinder<UInputAction> Attack1PObj(TEXT("/Game/LMBCPP/Inputs/IA_Attack1P.IA_Attack1P"));
	if (Attack1PObj.Succeeded())
	{
		IA_Attack1P = Attack1PObj.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> Attack2PObj(TEXT("/Game/LMBCPP/Inputs/IA_Attack2P.IA_Attack2P"));
	if (Attack2PObj.Succeeded())
	{
		IA_Attack2P = Attack2PObj.Object;
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
	for (int32 i = 0; i < PawnPlayers.Num(); i++)
	{
		PawnPlayers[i]->SetPlayerIndex(i);
		PawnPlayers[i]->ApplyMeshByPlayerIndex();
	}
}


void ALMBPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	check(EnhancedInputComponent);

	if (IA_Move1P) //IA_Move1P != nullptr
	{
		EnhancedInputComponent->BindAction(IA_Move1P, ETriggerEvent::Triggered, this, &ALMBPlayerController::OnInputMove1P);
		EnhancedInputComponent->BindAction(IA_Move1P, ETriggerEvent::Completed, this, &ALMBPlayerController::OnInputMove1P);
	}
	if (IA_Move2P) //IA_Move1P != nullptr
	{
		EnhancedInputComponent->BindAction(IA_Move2P, ETriggerEvent::Triggered, this, &ALMBPlayerController::OnInputMove2P);
		EnhancedInputComponent->BindAction(IA_Move2P, ETriggerEvent::Completed, this, &ALMBPlayerController::OnInputMove2P);
	}
	if (IA_Attack1P)
	{
		EnhancedInputComponent->BindAction(IA_Attack1P, ETriggerEvent::Triggered, this, &ALMBPlayerController::OnAttack1P);
		
	}
	if (IA_Attack2P)
	{
		EnhancedInputComponent->BindAction(IA_Attack2P, ETriggerEvent::Triggered, this, &ALMBPlayerController::OnAttack2P);
		
	}
}

void ALMBPlayerController::AddPawnPlayer(ALMBpawnPlayer* NewPlayer)
{
	check(NewPlayer);
	//contains : 배열에 포함중인지 체크
	if (!PawnPlayers.Contains(NewPlayer))
	{
		PawnPlayers.Add(NewPlayer);
		UE_LOG(LogTemp, Warning, TEXT("PawnPalyers에 %d 번 플레이어 추가"), NewPlayer->GetPlayerIndex());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PawnPalyers에 이미 추가 된 플레이어 입니다."));
	}
}

void ALMBPlayerController::OnInputMove1P(const FInputActionValue& Value)
{
	PawnPlayers[0]->OnInputMove(Value.Get<FVector2D>());
}

void ALMBPlayerController::OnInputMove2P(const FInputActionValue& Value)
{
	PawnPlayers[1]->OnInputMove(Value.Get<FVector2D>());
}

void ALMBPlayerController::OnAttack1P()
{
	PawnPlayers[0]->StartAttack();
}

void ALMBPlayerController::OnAttack2P()
{
	PawnPlayers[1]->StartAttack();
}
