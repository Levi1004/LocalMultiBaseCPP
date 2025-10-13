// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/LMBPlayerController.h"
#include "UObject/ConstructorHelpers.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Pawn/LMBpawnPlayer.h"

ALMBPlayerController::ALMBPlayerController()
{
	// Input Mapping Context
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMCObj(TEXT("/Game/LMBCPP/Inputs/IMC_LMBPlayerInput.IMC_LMBPlayerInput"));
	if (IMCObj.Succeeded()) IMC_LMBPlayerInput = IMCObj.Object;

	// 이동
	static ConstructorHelpers::FObjectFinder<UInputAction> Move1PObj(TEXT("/Game/LMBCPP/Inputs/IA_Move1P.IA_Move1P"));
	if (Move1PObj.Succeeded()) IA_Move1P = Move1PObj.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> Move2PObj(TEXT("/Game/LMBCPP/Inputs/IA_Move2P.IA_Move2P"));
	if (Move2PObj.Succeeded()) IA_Move2P = Move2PObj.Object;

	// 1P 스킬 (U/I/J/K)
	static ConstructorHelpers::FObjectFinder<UInputAction> Skill1P_UObj(TEXT("/Game/LMBCPP/Inputs/IA_Skill1P_1.IA_Skill1P_1"));
	if (Skill1P_UObj.Succeeded()) IA_Skill1P_U = Skill1P_UObj.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> Skill1P_IObj(TEXT("/Game/LMBCPP/Inputs/IA_Skill1P_2.IA_Skill1P_2"));
	if (Skill1P_IObj.Succeeded()) IA_Skill1P_I = Skill1P_IObj.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> Skill1P_JObj(TEXT("/Game/LMBCPP/Inputs/IA_Skill1P_3.IA_Skill1P_3"));
	if (Skill1P_JObj.Succeeded()) IA_Skill1P_J = Skill1P_JObj.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> Skill1P_KObj(TEXT("/Game/LMBCPP/Inputs/IA_Skill1P_4.IA_Skill1P_4"));
	if (Skill1P_KObj.Succeeded()) IA_Skill1P_K = Skill1P_KObj.Object;

	// 2P 스킬 (NumPad 4/5/1/2)
	static ConstructorHelpers::FObjectFinder<UInputAction> Skill2P_4Obj(TEXT("/Game/LMBCPP/Inputs/IA_Skill2P_1.IA_Skill2P_1"));
	if (Skill2P_4Obj.Succeeded()) IA_Skill2P_4 = Skill2P_4Obj.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> Skill2P_5Obj(TEXT("/Game/LMBCPP/Inputs/IA_Skill2P_2.IA_Skill2P_2"));
	if (Skill2P_5Obj.Succeeded()) IA_Skill2P_5 = Skill2P_5Obj.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> Skill2P_1Obj(TEXT("/Game/LMBCPP/Inputs/IA_Skill2P_3.IA_Skill2P_3"));
	if (Skill2P_1Obj.Succeeded()) IA_Skill2P_1 = Skill2P_1Obj.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> Skill2P_2Obj(TEXT("/Game/LMBCPP/Inputs/IA_Skill2P_4.IA_Skill2P_4"));
	if (Skill2P_2Obj.Succeeded()) IA_Skill2P_2 = Skill2P_2Obj.Object;
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

	// 이동
	if (IA_Move1P)
	{
		EnhancedInputComponent->BindAction(IA_Move1P, ETriggerEvent::Triggered, this, &ALMBPlayerController::OnInputMove1P);
		EnhancedInputComponent->BindAction(IA_Move1P, ETriggerEvent::Completed, this, &ALMBPlayerController::OnInputMove1P);
	}
	if (IA_Move2P)
	{
		EnhancedInputComponent->BindAction(IA_Move2P, ETriggerEvent::Triggered, this, &ALMBPlayerController::OnInputMove2P);
		EnhancedInputComponent->BindAction(IA_Move2P, ETriggerEvent::Completed, this, &ALMBPlayerController::OnInputMove2P);
	}

	// 1P 스킬 (U/I/J/K)
	if (IA_Skill1P_U) EnhancedInputComponent->BindAction(IA_Skill1P_U, ETriggerEvent::Triggered, this, &ALMBPlayerController::OnSkill1P_U);
	if (IA_Skill1P_I) EnhancedInputComponent->BindAction(IA_Skill1P_I, ETriggerEvent::Triggered, this, &ALMBPlayerController::OnSkill1P_I);
	if (IA_Skill1P_J) EnhancedInputComponent->BindAction(IA_Skill1P_J, ETriggerEvent::Triggered, this, &ALMBPlayerController::OnSkill1P_J);
	if (IA_Skill1P_K) EnhancedInputComponent->BindAction(IA_Skill1P_K, ETriggerEvent::Triggered, this, &ALMBPlayerController::OnSkill1P_K);

	// 2P 스킬 (NumPad 4/5/1/2)
	if (IA_Skill2P_4) EnhancedInputComponent->BindAction(IA_Skill2P_4, ETriggerEvent::Triggered, this, &ALMBPlayerController::OnSkill2P_4);
	if (IA_Skill2P_5) EnhancedInputComponent->BindAction(IA_Skill2P_5, ETriggerEvent::Triggered, this, &ALMBPlayerController::OnSkill2P_5);
	if (IA_Skill2P_1) EnhancedInputComponent->BindAction(IA_Skill2P_1, ETriggerEvent::Triggered, this, &ALMBPlayerController::OnSkill2P_1);
	if (IA_Skill2P_2) EnhancedInputComponent->BindAction(IA_Skill2P_2, ETriggerEvent::Triggered, this, &ALMBPlayerController::OnSkill2P_2);
}

void ALMBPlayerController::AddPawnPlayer(ALMBpawnPlayer* NewPlayer)
{
	check(NewPlayer);
	if (!PawnPlayers.Contains(NewPlayer))
	{
		PawnPlayers.Add(NewPlayer);
		UE_LOG(LogTemp, Warning, TEXT("PawnPlayers에 %d 번 플레이어 추가"), NewPlayer->GetPlayerIndex());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PawnPlayers에 이미 추가된 플레이어입니다."));
	}
}

void ALMBPlayerController::OnInputMove1P(const FInputActionValue& Value)
{
	if (PawnPlayers.IsValidIndex(0))
		PawnPlayers[0]->OnInputMove(Value.Get<FVector2D>());
}

void ALMBPlayerController::OnInputMove2P(const FInputActionValue& Value)
{
	if (PawnPlayers.IsValidIndex(1))
		PawnPlayers[1]->OnInputMove(Value.Get<FVector2D>());
}

// 1P 스킬
void ALMBPlayerController::OnSkill1P_U() { if (PawnPlayers.IsValidIndex(0)) PawnPlayers[0]->UseSkill(0); } // Attack
void ALMBPlayerController::OnSkill1P_I() { if (PawnPlayers.IsValidIndex(0)) PawnPlayers[0]->UseSkill(1); } // Skill1
void ALMBPlayerController::OnSkill1P_J() { if (PawnPlayers.IsValidIndex(0)) PawnPlayers[0]->UseSkill(2); } // Skill2
void ALMBPlayerController::OnSkill1P_K() { if (PawnPlayers.IsValidIndex(0)) PawnPlayers[0]->UseSkill(3); } // Skill3

// 2P 스킬
void ALMBPlayerController::OnSkill2P_4() { if (PawnPlayers.IsValidIndex(1)) PawnPlayers[1]->UseSkill(0); } // Attack
void ALMBPlayerController::OnSkill2P_5() { if (PawnPlayers.IsValidIndex(1)) PawnPlayers[1]->UseSkill(1); } // Skill1
void ALMBPlayerController::OnSkill2P_1() { if (PawnPlayers.IsValidIndex(1)) PawnPlayers[1]->UseSkill(2); } // Skill2
void ALMBPlayerController::OnSkill2P_2() { if (PawnPlayers.IsValidIndex(1)) PawnPlayers[1]->UseSkill(3); } // Skill3
