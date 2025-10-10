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

    // 콜리전 프로필 지정
    if (BoxComponent)
    {
        BoxComponent->SetCollisionProfileName(TEXT("PlayerProfile"));
    }
}

// -----------------------------
// BeginPlay
// -----------------------------
void ALMBpawnPlayer::BeginPlay()
{
    Super::BeginPlay();

    ApplyMeshByPlayerIndex();
}

// -----------------------------
// Tick에서 AnimInstance 초기화
// -----------------------------
void ALMBpawnPlayer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bAnimInitialized && MeshComponent)
    {
        ULMBAnimInstance* AnimInst = Cast<ULMBAnimInstance>(MeshComponent->GetAnimInstance());
        if (AnimInst)
        {
            LMBAnim = AnimInst;

            // 스킬 몽타주 등록
            for (auto& Pair : SkillMontages)
            {
                LMBAnim->SetSkillMontage(Pair.Key / 10, Pair.Key % 10, Pair.Value);
            }

            UE_LOG(LogTemp, Warning, TEXT("플레이어 %d AnimInstance 초기화 완료"), PlayerIndex);
            bAnimInitialized = true;
        }
    }
}

// -----------------------------
// Possess 처리
// -----------------------------
void ALMBpawnPlayer::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    if (APlayerController* PC = Cast<APlayerController>(NewController))
    {
        PC->SetViewTarget(this);
    }

    if (ALMBPlayerController* LMBPlayerController = Cast<ALMBPlayerController>(GetWorld()->GetFirstPlayerController()))
    {
        LMBPlayerController->AddPawnPlayer(this);
    }
}

// -----------------------------
// PlayerIndex에 따른 Mesh & Anim 세팅
// -----------------------------
void ALMBpawnPlayer::ApplyMeshByPlayerIndex()
{
    FString MeshPath;
    TSubclassOf<UAnimInstance> AnimClass = nullptr;

    FVector MeshLocation = FVector::ZeroVector;
    FRotator MeshRotation = FRotator::ZeroRotator;
    FVector MeshScale = FVector(1.0f);
    FVector SpawnLocation = FVector::ZeroVector;

    switch (PlayerIndex)
    {
    case 0: // 1P
        MeshPath = TEXT("/Game/Assets/Male/Dwarf_Idle.Dwarf_Idle");
        AnimClass = LoadClass<UAnimInstance>(nullptr, TEXT("/Game/Assets/Male/BluePrint/Male_player.Male_player_C"));
        MeshLocation = FVector(0.f, 0.f, -20.f);
        MeshRotation = FRotator(0.f, -90.f, 0.f);
        MeshScale = FVector(1.0f);
        SpawnLocation = FVector(3000.f, 3000.f, 20.f);
        break;

    case 1: // 2P
        MeshPath = TEXT("/Game/Assets/Female/Standing_Idle.Standing_Idle");
        AnimClass = LoadClass<UAnimInstance>(nullptr, TEXT("/Game/Assets/Female/BluePrint/female_Player.female_Player_C"));
        MeshLocation = FVector(0.f, 0.f, -30.f);
        MeshRotation = FRotator(0.f, -90.f, 0.f);
        MeshScale = FVector(0.6f);
        SpawnLocation = FVector(3200.f, -3200.f, 30.f);
        break;

    default:
        UE_LOG(LogTemp, Warning, TEXT("PlayerIndex에 해당하는 메쉬/AnimClass 없음"));
        return;
    }

    // ---- SkeletalMesh 적용 ----
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

    // ---- AnimClass 적용 ----
    if (AnimClass)
    {
        MeshComponent->SetAnimInstanceClass(AnimClass);
    }

    // ---- 스킬 몽타주 미리 로드 ----
    SkillMontages.Empty();
    if (PlayerIndex == 0)
    {
        SkillMontages.Add(11, LoadObject<UAnimMontage>(nullptr, TEXT("/Game/Assets/Male/anime/1P_Skill_1.1P_Skill_1")));
        SkillMontages.Add(12, LoadObject<UAnimMontage>(nullptr, TEXT("/Game/Assets/Male/anime/1P_Skill_2.1P_Skill_2")));
        SkillMontages.Add(13, LoadObject<UAnimMontage>(nullptr, TEXT("/Game/Assets/Male/anime/1P_Skill_3.1P_Skill_3")));
        SkillMontages.Add(14, LoadObject<UAnimMontage>(nullptr, TEXT("/Game/Assets/Male/anime/1P_Skill_4.1P_Skill_4")));
    }
    else if (PlayerIndex == 1)
    {
        SkillMontages.Add(21, LoadObject<UAnimMontage>(nullptr, TEXT("/Game/Assets/Female/anime/Standing_2H_Magic_Attack_01.Standing_2H_Magic_Attack_01")));
        SkillMontages.Add(22, LoadObject<UAnimMontage>(nullptr, TEXT("/Game/Assets/Female/anime/Standing_2H_Magic_Area_Attack_02.Standing_2H_Magic_Area_Attack_02")));
        SkillMontages.Add(23, LoadObject<UAnimMontage>(nullptr, TEXT("/Game/Assets/Female/anime/Standing_1H_Magic_Attack_03.Standing_1H_Magic_Attack_03")));
        SkillMontages.Add(24, LoadObject<UAnimMontage>(nullptr, TEXT("/Game/Assets/Female/anime/Standing_2H_Magic_Attack_04.Standing_2H_Magic_Attack_04")));
    }
}

// -----------------------------
// 스킬 사용
// -----------------------------
void ALMBpawnPlayer::UseSkill(int32 SkillIndex)
{
    UE_LOG(LogTemp, Warning, TEXT("%s : 스킬 %d 발동"), *GetName(), SkillIndex);

    if (LMBAnim)
    {
        LMBAnim->PlaySkillMontage(PlayerIndex + 1, SkillIndex);
    }
}

// -----------------------------
// 공격 종료 처리
// -----------------------------
void ALMBpawnPlayer::EndAttack()
{
    UE_LOG(LogTemp, Warning, TEXT("%s : 스킬/공격 종료"), *GetName());
}