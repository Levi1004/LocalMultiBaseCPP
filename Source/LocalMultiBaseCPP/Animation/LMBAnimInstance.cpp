// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/LMBAnimInstance.h"
#include "Pawn/LMBpawnPlayer.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "UObject/ConstructorHelpers.h"



ULMBAnimInstance::ULMBAnimInstance()
{
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

	SkillEndDelegate.BindUObject(this, &ULMBAnimInstance::SkillEnded);
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


void ULMBAnimInstance::PlaySkillMontage(int32 PlayerIndex, int32 SkillIndex)
{
	if (!SkillAllMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillAllMontage가 지정되지 않았습니다!"));
		return;
	}

	FName SectionName;
	switch (SkillIndex)
	{
	case 1: SectionName = FName("Skill1"); break;
	case 2: SectionName = FName("Skill2"); break;
	case 3: SectionName = FName("Skill3"); break;
	default: SectionName = FName("Attack"); break;
	}

	Montage_Play(SkillAllMontage);
	Montage_JumpToSection(SectionName, SkillAllMontage);
	Montage_SetEndDelegate(SkillEndDelegate, SkillAllMontage);
}

void ULMBAnimInstance::SkillEnded(UAnimMontage* AnimMontage, bool bInterrupted)
{
	if (Owner)
	{
		Owner->EndAttack(); // 스킬 종료 처리
		UE_LOG(LogTemp, Warning, TEXT("Skill Montage Ended"));
	}
}
