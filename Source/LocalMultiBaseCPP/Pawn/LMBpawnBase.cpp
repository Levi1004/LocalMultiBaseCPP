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

	AController* MyController = GetController();
	if (!MyController)
	{
		UE_LOG(LogTemp, Warning, TEXT("Controller가 없습니다. BeginPlay 시점에 아직 Possess되지 않았을 수 있습니다."));
		return;
	}

	APlayerController* PC = Cast<APlayerController>(MyController);
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerController가 아닙니다."));
		return;
	}

	int32 PlayerIndex = PC->GetLocalPlayer()->GetControllerId();
	UE_LOG(LogTemp, Warning, TEXT("플레이어 인덱스: %d"), PlayerIndex);

	// 각 플레이어 인덱스에 따른 Skeletal Mesh 설정
	switch (PlayerIndex)
	{
	case 0:
	{
		static ConstructorHelpers::FObjectFinder<USkeletalMesh> Mesh0(TEXT("/Game/Assets/Male/Dwarf_Idle.Dwarf_Idle"));
		if (Mesh0.Succeeded())
		{
			MeshComponent->SetSkeletalMesh(Mesh0.Object);
			MeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -50.0f));
			MeshComponent->SetRelativeRotation(FRotator(0, -90.0f, 0.0f));
			UE_LOG(LogTemp, Warning, TEXT("플레이어 0 메쉬 적용 완료"));
		}
		break;
	}
	case 1:
	{
		static ConstructorHelpers::FObjectFinder<USkeletalMesh> Mesh1(TEXT("/Game/Assets/Female/Standing_Idle.Standing_Idle"));
		if (Mesh1.Succeeded())
		{
			MeshComponent->SetSkeletalMesh(Mesh1.Object);
			MeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -50.0f));
			MeshComponent->SetRelativeRotation(FRotator(0, -90.0f, 0.0f));
			UE_LOG(LogTemp, Warning, TEXT("플레이어 1 메쉬 적용 완료"));
		}
		break;
	}
	default:
		UE_LOG(LogTemp, Warning, TEXT("플레이어 인덱스에 해당하는 메쉬가 없습니다."));
		break;
	}
}