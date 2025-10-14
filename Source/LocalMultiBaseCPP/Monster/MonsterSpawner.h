// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MonsterSpawner.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
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

    // 몬스터 스폰 관련
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
    TSubclassOf<AActorMonsterBase> EnemyClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
    float DelayTime = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
    int32 MaxSpawnCount = 25;

    // 몬스터 스폰 위치 오프셋
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
    TArray<FVector> SpreadOffsets;

    // 콜리전 및 시각용 메쉬
    UPROPERTY(VisibleAnywhere)
    UBoxComponent* BoxComp;

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* MeshComp;

private:
    // 내부 관리용
    float CurrentTime = 0.0f;

    UPROPERTY()
    TArray<AActorMonsterBase*> SpawnedMonsters;

    // 몬스터가 파괴될 때 호출
    UFUNCTION()
    void OnMonsterDestroyed(AActor* DestroyedActor);

    int32 TotalMonsterKilled = 0; // 전체 죽은 몬스터 수
    float SpawnedMonsterHpMultiplier = 1.f; // 스탯 증가용 곱셈
    float SpawnedMonsterAttackMultiplier = 1.f; // 공격력 증가용
};