// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Pawn/LMBpawnPlayer.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	TSubclassOf<class ALMBpawnPlayer> BP_PlayerClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	TSubclassOf<class ALMBpawnPlayer> LMBpawnPlayerClass;

private:
	APlayerStart* FindPlayerStart(UWorld* CurrentWorld, const FName& TargetTag);
	void SpawnLocalPlayer(UWorld* World, APlayerStart* PlayerStart);
	ULocalPlayer* CreateLocalPlayer();
	ALMBpawnPlayer* SpawnAndPossessPawn(UWorld* World, APlayerStart* PlayerStart, APlayerController* PlayerController);

public:
	UFUNCTION(BlueprintCallable)
void DestroyAllPawnsBeforeLevelChange();

};