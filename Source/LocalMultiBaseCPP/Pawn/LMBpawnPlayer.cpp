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

		springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
		springArmComp->SetupAttachment(RootComponent);
		springArmComp->SetUsingAbsoluteRotation(false);
		springArmComp->SetRelativeRotation(FRotator(-30.f, 0.f, 0.f));
		springArmComp->TargetArmLength = 1000.0f;
		springArmComp->bUsePawnControlRotation = false;
		

		
		cameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
		cameraComp->SetupAttachment(springArmComp);
		cameraComp->bUsePawnControlRotation = false;
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

		if (LastMoveDirection.SizeSquared() > KINDA_SMALL_NUMBER)
		{
			RotateTowardMovement(LastMoveDirection, DeltaTime);
		}
	}

	void ALMBpawnPlayer::OnInputMove(const FVector2D& MoveVector)
	{
		CurrentMoveVector = MoveVector;

		FVector CamForward = cameraComp->GetForwardVector();
		FVector CamRight = cameraComp->GetRightVector();
		CamForward.Z = 0.f;
		CamRight.Z = 0.f;
		CamForward.Normalize();
		CamRight.Normalize();

		if (!FMath::IsNearlyZero(MoveVector.X))
		{
			// 앞/뒤 이동
			LastMoveDirection = (CamForward * MoveVector.X + CamRight * MoveVector.Y).GetSafeNormal();
			AddMovementInput(LastMoveDirection);

			if (MoveVector.X > 0.f)
			{
				// 앞으로 이동할 때만 캐릭터 회전
				RotateTowardMovement(LastMoveDirection, GetWorld()->GetDeltaSeconds());
			}
			else if (!FMath::IsNearlyZero(MoveVector.Y))
			{
				// 뒤로 이동 중 좌/우 입력 → 이동 없이 카메라 회전
				float YawDelta = MoveVector.Y * CameraRotationSpeed * GetWorld()->GetDeltaSeconds();
				FRotator NewRotation = GetActorRotation();
				NewRotation.Yaw += YawDelta;
				SetActorRotation(NewRotation);
			}
		}
		else if (!FMath::IsNearlyZero(MoveVector.Y))
		{
			// 좌/우 입력만 → 이동 없이 캐릭터 회전
			float YawDelta = MoveVector.Y * CameraRotationSpeed * GetWorld()->GetDeltaSeconds();
			FRotator NewRotation = GetActorRotation();
			NewRotation.Yaw += YawDelta;
			SetActorRotation(NewRotation);
		}

		
	}

	void ALMBpawnPlayer::RotateTowardMovement(const FVector& MoveDir, float DeltaTime)
	{
		if (MoveDir.IsNearlyZero())
			return;

		FRotator TargetRot = MoveDir.Rotation();
		TargetRot.Pitch = 0.f;
		TargetRot.Roll = 0.f;

		FRotator NewRot = FMath::RInterpTo(GetActorRotation(), TargetRot, DeltaTime, RoatationInterpSpeed);
		SetActorRotation(NewRot);
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

