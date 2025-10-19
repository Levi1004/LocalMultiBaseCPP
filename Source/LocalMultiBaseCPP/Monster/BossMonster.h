// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Blueprint/UserWidget.h"
#include "BossMonster.generated.h"

class UBoxComponent;
class ALMBpawnPlayer;
class UAnimMontage;

UCLASS()
class LOCALMULTIBASECPP_API ABossMonster : public ACharacter
{
    GENERATED_BODY()

public:
    ABossMonster();

    static int32 AliveBossCount;

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    // ----------------------
    // Components
    // ----------------------
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UBoxComponent* HitBox;

    // Character 기반이므로 SkeletalMesh는 GetMesh() 사용
    // ----------------------
    // Stats
    // ----------------------
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float MaxHp = 1000.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
    float CurrentHp;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float AttackPower = 50.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float MoveSpeed = 300.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Attack")
    float AttackRange = 4500.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Attack")
    float AttackDamage = 30.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Attack")
    float AttackCooldown = 2.0f;

    FTimerHandle AttackTimerHandle;
    bool bCanAttack;
    bool bIsAttacking;
    bool bIsDead = false;
    // ----------------------
    // Target
    // ----------------------
    UPROPERTY()
    ALMBpawnPlayer* TargetPlayer;

    // ----------------------
    // Animations
    // ----------------------
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
    TArray<UAnimMontage*> AttackMontages; // 여러 공격 모션

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
    TArray<float> AttackDamages; // 각 공격 모션별 데미지

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
    UAnimMontage* DeathMontage;

    // ----------------------
    // Combat
    // ----------------------
    UFUNCTION()
    void ApplyDamage(float Damage, ALMBpawnPlayer* DamageInstigator);

    UFUNCTION()
    void Die();

    void PerformAttack();

    UFUNCTION()
    void ResetAttack();

private:
    void FindClosestPlayer();
    void MoveTowardsPlayer(float DeltaTime);

private:
    bool bIsActive;                  // 활성화 여부
    FTimerHandle ActivateTimerHandle; // 3분 후 활성화 타이머

    void ActivateBoss();
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> GameClearWidgetClass;

    UUserWidget* GameClearWidget;
};