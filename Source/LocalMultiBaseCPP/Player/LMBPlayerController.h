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


	UPROPERTY(VisibleAnywhere, Category = "Input")
	class UInputMappingContext* IMC_LMBPlayerInput;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	class UInputAction* IA_Move1P;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	class UInputAction* IA_Move2P;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	class UInputAction* IA_Attack1P;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	class UInputAction* IA_Attack2P;

private:
	void OnInputMove1P(const struct FInputActionValue& Value);
	void OnInputMove2P(const struct FInputActionValue& Value);
	void OnAttack1P();
	void OnAttack2P();
};
