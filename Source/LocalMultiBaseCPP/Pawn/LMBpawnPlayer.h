// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pawn/LMBpawnBase.h"
#include "LMBpawnPlayer.generated.h"



/**
 * 
 */
UCLASS()
class LOCALMULTIBASECPP_API ALMBpawnPlayer : public ALMBpawnBase
{
    GENERATED_BODY()

public:
    ALMBpawnPlayer();

protected:
    virtual void BeginPlay() override;
    virtual void PossessedBy(AController* NewController) override;
    virtual void Tick(float DeltaTime) override;

public:
    /** PlayerIndex에 따라 Mesh/Anim 설정 */
    virtual void ApplyMeshByPlayerIndex() override;

    /** 스킬 발동 */
    virtual void UseSkill(int32 SkillIndex) override;

    /** 공격 종료 */
    virtual void EndAttack() override;

private:
    /** AnimInstance 안전 초기화 체크 */
    bool bAnimInitialized = false;

    /** 플레이어별 스킬 몽타주 캐시 */
    TMap<int32, UAnimMontage*> SkillMontages;

    /** AnimInstance 생성/등록 */
    void InitializeAnimInstance();
};