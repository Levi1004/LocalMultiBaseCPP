// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pawn/LMBpawnBase.h"
#include "Attack/AttackComponent.h"
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

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(BlueprintReadOnly)
	bool bAnimInitialized = false;

	// Skill Montages Map (PlayerIndex + SkillIndex)
	UPROPERTY()
	TMap<int32, class UAnimMontage*> SkillMontages;

public:
	// PlayerIndex에 따라 스탯 초기화
	void InitializePlayerStats(int32 InPlayerIndex);


	UPROPERTY(BlueprintReadWrite)
	int32 CurrentExp = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 Level = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float HpIncreasePerLevel = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float AttackIncreasePerLevel = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 ExpToLevelUp = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float ExpGrowthRate = 1.2f;

	UFUNCTION()
	void AddExperience(int32 Exp);

	// PlayerIndex에 따라 Mesh/Anim 설정
	virtual void ApplyMeshByPlayerIndex() override;

	// 스킬 사용
	virtual void UseSkill(int32 SkillIndex) override;

	// 공격 종료
	virtual void EndAttack() override;

	// Possess 처리
	virtual void PossessedBy(AController* NewController) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack")
	class UAttackComponent* AttackComp;
};