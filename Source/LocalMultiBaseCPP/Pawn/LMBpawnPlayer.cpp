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

		if (!LMBAnim && MeshComponent)
		{
			LMBAnim = Cast<ULMBAnimInstance>(MeshComponent->GetAnimInstance());
			if (LMBAnim)
			{
				UE_LOG(LogTemp, Warning, TEXT("Player %d AnimInstance 가져오기 성공 (BeginPlay)"), PlayerIndex);
			}
		}
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

		if (LastMoveDirection.SizeSquared() > KINDA_SMALL_NUMBER) //LastMoveDirection이 0이 아니라면
		{
			if (!bIsAttacking && CurrentMoveVector.X > 0.0f)
			{
				RotateTowardMovement(LastMoveDirection, DeltaTime);
			}	
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
		if (bIsAttacking || MoveDir.IsNearlyZero())
			return;

		FRotator TargetRot = MoveDir.Rotation();
		TargetRot.Pitch = 0.f;
		TargetRot.Roll = 0.f;

		FRotator NewRot = FMath::RInterpTo(GetActorRotation(), TargetRot, DeltaTime, RoatationInterpSpeed);
		SetActorRotation(NewRot);
	}

	void ALMBpawnPlayer::StartAttack()
	{
		
			if (bIsAttacking)
				return;

		bIsAttacking = true;
		if (PawnMovement)
		{
			PawnMovement->StopMovementImmediately();
			PawnMovement->Deactivate();
		}
		if (LMBAnim)
			LMBAnim->PlayAttackMontage();
	}

	void ALMBpawnPlayer::EndAttack()
	{
		bIsAttacking = false;
            if (PawnMovement)
			PawnMovement->Activate();
	}


