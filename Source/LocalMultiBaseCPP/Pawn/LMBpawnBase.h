// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "LMBpawnBase.generated.h"

UCLASS()
class LOCALMULTIBASECPP_API ALMBpawnBase : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ALMBpawnBase();



protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* BoxComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USkeletalMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UFloatingPawnMovement* PawnMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MaxSpeed = 300.0f;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerName")
	FString PlayerName = TEXT("Adventurer");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHealth = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float CurrentHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float AttackPower = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Defence = 5.0f;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 Level = 1;

    FORCEINLINE class UFloatingPawnMovement* GetMovement() { return PawnMovement; };

public:
	virtual void BeginPlay() override;

	void SetPlayerIndex(int32 NewIndex) { PlayerIndex = NewIndex; }
	int32 GetPlayerIndex() const { return PlayerIndex; }

	void ApplyMeshByPlayerIndex();
protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player", meta = (AllowPrivateAccess = "true"))
	int32 PlayerIndex = -1;


};
