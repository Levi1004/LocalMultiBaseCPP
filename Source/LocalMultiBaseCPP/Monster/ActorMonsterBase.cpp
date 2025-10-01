// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/ActorMonsterBase.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"


// Sets default values
AActorMonsterBase::AActorMonsterBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("My Box Component"));
	SetRootComponent(BoxComp);

	FVector BoxSize = FVector(50.0f, 50.0f, 50.0f);
	BoxComp->SetBoxExtent(BoxSize);
	BoxComp->SetCollisionProfileName(TEXT("EnemyProfile"));
	
	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("My Skeletal Mesh"));
	SkeletalMeshComp->SetupAttachment(BoxComp);

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT(" / Game / Assets / Goblin / Goblin.Goblin"));
	if (MeshAsset.Succeeded())
	{
		SkeletalMeshComp->SetSkeletalMesh(MeshAsset.Object);
		
		SkeletalMeshComp->SetRelativeRotation(FRotator(0.f, 0.f, -90.f));
		SkeletalMeshComp->SetRelativeRotation(FRotator::ZeroRotator);
		SkeletalMeshComp->SetWorldScale3D(FVector(1.0f));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SetSkeletalMesh 가져오지 못했습니다."));
	}
	/*static ConstructorHelpers::FObjectFinder<UMaterial> MatAsset(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	if (MatAsset.Succeeded())
	{
		SkeletalMeshComp->SetMaterial(0, MatAsset.Object);
	}*/
}

// Called when the game starts or when spawned
void AActorMonsterBase::BeginPlay()
{
	Super::BeginPlay();
	
	StartLocation = GetActorLocation();
	bHasReachedDistance = false;
	MovePhase = EMonsterMovePhase::InitialForward;

	SetActorRotation(Direction.Rotation());
}

// Called every frame
void AActorMonsterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (MovePhase == EMonsterMovePhase::InitialForward && !bHasReachedDistance)
	{
		FVector MoveDir = Direction.GetSafeNormal();
		FVector NewLocation = GetActorLocation() + MoveDir * MoveSpeed * DeltaTime;
		float Traveled = (NewLocation - StartLocation).Size();

		if (Traveled >= MaxForwardDistance)
		{
			SetActorLocation(StartLocation + MoveDir * MaxForwardDistance);
			bHasReachedDistance = true;

			// 전방 이동 완료 → 자유 이동 단계
			MovePhase = EMonsterMovePhase::FreeMovement;
			StartLocation = GetActorLocation();
		}
		else
		{
			SetActorLocation(NewLocation);
		}
	}
	else if (MovePhase == EMonsterMovePhase::FreeMovement)
	{
		if (!bHasFreeTarget || (GetActorLocation() - FreeMoveTarget).Size() < 10.f)
		{
			// 새로운 목표 지정
			FVector RandomOffset = FVector(
				FMath::RandRange(-FreeMoveRange, FreeMoveRange),
				FMath::RandRange(-FreeMoveRange, FreeMoveRange),
				0
			);
			FreeMoveTarget = StartLocation + RandomOffset;
			bHasFreeTarget = true;
		}

		// 목표로 이동
		FVector MoveDir = (FreeMoveTarget - GetActorLocation()).GetSafeNormal();
		SetActorLocation(GetActorLocation() + MoveDir * MoveSpeed / 2 * DeltaTime);
	}
}


