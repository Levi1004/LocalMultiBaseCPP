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
	//  1P 스킬 (U, I, J, K)
	static ConstructorHelpers::FObjectFinder<UInputAction> Skill1P_1Obj(TEXT("/Game/LMBCPP/Inputs/IA_Skill1P_1.IA_Skill1P_1"));
	if (Skill1P_1Obj.Succeeded()) IA_Skill1P_1 = Skill1P_1Obj.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> Skill1P_2Obj(TEXT("/Game/LMBCPP/Inputs/IA_Skill1P_2.IA_Skill1P_2"));
	if (Skill1P_2Obj.Succeeded()) IA_Skill1P_2 = Skill1P_2Obj.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> Skill1P_3Obj(TEXT("/Game/LMBCPP/Inputs/IA_Skill1P_3.IA_Skill1P_3"));
	if (Skill1P_3Obj.Succeeded()) IA_Skill1P_3 = Skill1P_3Obj.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> Skill1P_4Obj(TEXT("/Game/LMBCPP/Inputs/IA_Skill1P_4.IA_Skill1P_4"));
	if (Skill1P_4Obj.Succeeded()) IA_Skill1P_4 = Skill1P_4Obj.Object;

	//  2P 스킬 (Numpad 4, 5, 1, 2)
	static ConstructorHelpers::FObjectFinder<UInputAction> Skill2P_1Obj(TEXT("/Game/LMBCPP/Inputs/IA_Skill2P_1.IA_Skill2P_1"));
	if (Skill2P_1Obj.Succeeded()) IA_Skill2P_1 = Skill2P_1Obj.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> Skill2P_2Obj(TEXT("/Game/LMBCPP/Inputs/IA_Skill2P_2.IA_Skill2P_2"));
	if (Skill2P_2Obj.Succeeded()) IA_Skill2P_2 = Skill2P_2Obj.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> Skill2P_3Obj(TEXT("/Game/LMBCPP/Inputs/IA_Skill2P_3.IA_Skill2P_3"));
	if (Skill2P_3Obj.Succeeded()) IA_Skill2P_3 = Skill2P_3Obj.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> Skill2P_4Obj(TEXT("/Game/LMBCPP/Inputs/IA_Skill2P_4.IA_Skill2P_4"));
	if (Skill2P_4Obj.Succeeded()) IA_Skill2P_4 = Skill2P_4Obj.Object;
	
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
	//  1P 스킬 입력 바인딩 (U, I, J, K)
	if (IA_Skill1P_1) EnhancedInputComponent->BindAction(IA_Skill1P_1, ETriggerEvent::Triggered, this, &ALMBPlayerController::OnSkill1P_1);
	if (IA_Skill1P_2) EnhancedInputComponent->BindAction(IA_Skill1P_2, ETriggerEvent::Triggered, this, &ALMBPlayerController::OnSkill1P_2);
	if (IA_Skill1P_3) EnhancedInputComponent->BindAction(IA_Skill1P_3, ETriggerEvent::Triggered, this, &ALMBPlayerController::OnSkill1P_3);
	if (IA_Skill1P_4) EnhancedInputComponent->BindAction(IA_Skill1P_4, ETriggerEvent::Triggered, this, &ALMBPlayerController::OnSkill1P_4);

	//  2P 스킬 입력 바인딩 (Numpad 4, 5, 1, 2)
	if (IA_Skill2P_1) EnhancedInputComponent->BindAction(IA_Skill2P_1, ETriggerEvent::Triggered, this, &ALMBPlayerController::OnSkill2P_1);
	if (IA_Skill2P_2) EnhancedInputComponent->BindAction(IA_Skill2P_2, ETriggerEvent::Triggered, this, &ALMBPlayerController::OnSkill2P_2);
	if (IA_Skill2P_3) EnhancedInputComponent->BindAction(IA_Skill2P_3, ETriggerEvent::Triggered, this, &ALMBPlayerController::OnSkill2P_3);
	if (IA_Skill2P_4) EnhancedInputComponent->BindAction(IA_Skill2P_4, ETriggerEvent::Triggered, this, &ALMBPlayerController::OnSkill2P_4);

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

//  1P 스킬
void ALMBPlayerController::OnSkill1P_1() { if (PawnPlayers.IsValidIndex(0)) PawnPlayers[0]->UseSkill(1); }
void ALMBPlayerController::OnSkill1P_2() { if (PawnPlayers.IsValidIndex(0)) PawnPlayers[0]->UseSkill(2); }
void ALMBPlayerController::OnSkill1P_3() { if (PawnPlayers.IsValidIndex(0)) PawnPlayers[0]->UseSkill(3); }
void ALMBPlayerController::OnSkill1P_4() { if (PawnPlayers.IsValidIndex(0)) PawnPlayers[0]->UseSkill(4); }

//  2P 스킬
void ALMBPlayerController::OnSkill2P_1() { if (PawnPlayers.IsValidIndex(1)) PawnPlayers[1]->UseSkill(1); }
void ALMBPlayerController::OnSkill2P_2() { if (PawnPlayers.IsValidIndex(1)) PawnPlayers[1]->UseSkill(2); }
void ALMBPlayerController::OnSkill2P_3() { if (PawnPlayers.IsValidIndex(1)) PawnPlayers[1]->UseSkill(3); }
void ALMBPlayerController::OnSkill2P_4() { if (PawnPlayers.IsValidIndex(1)) PawnPlayers[1]->UseSkill(4); }
