// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BossMonster.h"
#include "Components/BoxComponent.h"
#include "Pawn/LMBpawnPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"

ABossMonster::ABossMonster()
{
    PrimaryActorTick.bCanEverTick = true;

    // SkeletalMeshComp는 기본 RootComponent로 설정됨
    SkeletalMeshComp = GetMesh();

    // HitBox를 SkeletalMeshComp의 자식으로 생성
    HitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBox"));
    HitBox->SetupAttachment(SkeletalMeshComp);
    HitBox->SetRelativeLocation(FVector(0.f, 0.f, 50.f)); // 초기 위치 조정
    HitBox->SetBoxExtent(FVector(50.f, 50.f, 100.f));
    HitBox->SetCollisionProfileName(TEXT("EnemyProfile"));

    bIsAttacking = false;

    // CharacterMovementComponent 속도 설정
    GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
}

void ABossMonster::BeginPlay()
{
    Super::BeginPlay();
    CurrentHp = MaxHp;
}

void ABossMonster::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    FindClosestPlayer();

    if (!TargetPlayer) return;

    FVector ToPlayer = TargetPlayer->GetActorLocation() - GetActorLocation();
    float Dist = ToPlayer.Size();

    // 방향 계산
    FVector Direction = ToPlayer;
    Direction.Z = 0.f;

    if (!Direction.IsNearlyZero())
    {
        Direction.Normalize();

        // 회전
        FRotator TargetRot = Direction.Rotation();
        SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRot, DeltaTime, 3.f));
    }

    if (Dist <= AttackRange && !bIsAttacking)
    {
        PerformAttack();
    }
    else if (Dist > AttackRange)
    {
        // AddMovementInput 사용 → Anim BP와 동기화
        AddMovementInput(Direction, 1.f);
    }
}

void ABossMonster::FindClosestPlayer()
{
    TArray<AActor*> FoundPlayers;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALMBpawnPlayer::StaticClass(), FoundPlayers);

    float ClosestDistSq = FLT_MAX;
    TargetPlayer = nullptr;
    FVector MyLocation = GetActorLocation();

    for (AActor* Actor : FoundPlayers)
    {
        float DistSq = FVector::DistSquared(Actor->GetActorLocation(), MyLocation);
        if (DistSq < ClosestDistSq)
        {
            ClosestDistSq = DistSq;
            TargetPlayer = Cast<ALMBpawnPlayer>(Actor);
        }
    }
}

void ABossMonster::PerformAttack()
{
    if (!TargetPlayer) return;

    bIsAttacking = true;

    // 몽타주 재생
    if (AttackMontage && SkeletalMeshComp)
    {
        UAnimInstance* AnimInstance = SkeletalMeshComp->GetAnimInstance();
        if (AnimInstance)
        {
            AnimInstance->Montage_Play(AttackMontage);
        }
    }

    // 공격 적용 (라인트레이스)
    FVector Start = GetActorLocation() + FVector(0.f, 0.f, 50.f);
    FVector End = Start + GetActorForwardVector() * AttackRange;

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
    {
        if (ALMBpawnPlayer* HitPlayer = Cast<ALMBpawnPlayer>(Hit.GetActor()))
        {
            HitPlayer->ApplyDamage(AttackPower, this);
        }
    }

    // 공격 후 다음 Tick에서 공격 가능
    GetWorldTimerManager().SetTimerForNextTick([this]()
        {
            bIsAttacking = false;
        });
}

void ABossMonster::ApplyDamage(float Damage, ALMBpawnPlayer* DamageInstigator)
{
    if (Damage <= 0.f) return;

    CurrentHp -= Damage;
    UE_LOG(LogTemp, Warning, TEXT("%s took %f damage. Remaining HP: %f"), *GetName(), Damage, CurrentHp);

    if (CurrentHp <= 0.f)
    {
        Die();
    }
}

void ABossMonster::Die()
{
    UE_LOG(LogTemp, Warning, TEXT("%s has died."), *GetName());
    Destroy();
}