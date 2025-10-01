// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MonsterSpawner.generated.h"

class AActorMonsterBase;

UCLASS()
class LOCALMULTIBASECPP_API AMonsterSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMonsterSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

  

protected:
	UPROPERTY(EditAnywhere, Category = "DelayTime")
	float DelayTime = 3.0f;
	
private:
	float CurrentTime = 0.0f;
	TArray<FVector> SpreadDirections;
	TArray<AActorMonsterBase*> SpawnedMonsters;
public:
	UPROPERTY(EditAnywhere)
	class UBoxComponent* BoxComp;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* MeshComp;

	UPROPERTY(EditAnywhere, Category = "Spawner")
	TSubclassOf<AActorMonsterBase> EnemyClass;

	UPROPERTY(EditAnywhere, Category = "SpawnCount")
	int32 MaxSpawnCount = 5;

    UPROPERTY(EditAnywhere, Category = "Spawner")
	TArray<FVector> SpreadOffsets;
};
