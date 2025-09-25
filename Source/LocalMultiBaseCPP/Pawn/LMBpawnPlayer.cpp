// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/LMBpawnPlayer.h"
#include "Player/LMBPlayerController.h"

ALMBpawnPlayer::ALMBpawnPlayer()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ALMBpawnPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

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
	
		// 입력값이 있다.
		FRotator TargetRot = MoveDir.Rotation();
		TargetRot.Pitch = 0.0f;
		TargetRot.Roll = 0.0f;
		
		FRotator CurrentRot = GetActorRotation();
		FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRot, DeltaTime, RoatationTnterSpeed);
		SetActorRotation(NewRot);
	
}
