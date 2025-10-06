// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/LMBpawnBase.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Animation/LMBAnimInstance.h"




// Sets default values
ALMBpawnBase::ALMBpawnBase()
{
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	SetRootComponent(BoxComponent);
	BoxComponent->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	MeshComponent->SetupAttachment(BoxComponent);

	// FObjectfinder<가져올 오브젝트 타입>
	// CubeMesh : 변수 이름
	// () 경로 입력
	// 로드하는 시간이 오래걸린다. / 변하지 않을거에 적용 
	
	//FClassFinder : 경로에있는 클래스를 가져온다.
	// 클래스를 경로로 가져올땐 경로 뒤에 _C를 붙여줘야 한다.
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimClassRef(TEXT("/Game/LMBCPP/Animation/AB_LMBAnimainstans.AB_LMBAnimainstans_C"));
	if (AnimClassRef.Succeeded())
	{
		MeshComponent->SetAnimInstanceClass(AnimClassRef.Class);
	}

	

	PawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("PawnMovement"));
	PawnMovement->MaxSpeed = MaxSpeed; // 최대로 낼 수 있는 이동 속도
	PawnMovement->Acceleration = 2048.0f; // 얼마나 빠르게 속도가 증가할 지
	PawnMovement->Deceleration = 2000.0f; // 얼마나 빠르게 감속할 지 
	PawnMovement->TurningBoost = 8.0f; // 회전을 얼마나 민첩하게할 지

	CurrentHealth = MaxHealth;
	UE_LOG(LogTemp, Warning, TEXT("플레이어 0의 현재 체력 %f"), CurrentHealth);
	


}
void ALMBpawnBase::BeginPlay()
{
	Super::BeginPlay();

	// PlayerIndex가 이미 할당되어 있다고 가정
	ApplyMeshByPlayerIndex();
}


	

void ALMBpawnBase::ApplyMeshByPlayerIndex()
{
    FString MeshPath;
    FVector MeshLocation = FVector::ZeroVector;  // SkeletalMesh 상대 위치
    FRotator MeshRotation = FRotator::ZeroRotator;
    FVector MeshScale = FVector(1.f);

    // 스폰 위치를 PlayerIndex별로 설정할 지역 변수
    FVector PawnWorldLocation = FVector::ZeroVector;

    switch (PlayerIndex)
    {
    case 0: // 1P
        MeshPath = TEXT("/Game/Assets/Male/Dwarf_Idle.Dwarf_Idle");
        MeshLocation = FVector(0.f, 0.f, -20.f);
        MeshRotation = FRotator(0.f, -90.f, 0.f);
        MeshScale = FVector(1.0f);
        PawnWorldLocation = FVector(3000.f, 3000.f, 20.f);  // 월드 위치 설정
        break;

    case 1: // 2P
        MeshPath = TEXT("/Game/Assets/Female/Standing_Idle.Standing_Idle");
        MeshLocation = FVector(0.f, 0.f, 0.f);
        MeshRotation = FRotator(0.f, -90.f, 0.f);
        MeshScale = FVector(0.6f);
        PawnWorldLocation = FVector(3200.f, -3200.f, 150.f);  // 2P 시작 위치
        break;

    default:
        UE_LOG(LogTemp, Warning, TEXT("PlayerIndex에 해당하는 메쉬가 없습니다."));
        return;
    }

    // SkeletalMesh 적용
    USkeletalMesh* NewMesh = LoadObject<USkeletalMesh>(nullptr, *MeshPath);
    if (NewMesh)
    {
        MeshComponent->SetSkeletalMesh(NewMesh);
        MeshComponent->SetRelativeLocation(MeshLocation);
        MeshComponent->SetRelativeRotation(MeshRotation);
        MeshComponent->SetRelativeScale3D(MeshScale);

        // Pawn 월드 위치 설정
        SetActorLocation(PawnWorldLocation);

        UE_LOG(LogTemp, Warning, TEXT("플레이어 %d 메쉬 적용 완료: %s"), PlayerIndex, *NewMesh->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("플레이어 %d 메쉬를 불러오지 못했습니다: %s"), PlayerIndex, *MeshPath);
    }
}