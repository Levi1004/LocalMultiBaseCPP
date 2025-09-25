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
	
private:
	// Montage 애니메이션이 종료될 때 실행시킬 수 있는 델리게이트
	FOnMontageEnded AttackEndDelegate;
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

protected:
	UFUNCTION()
	void AttackEnded(UAnimMontage* AnimMontage, bool BInterrupted);

public:
	void PlayAttackMontage();

};
