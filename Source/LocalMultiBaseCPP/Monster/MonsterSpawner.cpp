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

	FVector BoxSize = FVector(50.0f, 50.0f, 50.0f);
	BoxComp->SetBoxExtent(BoxSize);
	BoxComp->SetCollisionProfileName(TEXT("EnemyProfile"));

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("My Static Mesh"));
	MeshComp->SetupAttachment(BoxComp);


	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/Assets/simple-stone-portal/textures/SM_Monster_Portal.SM_Monster_Portal"));
	if (MeshAsset.Succeeded())
	{
		MeshComp->SetStaticMesh(MeshAsset.Object);
		
		MeshComp->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));	
		MeshComp->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Static Mesh를 가져오지 못했습니다."));
	}
	static ConstructorHelpers::FObjectFinder<UMaterial> MatAsset(TEXT("/Game/Assets/simple-stone-portal/textures/SM_Monster_Portal.SM_Monster_Portal"));
	if (MatAsset.Succeeded())
	{
		MeshComp->SetMaterial(0, MatAsset.Object);
	}
}

// Called when the game starts or when spawned
void AMonsterSpawner::BeginPlay()
{
	Super::BeginPlay();
	
	SpawnOffsets.Add(FVector(0, 0, 0));
	
	

	CurrentSpawnIndex = 0;
	CurrentTime = 0.0f;
	DelayTime = 2.0f;

}


// Called every frame
void AMonsterSpawner::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
	
	CurrentTime += DeltaTime;
	
	if (CurrentTime >= DelayTime && SpawnedMonsters.Num() < MaxSpawnCount)
	{
		CurrentTime = 0.0f;

		if (EnemyClass)
		{
			// 1️ 포탈 위치에서 Spawn
			FVector SpawnLocation = GetActorLocation();
			FRotator SpawnRotation = GetActorRotation();

			AActorMonsterBase* NewMonster = GetWorld()->SpawnActor<AActorMonsterBase>(EnemyClass, SpawnLocation, SpawnRotation);
			if (NewMonster)
			{
				SpawnedMonsters.Add(NewMonster);

				// 2️ Spawn 직후 포탈 앞으로 전진
				FVector ForwardOffset = NewMonster->GetActorForwardVector() * 100.0f; // 100 유니트 전진
				NewMonster->SetActorLocation(SpawnLocation + ForwardOffset);

				// 3️ 이후 퍼지도록 랜덤 방향 지정
				float Angle = FMath::RandRange(0.0f, 2 * PI);
				NewMonster->Direction = FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0);

				// 4️ Tick 이동용 초기값
				NewMonster->StartLocation = NewMonster->GetActorLocation();
				NewMonster->bHasReachedDistance = false;
			}
		}
	}
}

