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

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Game/Assets/Gobelin/Gobelin.Gobelin"));
	if (MeshAsset.Succeeded())
	{
		SkeletalMeshComp->SetSkeletalMesh(MeshAsset.Object);
		
		SkeletalMeshComp->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
		SkeletalMeshComp->SetRelativeRotation(FRotator::ZeroRotator);
		SkeletalMeshComp->SetWorldScale3D(FVector(1.0f));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SetSkeletalMesh 가져오지 못했습니다."));
	}
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
        // 전방 이동
        FVector MoveDir = Direction.GetSafeNormal();
        FVector NewLocation = GetActorLocation() + MoveDir * MoveSpeed * DeltaTime;
        float Traveled = (NewLocation - StartLocation).Size();

        if (Traveled >= MaxForwardDistance)
        {
            SetActorLocation(StartLocation + MoveDir * MaxForwardDistance);
            bHasReachedDistance = true;

            // 자유 이동 단계로 전환
            MovePhase = EMonsterMovePhase::FreeMovement;
            StartLocation = GetActorLocation();
            bHasFreeTarget = false;
            FreeMoveWaitTime = 0.f;
        }
        else
        {
            SetActorLocation(NewLocation);
        }

        // 전방 이동 중에도 방향 회전
        if (!MoveDir.IsNearlyZero())
        {
            FRotator TargetRot = MoveDir.Rotation();
            SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRot, DeltaTime, 5.f));
        }
    }
    else if (MovePhase == EMonsterMovePhase::FreeMovement)
    {
        if (!bHasFreeTarget)
        {
            // 새로운 목표 설정
            FVector RandomOffset = FVector(
                FMath::RandRange(-FreeMoveRange, FreeMoveRange),
                FMath::RandRange(-FreeMoveRange, FreeMoveRange),
                0.f
            );
            FreeMoveTarget = StartLocation + RandomOffset;
            bHasFreeTarget = true;
            FreeMoveWaitTime = 0.f;
        }

        FVector CurrentLocation = GetActorLocation();
        float DistToTarget = (FreeMoveTarget - CurrentLocation).Size();

        if (DistToTarget < 10.f)
        {
            // 목표 도달 → 대기
            FreeMoveWaitTime += DeltaTime;
            if (FreeMoveWaitTime >= 10.f)
            {
                bHasFreeTarget = false;
                FreeMoveWaitTime = 0.f;
            }
        }
        else
        {
            // 목표를 향해 부드럽게 이동
            FVector MoveDir = (FreeMoveTarget - CurrentLocation).GetSafeNormal();
            SetActorLocation(CurrentLocation + MoveDir * MoveSpeed / 2 * DeltaTime);

            // 이동 방향으로 자연스럽게 회전
            if (!MoveDir.IsNearlyZero())
            {
                FRotator TargetRot = MoveDir.Rotation();
                SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRot, DeltaTime, 5.f));
            }
        }
    }
	
}


