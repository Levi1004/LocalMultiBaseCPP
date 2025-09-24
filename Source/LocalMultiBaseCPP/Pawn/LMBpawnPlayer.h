// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pawn/LMBpawnBase.h"
#include "LMBpawnPlayer.generated.h"

/**
 * 
 */
UCLASS()
class LOCALMULTIBASECPP_API ALMBpawnPlayer : public ALMBpawnBase
{
	GENERATED_BODY()
	
public:
	ALMBpawnPlayer();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
	int32 PlayerIndex = -1;

public:
	UFUNCTION(BlueprintCallable, Category = "Player")
	FORCEINLINE int32 GetPlayerIndex() const { return PlayerIndex; }

	UFUNCTION(BlueprintCallable, Category = "Player")
	FORCEINLINE void SetPlayerIndex(int32 NewIndex) { PlayerIndex = NewIndex; }
};
