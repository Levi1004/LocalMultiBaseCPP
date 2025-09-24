// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/LMBpawnBase.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/BoxComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "GameFramework/FloatingPawnMovement.h"

// Sets default values
ALMBpawnBase::ALMBpawnBase()
{
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	SetRootComponent(BoxComponent);
	BoxComponent->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	MeshComponent->SetupAttachment(BoxComponent);

	// FObjectfinder<가져올 오브젝트 타입>
	// CubeMesh : 변수 이름
	// () 경로 입력
	// 로드하는 시간이 오래걸린다. / 변하지 않을거에 적용 
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		MeshComponent->SetStaticMesh(CubeMesh.Object);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("CubeMesh를 가져오지 못했습니다."));
	}
	static ConstructorHelpers::FObjectFinder<UMaterial> BasicMaterial(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	if (BasicMaterial.Succeeded())
	{
		MeshComponent->GetStaticMesh()->SetMaterial(0, BasicMaterial.Object);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BasicMaterial를 가져오지 못했습니다."));
	}
	
}

