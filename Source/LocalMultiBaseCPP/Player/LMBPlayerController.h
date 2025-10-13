// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "LMBPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class ALMBpawnBase;

/**
 * 
 */
UCLASS()
class LOCALMULTIBASECPP_API ALMBPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ALMBPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	// 등록된 플레이어 배열
	UPROPERTY()
	TArray<ALMBpawnPlayer*> PawnPlayers;

public:
	// 블루프린트에서 호출 가능
	UFUNCTION(BlueprintCallable, Category = "Player")
	void AddPawnPlayer(ALMBpawnPlayer* NewPlayer);

protected:
	// Input Mapping Context
	UPROPERTY(VisibleAnywhere, Category = "Input")
	UInputMappingContext* IMC_LMBPlayerInput;

	// 이동
	UPROPERTY(VisibleAnywhere, Category = "Input")
	UInputAction* IA_Move1P;
	UPROPERTY(VisibleAnywhere, Category = "Input")
	UInputAction* IA_Move2P;

	// 1P 스킬 (U/I/J/K)
	UPROPERTY(VisibleAnywhere, Category = "Input")
	UInputAction* IA_Skill1P_U;
	UPROPERTY(VisibleAnywhere, Category = "Input")
	UInputAction* IA_Skill1P_I;
	UPROPERTY(VisibleAnywhere, Category = "Input")
	UInputAction* IA_Skill1P_J;
	UPROPERTY(VisibleAnywhere, Category = "Input")
	UInputAction* IA_Skill1P_K;

	// 2P 스킬 (NumPad 4/5/1/2)
	UPROPERTY(VisibleAnywhere, Category = "Input")
	UInputAction* IA_Skill2P_4;
	UPROPERTY(VisibleAnywhere, Category = "Input")
	UInputAction* IA_Skill2P_5;
	UPROPERTY(VisibleAnywhere, Category = "Input")
	UInputAction* IA_Skill2P_1;
	UPROPERTY(VisibleAnywhere, Category = "Input")
	UInputAction* IA_Skill2P_2;

private:
	// 이동
	void OnInputMove1P(const FInputActionValue& Value);
	void OnInputMove2P(const FInputActionValue& Value);

	// 1P 스킬
	void OnSkill1P_U(); // Attack
	void OnSkill1P_I(); // Skill1
	void OnSkill1P_J(); // Skill2
	void OnSkill1P_K(); // Skill3

	// 2P 스킬
	void OnSkill2P_4(); // Attack
	void OnSkill2P_5(); // Skill1
	void OnSkill2P_1(); // Skill2
	void OnSkill2P_2(); // Skill3
};