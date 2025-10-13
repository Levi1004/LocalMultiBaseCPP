// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "LMBAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class LOCALMULTIBASECPP_API ULMBAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	ULMBAnimInstance();

protected:
	// NativeInitializeAnimation : AnimInstance Beginplay와 비슷한 함수
    virtual void NativeInitializeAnimation() override;
	// tick 역할을 하는 함수
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	

protected:
	// TObjectptr : 꺽쇠 안에 들어온 타입과 그타입의 자식 포인터가 들어 올 수 있는 타입.
	// 언리얼에서 지원하는 UObject용 포인터
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Owner")
	TObjectPtr<class ALMBpawnPlayer> Owner;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	TObjectPtr<class UFloatingPawnMovement> Movement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Montage")
	class UAnimMontage* AttackMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	FVector Velocity;	

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float GroundSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float MovingThreshould;
	// 비트 플래그
	// bisidle 은 메모리에서 1bit만 사용됨

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	uint8 bIsIdle : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	UAnimMontage* SkillAllMontage;

protected:
	FOnMontageEnded SkillEndDelegate;

	// 스킬 몽타주 종료 시 호출되는 함수
	UFUNCTION()
	void SkillEnded(UAnimMontage* AnimMontage, bool bInterrupted);

public:
	void SetSkillMontage(int32 PlayerIndex, int32 SkillIndex, UAnimMontage* Montage);

	UFUNCTION(BlueprintCallable, Category = "Skill")
	void PlaySkillMontage(int32 PlayerIndex, int32 SkillIndex);
};
