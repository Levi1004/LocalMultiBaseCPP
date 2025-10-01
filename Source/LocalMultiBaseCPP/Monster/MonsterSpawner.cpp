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
		MeshComp->SetRelativeLocation(FVector::ZeroVector);
		MeshComp->SetRelativeRotation(FRotator::ZeroRotator);
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
	CurrentTime = 0.f;
}


// Called every frame
void AMonsterSpawner::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
	
	CurrentTime += DeltaTime;
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

				// 전방 이동만 지정
				NewMonster->StartLocation = SpawnLocation;
				NewMonster->Direction = FVector(1, 0, 0); // 포탈 뒤쪽이 정면
				NewMonster->MovePhase = EMonsterMovePhase::InitialForward;
				NewMonster->bHasReachedDistance = false;
			}
		}
	}
	
}

