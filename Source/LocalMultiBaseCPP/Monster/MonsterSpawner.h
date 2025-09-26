// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MonsterSpawner.generated.h"

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
	UPROPERTY(EditAnywhere, Category = "DealyTime")
	float DelayTime = 10.0f;
	
private:
	float CurrentTime = 0.0f;

public:

	UPROPERTY(EditAnywhere)
	class UBoxComponent* BoxComp;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* MeshComp;

	UPROPERTY(EditAnywhere)
	int32 TraceRate = 25;

	UPROPERTY(EditAnywhere)
	float MoveSpeed = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Spawner")
	TSubclassOf<class AActorMonsterBase> EnemyClass;



};
