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


	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (MeshAsset.Succeeded())
	{
		MeshComp->SetStaticMesh(MeshAsset.Object);
		// pitch : Y
		// Yaw : Z
		// Roll : X
		MeshComp->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));	
		MeshComp->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Static Mesh를 가져오지 못했습니다."));
	}
}

// Called when the game starts or when spawned
void AMonsterSpawner::BeginPlay()
{
	Super::BeginPlay();
	
	SpawnOffsets.Add(FVector(200, 0, 0));
	SpawnOffsets.Add(FVector(200, 200, 0));
	SpawnOffsets.Add(FVector(0, 200, 0));
	SpawnOffsets.Add(FVector(-200, 200, 0));
	SpawnOffsets.Add(FVector(200, -200, 0));

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

		if (EnemyClass && SpawnOffsets.Num() > 0)
		{
			FVector SpawnLocation = GetActorLocation() + SpawnOffsets[CurrentSpawnIndex];
			FRotator SpawnRotation = GetActorRotation();

			AActorMonsterBase* NewMonster = GetWorld()->SpawnActor<AActorMonsterBase>(EnemyClass, SpawnLocation, SpawnRotation);

			if (NewMonster)
			{
				SpawnedMonsters.Add(NewMonster);
				CurrentSpawnIndex = (CurrentSpawnIndex + 1) % SpawnOffsets.Num();
			}



		}
	}
}

