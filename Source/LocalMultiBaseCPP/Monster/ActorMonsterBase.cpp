// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/ActorMonsterBase.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"


// Sets default values
AActorMonsterBase::AActorMonsterBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("My Box Component"));
	SetRootComponent(BoxComp);

	FVector BoxSize = FVector(50.0f, 50.0f, 50.0f);
	BoxComp->SetBoxExtent(BoxSize);
	BoxComp->SetCollisionProfileName(TEXT("EnemyProfile"));
	
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("My Static Mesh"));
	MeshComp->SetupAttachment(BoxComp);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (MeshAsset.Succeeded())
	{
		MeshComp->SetStaticMesh(MeshAsset.Object);
		// pitch : Y
		// Yaw : Z
		// Roll : X
		MeshComp->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
		MeshComp->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
		MeshComp->SetWorldScale3D(FVector(1.0f));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Static Mesh를 가져오지 못했습니다."));
	}
	static ConstructorHelpers::FObjectFinder<UMaterial> MatAsset(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	if (MatAsset.Succeeded())
	{
		MeshComp->SetMaterial(0, MatAsset.Object);
	}
}

// Called when the game starts or when spawned
void AActorMonsterBase::BeginPlay()
{
	Super::BeginPlay();
	Direction = FVector(1.0f, 0.0f, 0.0f);

	// 확률에 따라 다른방향으로 직진하는거 만들고
	// 최대 5마리가 되면 스폰 멈추는 기능 만들고
	// 일정 범위 안에서 머물도록 만들고 
	





}

// Called every frame
void AActorMonsterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector NewLocation = GetActorLocation() + Direction * MoveSpeed * DeltaTime;
	SetActorLocation(NewLocation);

}

