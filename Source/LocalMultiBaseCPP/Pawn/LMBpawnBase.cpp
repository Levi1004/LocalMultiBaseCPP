// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/LMBpawnBase.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/FloatingPawnMovement.h"

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
	
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshObj(TEXT("/Game/LMBCPP/Art/Models/sk_CharM_Base.sk_CharM_Base"));
	if (MeshObj.Succeeded())
	{
		MeshComponent->SetSkeletalMesh(MeshObj.Object);
		// pitch : Y
		// Yaw : Z
		// Roll : X
		MeshComponent->AddRelativeLocation(FVector(0.0f, 0.0f, -50.0f));
		MeshComponent->AddLocalRotation(FRotator(0, -90.0f, 0.0f));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("MeshObj를 가져오지 못했습니다."));
	}
	

	PawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("PawnMovement"));
	PawnMovement->MaxSpeed = MaxSpeed; // 최대로 낼 수 있는 이동 속도
	PawnMovement->Acceleration = 2048.0f; // 얼마나 빠르게 속도가 증가할 지
	PawnMovement->Deceleration = 2000.0f; // 얼마나 빠르게 감속할 지 
	PawnMovement->TurningBoost = 8.0f; // 회전을 얼마나 민첩하게할 지


}

