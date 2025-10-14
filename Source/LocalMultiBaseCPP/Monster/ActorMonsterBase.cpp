// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/ActorMonsterBase.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Pawn/LMBpawnPlayer.h"


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
	
    CurrentHp = Hp;
	StartLocation = GetActorLocation();
	bHasReachedDistance = false;
	MovePhase = EMonsterMovePhase::InitialForward;

	SetActorRotation(Direction.Rotation());

}

// Called every frame
void AActorMonsterBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // --- [1] 씬에 있는 모든 플레이어 가져오기 ---
    TArray<AActor*> FoundPlayers;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALMBpawnPlayer::StaticClass(), FoundPlayers);
    if (FoundPlayers.Num() == 0) return;

    // --- [2] 가장 가까운 플레이어 탐색 ---
    AActor* ClosestPlayer = nullptr;
    float ClosestDistSq = FLT_MAX;
    FVector MyLocation = GetActorLocation();

    for (AActor* Player : FoundPlayers)
    {
        float DistSq = FVector::DistSquared(Player->GetActorLocation(), MyLocation);
        if (DistSq < ClosestDistSq)
        {
            ClosestDistSq = DistSq;
            ClosestPlayer = Player;
        }
    }

    if (!ClosestPlayer) return;

    // --- [3] 가장 가까운 플레이어 방향으로 이동 ---
    FVector ToPlayer = (ClosestPlayer->GetActorLocation() - MyLocation);
    ToPlayer.Z = 0; // Yaw 회전만 적용
    FVector MoveDir = ToPlayer.GetSafeNormal();

    if (!MoveDir.IsNearlyZero())
    {
        // 회전 (부드럽게 플레이어를 향하도록)
        FRotator TargetRot = MoveDir.Rotation();
        SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRot, DeltaTime, 3.f));

        // 이동 (일정 거리 이상일 때만)
        float DistToPlayer = ToPlayer.Size();
        if (DistToPlayer > 150.f) // 너무 가까우면 멈춤
        {
            SetActorLocation(MyLocation + MoveDir * MoveSpeed * DeltaTime);
        }
    }

    // --- [4] 디버그 시각화 ---
    DrawDebugLine(GetWorld(), MyLocation, ClosestPlayer->GetActorLocation(), FColor::Red, false, -1.f, 0, 2.f);
}



void AActorMonsterBase::ApplyDamage(float Damage, ALMBpawnPlayer* DamageInstigator)
{
    if (Damage <= 0.f) return;

    CurrentHp -= Damage;
    LastDamageInstigator = DamageInstigator;

    UE_LOG(LogTemp, Warning, TEXT("%s took %f damage. Remaining HP: %f"), *GetName(), Damage, CurrentHp);

    if (CurrentHp <= 0.f)
    {
        Die();
    }
}

void AActorMonsterBase::Die()
{
    UE_LOG(LogTemp, Warning, TEXT("%s 사망"), *GetName());

    if (LastDamageInstigator)
    {
        LastDamageInstigator->AddExperience(ExperienceValue);
    }

    Destroy();
}

