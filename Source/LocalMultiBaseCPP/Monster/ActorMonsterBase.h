// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActorMonsterBase.generated.h"

UENUM(BlueprintType)
enum class EMonsterMovePhase : uint8
{
	InitialForward, 
	FreeMovement 
};

UCLASS()
class LOCALMULTIBASECPP_API AActorMonsterBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AActorMonsterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* BoxComp;
	
	UPROPERTY(VisibleAnywhere)
	class USkeletalMeshComponent* SkeletalMeshComp;

	UPROPERTY(EditAnywhere)
	int32 TraceRate = 25;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Stats")
	int32 Hp = 50;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Stats")
	int32 AttackPower = 3;

public:

	UPROPERTY(EditAnywhere, Category = "Movement")
	FVector Direction = FVector(1, 0, 0);

	UPROPERTY(EditAnywhere, Category = "Movement")
	float MoveSpeed = 300.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float MaxForwardDistance = 700.0f;
    UPROPERTY(EditAnywhere, Category = "Movement")
	float FreeMoveRange = 300.f;

	FVector FreeMoveTarget;
	bool bHasFreeTarget = false;
	
	FVector StartLocation;
	bool bHasReachedDistance = false;
	
	EMonsterMovePhase MovePhase = EMonsterMovePhase::InitialForward;








};
