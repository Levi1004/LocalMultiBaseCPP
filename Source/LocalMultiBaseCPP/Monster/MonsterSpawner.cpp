// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/MonsterSpawner.h"
#include "ActorMonsterBase.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/StaticMesh.h"
#include "Materials/Material.h"

AMonsterSpawner::AMonsterSpawner()
{
    PrimaryActorTick.bCanEverTick = true;

    // BoxComponent 생성 및 RootComponent 설정
    BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
    SetRootComponent(BoxComp);
    BoxComp->SetBoxExtent(FVector(100.f, 100.f, 200.f));
    BoxComp->SetCollisionProfileName(TEXT("EnemyProfile"));

    // MeshComponent 생성 및 BoxComponent에 부착
    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    MeshComp->SetupAttachment(BoxComp);

    // Static Mesh 할당
    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/Assets/SM_MERGED_StaticMeshActor_7.SM_MERGED_StaticMeshActor_7"));
    if (MeshAsset.Succeeded())
    {
        MeshComp->SetStaticMesh(MeshAsset.Object);
        MeshComp->SetRelativeLocation(FVector::ZeroVector);
        MeshComp->SetRelativeRotation(FRotator::ZeroRotator);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Static Mesh를 가져오지 못했습니다! 경로 확인 필요."));
    }

    // Material 할당 (Mesh와 다른 Material일 경우)
    static ConstructorHelpers::FObjectFinder<UMaterial> MatAsset(TEXT("/Game/Assets/Materials/M_MyMaterial.M_MyMaterial"));
    if (MatAsset.Succeeded())
    {
        MeshComp->SetMaterial(0, MatAsset.Object);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Material을 가져오지 못했습니다!"));
    }
}

void AMonsterSpawner::BeginPlay()
{
    Super::BeginPlay();
    CurrentTime = 0.f;
    TotalMonsterKilled = 0;
    SpawnedMonsterHpMultiplier = 1.f;
    SpawnedMonsterAttackMultiplier = 1.f;
}

void AMonsterSpawner::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 이미 Destroy된 몬스터 제거
    for (int32 i = SpawnedMonsters.Num() - 1; i >= 0; --i)
    {
        if (!IsValid(SpawnedMonsters[i]))
        {
            SpawnedMonsters.RemoveAt(i);
        }
    }

    // 누적 시간 증가
    CurrentTime += DeltaTime;

    // 최대 스폰 시간 체크
    if (CurrentTime >= MaxSpawnDuration)
        return;

    // DelayTime마다 스폰
    if (CurrentTime >= DelayTime && SpawnedMonsters.Num() < MaxSpawnCount)
    {
        CurrentTime = 0.f;

        if (EnemyClass)
        {
            FVector SpawnLocation = GetActorLocation();
            FRotator SpawnRotation = GetActorRotation();

            AActorMonsterBase* NewMonster = GetWorld()->SpawnActor<AActorMonsterBase>(EnemyClass, SpawnLocation, SpawnRotation);
            if (NewMonster)
            {
                SpawnedMonsters.Add(NewMonster);
                NewMonster->StartLocation = SpawnLocation;
                NewMonster->Direction = FVector(1, 0, 0);
                NewMonster->MovePhase = EMonsterMovePhase::InitialForward;
                NewMonster->bHasReachedDistance = false;

                NewMonster->Hp = FMath::RoundToInt(NewMonster->Hp * SpawnedMonsterHpMultiplier);
                NewMonster->AttackPower = FMath::RoundToInt(NewMonster->AttackPower * SpawnedMonsterAttackMultiplier);

                NewMonster->OnDestroyed.AddDynamic(this, &AMonsterSpawner::OnMonsterDestroyed);
            }
        }
    }
}

void AMonsterSpawner::OnMonsterDestroyed(AActor* DestroyedActor)
{
    AActorMonsterBase* DestroyedMonster = Cast<AActorMonsterBase>(DestroyedActor);
    if (DestroyedMonster)
    {
        SpawnedMonsters.Remove(DestroyedMonster);
        TotalMonsterKilled++;

        UE_LOG(LogTemp, Warning, TEXT("몬스터 처치! 총 킬 수: %d"), TotalMonsterKilled);

        // 10마리마다 스탯 증가
        if (TotalMonsterKilled % 10 == 0)
        {
            SpawnedMonsterHpMultiplier += 0.2f;
            SpawnedMonsterAttackMultiplier += 0.1f;
            UE_LOG(LogTemp, Warning, TEXT("몬스터 스탯 증가! HP x%.1f / Attack x%.1f"),
                SpawnedMonsterHpMultiplier, SpawnedMonsterAttackMultiplier);
        }
    }
}