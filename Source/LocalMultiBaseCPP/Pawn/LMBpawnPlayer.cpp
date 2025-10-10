	// Fill out your copyright notice in the Description page of Project Settings.


	#include "Pawn/LMBpawnPlayer.h"
	#include "Player/LMBPlayerController.h"
	#include "Animation/LMBAnimInstance.h"
	#include "GameFramework/FloatingPawnMovement.h"
	#include "GameFramework/SpringArmComponent.h"
	#include "Camera/CameraComponent.h"
	#include "Components/BoxComponent.h"


	ALMBpawnPlayer::ALMBpawnPlayer()
	{
		PrimaryActorTick.bCanEverTick = true;

		BoxComponent->SetCollisionProfileName(TEXT("PlayerProfile"));

	}
	void ALMBpawnPlayer::BeginPlay()
	{
		Super::BeginPlay();
	}
	
	void ALMBpawnPlayer::PossessedBy(AController* NewController)
	{
		Super::PossessedBy(NewController);
		
		if (MeshComponent)
		{
			TSubclassOf<UAnimInstance> AnimClass = nullptr;

			switch (PlayerIndex)
			{
			case 0:
				AnimClass = LoadClass<UAnimInstance>(nullptr, TEXT("/Game/Assets/Male/BluePrint/Male_player.Male_player_C"));
				break;

			case 1:
				AnimClass = LoadClass<UAnimInstance>(nullptr, TEXT("/Game/Assets/Female/BluePrint/female_Player.female_Player_C"));
				break;

			default:
				UE_LOG(LogTemp, Warning, TEXT("PlayerIndex에 맞는 AnimClass 없음"));
				break;
			}

			if (AnimClass)
			{
				MeshComponent->SetAnimInstanceClass(AnimClass);

				// AnimInstance가 생성된 직후 가져오기
				LMBAnim = Cast<ULMBAnimInstance>(MeshComponent->GetAnimInstance());
				if (LMBAnim)
				{
					// 1P 스킬
					LMBAnim->SetSkillMontage(1, 1, LoadObject<UAnimMontage>(nullptr, TEXT("/Game/Assets/Male/anime/1P_Skill_1.1P_Skill_1")));
					LMBAnim->SetSkillMontage(1, 2, LoadObject<UAnimMontage>(nullptr, TEXT("/Game/Assets/Male/anime/1P_Skill_2.1P_Skill_2")));
					LMBAnim->SetSkillMontage(1, 3, LoadObject<UAnimMontage>(nullptr, TEXT("/Game/Assets/Male/anime/1P_Skill_3.1P_Skill_3")));
					LMBAnim->SetSkillMontage(1, 4, LoadObject<UAnimMontage>(nullptr, TEXT("/Game/Assets/Male/anime/1P_Skill_4.1P_Skill_4")));

					// 2P 스킬
					LMBAnim->SetSkillMontage(2, 1, LoadObject<UAnimMontage>(nullptr, TEXT("")));
					LMBAnim->SetSkillMontage(2, 2, LoadObject<UAnimMontage>(nullptr, TEXT("")));
					LMBAnim->SetSkillMontage(2, 3, LoadObject<UAnimMontage>(nullptr, TEXT("")));
					LMBAnim->SetSkillMontage(2, 4, LoadObject<UAnimMontage>(nullptr, TEXT("")));

					UE_LOG(LogTemp, Warning, TEXT("모든 스킬 몽타주 Setter로 할당 완료 (PossessedBy)"));
				}
			}

			// AnimInstance는 Tick이나 BeginPlay에서 가져오자
			// LMBAnim = Cast<ULMBAnimInstance>(MeshComponent->GetAnimInstance()); <-- 여기서는 아직 nullptr 가능
		}

		// PlayerController 연결
		if (APlayerController* PC = Cast<APlayerController>(NewController))
		{
			PC->SetViewTarget(this);
		}

		if (ALMBPlayerController* LMBPlayerController = Cast<ALMBPlayerController>(GetWorld()->GetFirstPlayerController()))
		{
			LMBPlayerController->AddPawnPlayer(this);
		}
	}
	
	void ALMBpawnPlayer::Tick(float DeltaTime)
	{
		Super::Tick(DeltaTime);

	}

	void ALMBpawnPlayer::UseSkill(int32 SkillIndex)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s : 스킬 %d 발동"), *GetName(), SkillIndex);

		if (LMBAnim)
		{
			// PlayerIndex는 0/1이므로 AnimInstance에서 1/2로 맞춰서 호출
			LMBAnim->PlaySkillMontage(PlayerIndex + 1, SkillIndex);
		}
	}

	void ALMBpawnPlayer::EndAttack()
	{
		// 공격 상태 해제
		UE_LOG(LogTemp, Warning, TEXT("%s : 스킬/공격 종료"), *GetName());

		// 필요하다면 이동 가능 상태로 복구
		if (MeshComponent)
		{
			// 예: 공격 중 이동 제한을 풀고 싶다면 여기서 처리
		}

		// 다른 상태 초기화도 가능
	}

