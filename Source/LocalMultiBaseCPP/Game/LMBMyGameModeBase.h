// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LMBMyGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class LOCALMULTIBASECPP_API ALMBMyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ALMBMyGameModeBase();

protected:
	virtual void BeginPlay() override;

private:
	int32 MaxPlayerIndex = 2;
	int32 CurrentPlayerIndex = 0;

protected:
	UPROPERTY(VisibleAnywhere, Category = "PlayerStat")
	TArray<FName> PlayerStartTags = { TEXT("Start1P"), TEXT("Start2P") };

	UPROPERTY(VisibleAnywhere, Category = "Player")
	TSubclassOf<class ALMBpawnPlayer> LMBpawnPlayerClass;

private:
	class APlayerStart* FindPlayerStart(class UWorld* CurrentWorld, const FName& TargetTag);
	
};
