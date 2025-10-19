// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/MonsterSpawner.h"
#include "ActorMonsterBase.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"




// Sets default values
AMonsterSpawner::AMonsterSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("My Box Component"));
	SetRootComponent(BoxComp);

	FVector BoxSize = FVector(100.0f, 100.0f, -200.0f);
	BoxComp->SetBoxExtent(BoxSize);
	BoxComp->SetCollisionProfileName(TEXT("EnemyProfile"));

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("My Static Mesh"));
	MeshComp->SetupAttachment(BoxComp);


	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/Assets/SM_MERGED_StaticMeshActor_7.SM_MERGED_StaticMeshActor_7"));
	if (MeshAsset.Succeeded())
	{
		MeshComp->SetStaticMesh(MeshAsset.Object);
		MeshComp->SetRelativeLocation(FVector::ZeroVector);
		MeshComp->SetRelativeRotation(FRotator::ZeroRotator);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Static Mesh를 가져오지 못했습니다."));
	}
	static ConstructorHelpers::FObjectFinder<UMaterial> MatAsset(TEXT("/Game/Assets/SM_MERGED_StaticMeshActor_7.SM_MERGED_StaticMeshActor_7"));
	if (MatAsset.Succeeded())
	{
		MeshComp->SetMaterial(0, MatAsset.Object);
	}

}

// Called when the game starts or when spawned
void AMonsterSpawner::BeginPlay()
{
	Super::BeginPlay();
	CurrentTime = 0.f;
	TotalMonsterKilled = 0;
	SpawnedMonsterHpMultiplier = 1.f;
	SpawnedMonsterAttackMultiplier = 1.f;
}


// Called every frame
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

	// 3분(180초) 지나면 스폰 중단
	if (CurrentTime >= MaxSpawnDuration)
	{
		return; // 더 이상 스폰하지 않음
	}

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

		// 100마리마다 스탯 증가
		if (TotalMonsterKilled % 10 == 0)
		{
			SpawnedMonsterHpMultiplier += 0.2f;      // 예: HP 20% 증가
			SpawnedMonsterAttackMultiplier += 0.1f;  // 공격력 10% 증가
			UE_LOG(LogTemp, Warning, TEXT("몬스터 스탯 증가! HP x%.1f / Attack x%.1f"),
				SpawnedMonsterHpMultiplier, SpawnedMonsterAttackMultiplier);
		}
	}
}
