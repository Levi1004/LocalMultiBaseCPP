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
	springArmComp->SetUsingAbsoluteRotation(true);
	springArmComp->SetWorldRotation(FRotator(-30.f, 0.f, 0.f));
	springArmComp->TargetArmLength = 1000;
	cameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	cameraComp->SetupAttachment(springArmComp);

	
}


void ALMBpawnPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
    ULMBAnimInstance* AnimInstance = Cast<ULMBAnimInstance>(MeshComponent->GetAnimInstance());
	LMBAnim = AnimInstance;
	
	if (APlayerController* PC = Cast<APlayerController>(NewController))
	{
		PC->SetViewTarget(this);
	}

	APlayerController* FirstPlayerController = GetWorld()->GetFirstPlayerController();
	ALMBPlayerController* LMBPlayerController = Cast<ALMBPlayerController>(FirstPlayerController);

	LMBPlayerController->AddPawnPlayer(this);
}

void ALMBpawnPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (LastMoveDirection.SizeSquared() > KINDA_SMALL_NUMBER) //LastMoveDirection이 0이 아니라면
	{
		RotateTowardMovement(LastMoveDirection, DeltaTime);
	}
}

void ALMBpawnPlayer::OnInputMove(const FVector2D& MoveVector)
{
	if (MoveVector.IsNearlyZero())
	{
		// 입력값이 없다.
		LastMoveDirection = FVector::ZeroVector;
		return;
	}
	// 입력값이 있다.
	// 컨트롤러의 Yaw를 기준으로 회전축을 저으이
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;

	FVector Forward = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::X);
	FVector Right = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y);

	// 이동방향 
	LastMoveDirection = (Forward * MoveVector.X + Right * MoveVector.Y).GetSafeNormal();

	AddMovementInput(LastMoveDirection);
}

void ALMBpawnPlayer::RotateTowardMovement(const FVector& MoveDir, float DeltaTime)
{
	if (bIsAttacking)
	{
		return;
	}
	
		// 입력값이 있다.
		FRotator TargetRot = MoveDir.Rotation();
		TargetRot.Pitch = 0.0f;
		TargetRot.Roll = 0.0f;
		
		FRotator CurrentRot = GetActorRotation();
		FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRot, DeltaTime, RoatationTnterSpeed);
		SetActorRotation(NewRot);
	
}

void ALMBpawnPlayer::StartAttack()
{
	if (bIsAttacking)
	{
		return;
	}
	bIsAttacking = true;
	PawnMovement->StopMovementImmediately();
	PawnMovement->Deactivate();
	LMBAnim->PlayAttackMontage();

}

void ALMBpawnPlayer::EndAttack()
{
	bIsAttacking = false;

	PawnMovement->Activate();
}
