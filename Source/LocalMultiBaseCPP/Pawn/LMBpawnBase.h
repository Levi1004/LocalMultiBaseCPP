// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "LMBpawnBase.generated.h"

UCLASS()
class LOCALMULTIBASECPP_API ALMBpawnBase : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ALMBpawnBase();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* BoxComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USkeletalMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UFloatingPawnMovement* PawnMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MaxSpeed = 2000.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float CameraRotationSpeed = 100.f;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	class USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	class UCameraComponent* CameraComp;

	// ===== Animation =====
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	class ULMBAnimInstance* LMBAnim;

	UPROPERTY()
	TArray<TSubclassOf<UAnimInstance>> AnimClasses;

public:
	//========================================================================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerName")
	FString PlayerName = TEXT("Player");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHealth = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float CurrentHp = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float AttackPower = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Defence = 5.0f;	


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player", meta = (AllowPrivateAccess = "true"))
	int32 PlayerIndex = -1;

	// ===== Functions =====
	virtual void Die();

public:
	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser
	) override;

	FORCEINLINE float GetCurrentHp() const { return CurrentHp; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }

public:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void SetPlayerIndex(int32 NewIndex) { PlayerIndex = NewIndex; }
	int32 GetPlayerIndex() const { return PlayerIndex; }

	virtual void ApplyMeshByPlayerIndex();
	
	void OnInputMove(const FVector2D& MoveVector);
	FORCEINLINE UFloatingPawnMovement* GetMovement() { return PawnMovement; };
	
	
	UFUNCTION(BlueprintCallable, Category = "Skill")
	virtual void UseSkill(int32 SkillIndex) { /* 기본 동작 없음 */ }
		
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void EndAttack() { /* 기본 동작 없음 */ }

private:
	FVector2D CurrentMoveVector;

};
