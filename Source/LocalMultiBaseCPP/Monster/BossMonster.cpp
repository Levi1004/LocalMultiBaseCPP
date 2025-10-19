// Fill out your copyright notice in the Description page of Project Settings.

#include "BossMonster.h"
#include "Components/BoxComponent.h"
#include "Pawn/LMBpawnPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Animation/AnimInstance.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"

int32 ABossMonster::AliveBossCount = 0; // 정적 변수 초기화

ABossMonster::ABossMonster()
{
    PrimaryActorTick.bCanEverTick = true;

    bIsActive = false;

    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);

    HitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBox"));
    HitBox->SetupAttachment(GetRootComponent());
    HitBox->SetBoxExtent(FVector(100.f, 100.f, 100.f));
    HitBox->SetRelativeLocation(FVector(50.f, 0.f, 50.f));

    GetMesh()->SetupAttachment(GetCapsuleComponent());
    GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
    GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

    GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);

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

    // 보스 활성화 3분 타이머
    GetWorldTimerManager().SetTimer(ActivateTimerHandle, this, &ABossMonster::ActivateBoss, 180.f, false);

    // 보스 카운트 증가
    AliveBossCount++;
}

void ABossMonster::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bIsActive) return;
    if (bIsDead) return;

    if (!TargetPlayer || TargetPlayer->IsDead())
    {
        TargetPlayer = nullptr;
        FindClosestPlayer();
    }

    if (TargetPlayer && !bIsDead)
    {
        float Distance = FVector::Dist(GetActorLocation(), TargetPlayer->GetActorLocation());

        if (Distance <= AttackRange)
        {
            PerformAttack();
        }
        else
        {
            MoveTowardsPlayer(DeltaTime);
        }

       // DrawDebugLine(GetWorld(), GetActorLocation(), TargetPlayer->GetActorLocation(), FColor::Green, false, 0.1f, 0, 2.0f);
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

    FVector Direction = (TargetPlayer->GetActorLocation() - GetActorLocation()).GetSafeNormal();
    AddMovementInput(Direction, 1.0f);
}

void ABossMonster::ActivateBoss()
{
    bIsActive = true;
    UE_LOG(LogTemp, Warning, TEXT("Boss is now active!"));
    bCanAttack = true;
}

void ABossMonster::PerformAttack()
{
    if (!bCanAttack || bIsAttacking || !TargetPlayer || bIsDead || TargetPlayer->IsDead())
        return;

    bCanAttack = false;
    bIsAttacking = true;

    UE_LOG(LogTemp, Warning, TEXT("BossMonster is Attacking!"));

    if (AttackMontages.Num() > 0)
    {
        int32 Index = FMath::RandRange(0, AttackMontages.Num() - 1);
        UAnimMontage* SelectedMontage = AttackMontages[Index];
        if (SelectedMontage)
        {
            GetMesh()->GetAnimInstance()->Montage_Play(SelectedMontage);
        }
    }

    FHitResult HitResult;
    FVector Start = GetActorLocation() + FVector(0, 0, -1500);
    FVector End = Start + GetActorForwardVector() * AttackRange;

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_GameTraceChannel1, Params);

   // DrawDebugLine(GetWorld(), Start, End, bHit ? FColor::Red : FColor::Green, false, 1.0f, 0, 5.0f);

    if (bHit)
    {
        UE_LOG(LogTemp, Warning, TEXT("Boss 라인트레이스가 %s 에 맞음!"), *HitResult.GetActor()->GetName());

        if (ALMBpawnPlayer* HitPlayer = Cast<ALMBpawnPlayer>(HitResult.GetActor()))
        {
            float Damage = AttackDamage;
            if (AttackDamages.Num() > 0)
            {
                int32 Index = FMath::RandRange(0, AttackDamages.Num() - 1);
                Damage = AttackDamages[Index];
            }

            UE_LOG(LogTemp, Warning, TEXT("Boss가 %s에게 %f 피해를 줌"), *HitPlayer->GetName(), Damage);
            HitPlayer->ApplyDamage(Damage, this);
        }
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
    // 아직 활성화되지 않았다면 무적 상태
    if (!bIsActive)
    {
        UE_LOG(LogTemp, Warning, TEXT("%s 은(는) 아직 활성화되지 않아 무적입니다!"), *GetName());
        return;
    }

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
    if (bIsDead) return;

    bIsDead = true;
    UE_LOG(LogTemp, Warning, TEXT("BossMonster %s has died."), *GetName());

    bCanAttack = false;
    bIsAttacking = false;

    if (DeathMontage)
    {
        UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
        if (AnimInstance)
        {
            AnimInstance->Montage_Play(DeathMontage);
        }
    }

    // 보스 카운트 감소
    AliveBossCount--;
    if (AliveBossCount <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("모든 보스 처치! 3초 뒤 게임 클리어 UI 표시 예정"));

        // 3초 뒤 게임 클리어 위젯 표시
        FTimerHandle GameClearTimerHandle;
        GetWorldTimerManager().SetTimer(
            GameClearTimerHandle,
            [this]()
            {
                if (GameClearWidgetClass)
                {
                    GameClearWidget = CreateWidget<UUserWidget>(GetWorld(), GameClearWidgetClass);
                    if (GameClearWidget)
                    {
                        GameClearWidget->AddToViewport();

                        if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
                        {
                            FInputModeUIOnly InputMode;
                            InputMode.SetWidgetToFocus(GameClearWidget->TakeWidget());
                            PC->SetInputMode(InputMode);
                            PC->bShowMouseCursor = true;
                        }
                    }
                }
            },
            5.0f,  
            false
        );
    }

    SetLifeSpan(3.5f);
}