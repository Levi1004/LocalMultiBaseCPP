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

    // 1. RootComponent 설정 (HitBox)
    HitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBox"));
    SetRootComponent(HitBox);
    HitBox->SetBoxExtent(FVector(100.f, 100.f, 100.f));
    HitBox->SetRelativeLocation(FVector(50.f, 0.f, 50.f));

    // 2. Mesh 설정
    USkeletalMeshComponent* MeshComp = GetMesh();
    MeshComp->SetupAttachment(HitBox); // RootComponent에 Attach
    MeshComp->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
    MeshComp->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

    // 3. 이동 설정
    GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);

    // 4. 초기 상태
    CurrentHp = MaxHp;
    bCanAttack = true;
    bIsAttacking = false;
    bIsDead = false;
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

    if (bIsDead) return;

    // 타겟 없으면 탐색
    if (!TargetPlayer || TargetPlayer->IsDead())
    {
        TargetPlayer = nullptr;
        FindClosestPlayer();
    }

    if (TargetPlayer && !bIsDead)
        MoveTowardsPlayer(DeltaTime);
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
    if (!bCanAttack || bIsAttacking || !TargetPlayer || bIsDead || TargetPlayer->IsDead())
        return;

    bCanAttack = false;
    bIsAttacking = true;

    // 공격 애니메이션 재생
    if (AttackMontages.Num() > 0)
    {
        int32 Index = FMath::RandRange(0, AttackMontages.Num() - 1);
        UAnimMontage* SelectedMontage = AttackMontages[Index];
        if (SelectedMontage)
        {
            GetMesh()->GetAnimInstance()->Montage_Play(SelectedMontage);
        }
    }

    // 데미지 판정 (라인 트레이스)
    FHitResult HitResult;
    FVector Start = GetActorLocation();
    FVector End = Start + GetActorForwardVector() * AttackRange;

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);
    if (bHit)
    {
        if (ALMBpawnPlayer* HitPlayer = Cast<ALMBpawnPlayer>(HitResult.GetActor()))
        {
            float Damage = AttackDamage;
            if (AttackDamages.Num() > 0)
            {
                int32 Index = FMath::Clamp(FMath::RandRange(0, AttackDamages.Num() - 1), 0, AttackDamages.Num() - 1);
                Damage = AttackDamages[Index];
            }

            HitPlayer->ApplyDamage(Damage, this);
        }
    }

    // 공격 쿨다운 후 ResetAttack 호출
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