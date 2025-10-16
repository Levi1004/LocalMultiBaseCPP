// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
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

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    // ----------------------
    // Components
    // ----------------------
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UBoxComponent* HitBox;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    USkeletalMeshComponent* SkeletalMeshComp;

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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float AttackRange = 150.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float ExperienceValue = 500.f;

    // ----------------------
    // Animations
    // ----------------------
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
    UAnimMontage* AttackMontage;

    // ----------------------
    // Combat
    // ----------------------
    UFUNCTION()
    void ApplyDamage(float Damage, ALMBpawnPlayer* DamageInstigator);

    UFUNCTION()
    void Die();

    void PerformAttack();

private:
    ALMBpawnPlayer* TargetPlayer;

    void FindClosestPlayer();

    void MoveTowardsPlayer(float DeltaTime);

    bool bIsAttacking;
};