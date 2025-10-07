// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LMBPlayerController.generated.h"


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

	UPROPERTY()
	TArray<ALMBpawnPlayer*> PawnPlayers;
public:
	// 블루프린트에서 호출 가능
	UFUNCTION(BlueprintCallable, Category = "Player")
	void AddPawnPlayer(ALMBpawnPlayer* NewPlayer);
	
protected:

	// 1P 스킬 키 (U, I, J, K)
	UPROPERTY(VisibleAnywhere, Category = "Input")
	class UInputMappingContext* IMC_LMBPlayerInput;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	class UInputAction* IA_Move1P;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	class UInputAction* IA_Move2P;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	UInputAction* IA_Skill1P_1;
	UPROPERTY(VisibleAnywhere, Category = "Input")
	UInputAction* IA_Skill1P_2;
	UPROPERTY(VisibleAnywhere, Category = "Input")
	UInputAction* IA_Skill1P_3;
	UPROPERTY(VisibleAnywhere, Category = "Input")
	UInputAction* IA_Skill1P_4;

	//  2P 스킬 (Numpad 4, 5, 1, 2)
	UPROPERTY(VisibleAnywhere, Category = "Input")
	UInputAction* IA_Skill2P_1;
	UPROPERTY(VisibleAnywhere, Category = "Input")
	UInputAction* IA_Skill2P_2;
	UPROPERTY(VisibleAnywhere, Category = "Input")
	UInputAction* IA_Skill2P_3;
	UPROPERTY(VisibleAnywhere, Category = "Input")
	UInputAction* IA_Skill2P_4;

private:
	void OnInputMove1P(const struct FInputActionValue& Value);
	void OnInputMove2P(const struct FInputActionValue& Value);

	void OnSkill1P_1();
	void OnSkill1P_2();
	void OnSkill1P_3();
	void OnSkill1P_4();

	void OnSkill2P_1();
	void OnSkill2P_2();
	void OnSkill2P_3();
	void OnSkill2P_4();
};
