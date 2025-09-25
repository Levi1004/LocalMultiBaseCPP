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

protected:
	UPROPERTY(VisibleAnywhere, Category = "Player")
	TArray<class ALMBpawnPlayer*> PawnPlayers;

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

	public:
		UFUNCTION(BlueprintCallable, Category = "Player")
		void AddPawnPlayer(class ALMBpawnPlayer* NewPlayer);

private:
	void OnInputMove1P(const struct FInputActionValue& Value);
	void OnInputMove2P(const struct FInputActionValue& Value);
	void OnAttack1P();
	void OnAttack2P();
};
