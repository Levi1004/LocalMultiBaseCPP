// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pawn/LMBpawnPlayer.h"
#include "ActorMonsterBase.generated.h"


class UBoxComponent;
class USkeletalMeshComponent;
class ALMBpawnPlayer;

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

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere)
	int32 TraceRate = 25;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Stats")
	int32 Hp = 100;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster Stats")
	float CurrentHp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Stats")
	int32 AttackPower = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ExperienceValue = 50; // 죽으면 주는 경험치

	UPROPERTY()
	ALMBpawnPlayer* LastDamageInstigator;


public:
	void ApplyDamage(float Damage, ALMBpawnPlayer* DamageInstigatorPlayer);
	// 데미지 처리

	void Die();
	
	
	// 이동 관련 =====================================
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
	float FreeMoveWaitTime = 0.f;
	FVector StartLocation;
	bool bHasReachedDistance = false;
	
	EMonsterMovePhase MovePhase = EMonsterMovePhase::InitialForward;


};
