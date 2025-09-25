// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/LMBAnimInstance.h"
#include "Pawn/LMBpawnPlayer.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "UObject/ConstructorHelpers.h"



ULMBAnimInstance::ULMBAnimInstance()
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AMObj(TEXT("/Game/LMBCPP/Animation/AM_AttackMontage.AM_AttackMontage"));
	if (AMObj.Succeeded())
	{
		AttackMontage = AMObj.Object;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AttackMontage를 가져오지 못했습니다."));
	}

	MovingThreshould = 3.0f;
}

void ULMBAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// GetOwningActor : 나를 소유하고 있는 Actor를 반환
	ALMBpawnPlayer* OwningActor = Cast<ALMBpawnPlayer>(GetOwningActor());
	if (OwningActor)
	{
		Owner = OwningActor;
		Movement = OwningActor->GetMovement();
	}

	AttackEndDelegate.BindUObject(this, &ULMBAnimInstance::AttackEnded);
}

void ULMBAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

		if (Movement)
		{
			Velocity = Movement->Velocity;
			
			// Velocity.Size2D() : Vector의 힘의 크기를 double로 반환
			GroundSpeed = Velocity.Size2D();
			// 이동하고 있지 않다 = 이동의 힘 < 이동하는지 체크하는 임계값
			bIsIdle = GroundSpeed < MovingThreshould;
		}

}

void ULMBAnimInstance::AttackEnded(UAnimMontage* AnimMontage, bool BInterrupted)
{
	Owner->EndAttack();
}

void ULMBAnimInstance::PlayAttackMontage()
{
	if (AttackMontage)
	{
		float PlayedLen = Montage_Play(AttackMontage, 1.0f);
		if (PlayedLen > 0.0f)
		{
			//Montage_SetEndDelegate a
			Montage_SetEndDelegate(AttackEndDelegate, AttackMontage);
		}
	}
}
