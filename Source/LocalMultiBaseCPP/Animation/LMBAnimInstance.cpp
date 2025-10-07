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


void ULMBAnimInstance::SetSkillMontage(int32 PlayerIndex, int32 SkillIndex, UAnimMontage* Montage)
{
	if (!Montage) return;

	if (PlayerIndex == 1)
	{
		switch (SkillIndex)
		{
		case 1: Skill1P_1 = Montage; break;
		case 2: Skill1P_2 = Montage; break;
		case 3: Skill1P_3 = Montage; break;
		case 4: Skill1P_4 = Montage; break;
		default: break;
		}
	}
	else if (PlayerIndex == 2)
	{
		switch (SkillIndex)
		{
		case 1: Skill2P_1 = Montage; break;
		case 2: Skill2P_2 = Montage; break;
		case 3: Skill2P_3 = Montage; break;
		case 4: Skill2P_4 = Montage; break;
		default: break;
		}
	}
}

void ULMBAnimInstance::PlaySkillMontage(int32 PlayerIndex, int32 SkillIndex)
{
	if (!Owner) return;

	UAnimMontage* MontageToPlay = nullptr;

	// PlayerIndex 1/2에 따라 몽타주 선택
	if (PlayerIndex == 1)
	{
		switch (SkillIndex)
		{
		case 1: MontageToPlay = Skill1P_1; break;
		case 2: MontageToPlay = Skill1P_2; break;
		case 3: MontageToPlay = Skill1P_3; break;
		case 4: MontageToPlay = Skill1P_4; break;
		default: break;
		}
	}
	else if (PlayerIndex == 2)
	{
		switch (SkillIndex)
		{
		case 1: MontageToPlay = Skill2P_1; break;
		case 2: MontageToPlay = Skill2P_2; break;
		case 3: MontageToPlay = Skill2P_3; break;
		case 4: MontageToPlay = Skill2P_4; break;
		default: break;
		}
	}

	if (MontageToPlay)
	{
		Montage_Play(MontageToPlay);
		Montage_SetEndDelegate(SkillEndDelegate, MontageToPlay);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PlaySkillMontage: 해당 스킬 몽타주가 없습니다! PlayerIndex=%d, SkillIndex=%d"), PlayerIndex, SkillIndex);
	}
}

void ULMBAnimInstance::SkillEnded(UAnimMontage* AnimMontage, bool bInterrupted)
{
	if (Owner)
	{
		Owner->EndAttack(); // 스킬 종료 처리
		UE_LOG(LogTemp, Warning, TEXT("Skill Montage Ended"));
	}
}
