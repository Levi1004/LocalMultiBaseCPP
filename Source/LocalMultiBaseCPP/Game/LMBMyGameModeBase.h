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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ALMBpawnPlayer> LMBpawnPlayerClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	TSubclassOf<class ALMBpawnPlayer> BP_PlayerClass;

private:
	class APlayerStart* FindPlayerStart( UWorld* CurrentWorld, const FName& TargetTag);

	void SpawnLocalPlayer(UWorld* World, class APlayerStart* PlayerStart);

	ULocalPlayer* CreateLocalPlayer();

	class ALMBpawnPlayer* SpawnAndPossessPawn(
		UWorld* World, 
		class APlayerStart* PlayerStart, 
		class APlayerController* PlayerController);


};
