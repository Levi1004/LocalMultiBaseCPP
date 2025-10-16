// Fill out your copyright notice in the Description page of Project Settings.

#include "BossMonster.h"
#include "Components/BoxComponent.h"
#include "Pawn/LMBpawnPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Animation/AnimInstance.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"

ABossMonster::ABossMonster()
{
    PrimaryActorTick.bCanEverTick = true;

    // HitBox
    HitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBox"));
    HitBox->SetupAttachment(RootComponent);
    HitBox->SetBoxExtent(FVector(100.f, 100.f, 100.f));
    HitBox->SetRelativeLocation(FVector(50.f, 0.f, 50.f));

    // SkeletalMesh
    USkeletalMeshComponent* MeshComp = GetMesh();
    MeshComp->SetupAttachment(RootComponent);
    MeshComp->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
    MeshComp->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

    // 이동 속도
    GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;

    // 자동 회전 설정
    bUseControllerRotationYaw = false;                        // 컨트롤러 Yaw 회전 비활성
    GetCharacterMovement()->bOrientRotationToMovement = true; // 이동 방향으로 회전
    GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f); // 회전 속도

    // 초기화
    CurrentHp = MaxHp;
    bCanAttack = true;
    bIsAttacking = false;
    TargetPlayer = nullptr;
}

void ABossMonster::BeginPlay()
{
    Super::BeginPlay();
    CurrentHp = MaxHp;
}

void ABossMonster::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!TargetPlayer)
    {
        FindClosestPlayer();
    }
    else
    {
        MoveTowardsPlayer(DeltaTime);
    }


    if (bIsDead) return; // 죽으면 아무것도 안 함

    // 타겟이 없거나 죽어있으면 새 타겟 탐색
    if (!TargetPlayer || TargetPlayer->IsDead())
    {
        TargetPlayer = nullptr;
        FindClosestPlayer();
    }

    if (TargetPlayer)
    {
        MoveTowardsPlayer(DeltaTime);
    }
}


void ABossMonster::FindClosestPlayer()
{
    TArray<AActor*> FoundPlayers;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALMBpawnPlayer::StaticClass(), FoundPlayers);

    float ClosestDistance = FLT_MAX;
    ALMBpawnPlayer* ClosestPlayer = nullptr;

    for (AActor* Actor : FoundPlayers)
    {
        float Distance = FVector::Dist(GetActorLocation(), Actor->GetActorLocation());
        if (Distance < ClosestDistance)
        {
            ClosestDistance = Distance;
            ClosestPlayer = Cast<ALMBpawnPlayer>(Actor);
        }
    }

    TargetPlayer = ClosestPlayer;
}

void ABossMonster::MoveTowardsPlayer(float DeltaTime)
{
    if (!TargetPlayer || bIsAttacking) return;

    FVector MyLocation = GetActorLocation();
    FVector TargetLocation = TargetPlayer->GetActorLocation();
    float Distance = FVector::Dist(MyLocation, TargetLocation);

    if (Distance <= AttackRange)
    {
        PerformAttack();
        return;
    }

    // 이동 방향 계산
    FVector Direction = (TargetLocation - MyLocation).GetSafeNormal();
    AddMovementInput(Direction, 1.0f);

    // 디버그 라인
    DrawDebugLine(GetWorld(), MyLocation, TargetLocation, FColor::Green, false, 0.1f, 0, 2.0f);
}

// 공격, 회전 및 데미지 로직은 기존 ApplyDamage 방식 그대로 사용
void ABossMonster::PerformAttack()
{
    if (!bCanAttack || !TargetPlayer || bIsDead || TargetPlayer->IsDead()) return;

    bCanAttack = false;
    bIsAttacking = true;

    if (AttackMontage)
    {
        UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
        if (AnimInstance)
        {
            AnimInstance->Montage_Play(AttackMontage);
        }
    }

    FHitResult HitResult;
    FVector Start = GetActorLocation();
    FVector End = Start + GetActorForwardVector() * AttackRange;

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 3.f, 0, 5.f);


    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);
    if (bHit)
    {
        ALMBpawnPlayer* HitPlayer = Cast<ALMBpawnPlayer>(HitResult.GetActor());
        if (HitPlayer)
        {
            UE_LOG(LogTemp, Warning, TEXT("BossMonster hit %s!"), *HitPlayer->GetName());
            HitPlayer->ApplyDamage(AttackDamage, this);
        }
    }

    DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1.f, 0, 3.f);
    if (bHit && HitResult.GetActor())
    {
        UE_LOG(LogTemp, Warning, TEXT("라인 트레이스 충돌: %s"), *HitResult.GetActor()->GetName());
    }

    GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &ABossMonster::ResetAttack, AttackCooldown, false);
}

void ABossMonster::ResetAttack()
{
    bCanAttack = true;
    bIsAttacking = false;
}

void ABossMonster::ApplyDamage(float Damage, ALMBpawnPlayer* DamageInstigator)
{
    if (Damage <= 0.f || CurrentHp <= 0.f) return;

    CurrentHp -= Damage;
    UE_LOG(LogTemp, Warning, TEXT("%s took %f damage! Remaining HP: %f"), *GetName(), Damage, CurrentHp);

    if (CurrentHp <= 0.f)
    {
        Die();
    }
}

void ABossMonster::Die()
{
    if (bIsDead) return; // 중복 호출 방지

    bIsDead = true;

    UE_LOG(LogTemp, Warning, TEXT("BossMonster %s has died."), *GetName());

    // 공격/이동 중지
    bCanAttack = false;
    bIsAttacking = false;

    // 죽음 애니메이션 재생
    if (DeathMontage)
    {
        UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
        if (AnimInstance)
        {
            AnimInstance->Montage_Play(DeathMontage);
        }
    }

    // 일정 시간 후 삭제
    SetLifeSpan(3.5f);
}