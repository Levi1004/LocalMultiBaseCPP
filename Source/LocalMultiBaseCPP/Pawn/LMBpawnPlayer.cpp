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

	AttackComp = CreateDefaultSubobject<UAttackComponent>(TEXT("AttackComponent"));
	// 콜리전 프로필
	if (BoxComponent)
	{
		BoxComponent->SetCollisionProfileName(TEXT("PlayerProfile"));
	}
}

void ALMBpawnPlayer::InitializePlayerStats(int32 InPlayerIndex)
{
	PlayerIndex = InPlayerIndex;

	switch (PlayerIndex)
	{
	case 0: // 1P
		PlayerName = TEXT("Warrior1");
		MaxHealth = 200.f;
		AttackPower = 25.f;
		Defence = 5.f;
		break;
	case 1: // 2P
		PlayerName = TEXT("Warrior2");
		MaxHealth = 200.f;
		AttackPower = 25.f;
		Defence = 5.f;
		break;
	default:
		PlayerName = TEXT("Unknown");
		break;
	}

	// 스탯 적용
	CurrentHp = MaxHealth;
}

void ALMBpawnPlayer::BeginPlay()
{
	Super::BeginPlay();
	ApplyMeshByPlayerIndex();

	USkeletalMesh* SwordMesh = LoadObject<USkeletalMesh>(nullptr, TEXT("/Game/Assets/Weapon/InfinityBladeWeapons/Weapons/Blade/Swords/Blade_HeroSword11/SK_Blade_HeroSword11.SK_Blade_HeroSword11"));
	if (SwordMesh)
	{
		USkeletalMeshComponent* SwordComp = NewObject<USkeletalMeshComponent>(this);
		SwordComp->SetSkeletalMesh(SwordMesh);
		SwordComp->RegisterComponent(); // 반드시 컴포넌트 등록
		SwordComp->AttachToComponent(MeshComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("RightHandSocket"));

		
	}
}


void ALMBpawnPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// AnimInstance 초기화
	if (!bAnimInitialized && MeshComponent)
	{
		ULMBAnimInstance* AnimInst = Cast<ULMBAnimInstance>(MeshComponent->GetAnimInstance());
		if (AnimInst)
		{
			LMBAnim = AnimInst;

			// 스킬 몽타주 등록

			bAnimInitialized = true;
			UE_LOG(LogTemp, Warning, TEXT("플레이어 %d AnimInstance 초기화 완료"), PlayerIndex);
		}
	}
}

void ALMBpawnPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (APlayerController* PC = Cast<APlayerController>(NewController))
	{
		PC->SetViewTarget(this);
	}

	if (ALMBPlayerController* LMBPC = Cast<ALMBPlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		LMBPC->AddPawnPlayer(this);
	}
}

void ALMBpawnPlayer::ApplyMeshByPlayerIndex()
{
	FString MeshPath = TEXT("/Game/Assets/Male/Dwarf_Idle.Dwarf_Idle");
	TSubclassOf<UAnimInstance> AnimClass = LoadClass<UAnimInstance>(nullptr, TEXT("/Game/Assets/Male/BluePrint/NewAnimBlueprint.NewAnimBlueprint_C"));
	FVector MeshLocation = FVector(0.f, 0.f, -20.f);
	FRotator MeshRotation = FRotator(0.f, -90.f, 0.f);
	FVector MeshScale = FVector(1.f);
	FVector SpawnLocation = FVector::ZeroVector;

	switch (PlayerIndex)
	{
	case 0:
		SpawnLocation = FVector(3000.f, 3000.f, 20.f);
		break;
	case 1:
		SpawnLocation = FVector(3200.f, -3200.f, 30.f);
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("PlayerIndex에 해당하는 메쉬/AnimClass 없음"));
		return;
	}

	// SkeletalMesh 적용
	if (USkeletalMesh* NewMesh = LoadObject<USkeletalMesh>(nullptr, *MeshPath))
	{
		MeshComponent->SetSkeletalMesh(NewMesh);
		MeshComponent->SetRelativeLocation(MeshLocation);
		MeshComponent->SetRelativeRotation(MeshRotation);
		MeshComponent->SetRelativeScale3D(MeshScale);
		SetActorLocation(SpawnLocation);

		UE_LOG(LogTemp, Warning, TEXT("플레이어 %d 메쉬 적용 완료: %s"), PlayerIndex, *NewMesh->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("플레이어 %d 메쉬 로드 실패: %s"), PlayerIndex, *MeshPath);
		return;
	}

	// AnimClass 적용
	if (AnimClass)
	{
		MeshComponent->SetAnimInstanceClass(AnimClass);
	}
	
}

void ALMBpawnPlayer::UseSkill(int32 SkillIndex)
{
	if (LMBAnim)
	{
		LMBAnim->PlaySkillMontage(PlayerIndex + 1, SkillIndex);
	}
	UE_LOG(LogTemp, Warning, TEXT("%s : 스킬 %d 발동"), *GetName(), SkillIndex);

	if (AttackComp)
	{
		AttackComp->PerformAttack(this, 200.f); // 공격 거리 예시
	}
}

void ALMBpawnPlayer::EndAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("%s : 스킬/공격 종료"), *GetName());
}