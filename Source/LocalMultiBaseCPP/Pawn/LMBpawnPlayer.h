// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pawn/LMBpawnBase.h"
#include "LMBpawnPlayer.generated.h"



/**
 * 
 */
UCLASS()
class LOCALMULTIBASECPP_API ALMBpawnPlayer : public ALMBpawnBase
{
	GENERATED_BODY()
	
public:
	ALMBpawnPlayer();

protected:
	virtual void BeginPlay() override;
	// Posses 될 때 나를 빙의 시키는 컨트롤러를 매개변수로 넣어주며 실행되는 함수
	virtual void PossessedBy(AController* NewController)override;

	virtual void Tick(float DeltaTime) override;


private:
	FVector LastMoveDirection;

protected:
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float RoatationInterpSpeed = 2.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<class ULMBAnimInstance> LMBAnim;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float CameraRotationSpeed = 100.f;

private:
	FVector2D CurrentMoveVector;

public:
	UFUNCTION(BlueprintCallable, Category = "Player")
	FORCEINLINE int32 GetPlayerIndex() const { return PlayerIndex; }

	UFUNCTION(BlueprintCallable, Category = "Player")
	FORCEINLINE void SetPlayerIndex(int32 NewIndex) { PlayerIndex = NewIndex; }

public:
	void OnInputMove(const FVector2D& MoveVector);
	void RotateTowardMovement(const FVector& MoveDir, float DeltaTime);
	
	UFUNCTION(BlueprintCallable, Category = "Skill")
	void UseSkill(int32 SkillIndex);

    void EndAttack();
};
