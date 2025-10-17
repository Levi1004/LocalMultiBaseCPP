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

    PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	SetRootComponent(BoxComponent);
	BoxComponent->SetBoxExtent(FVector(50.0f, 50.0f, 100.0f));
	BoxComponent->SetRelativeLocation(FVector(0.f, 0.f, 100.f));

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	MeshComponent->SetupAttachment(BoxComponent);

	MeshComponent->SetRelativeLocation(FVector(0.f, 0.f, -50.f));
	// FObjectfinder<가져올 오브젝트 타입>
	// CubeMesh : 변수 이름
	// () 경로 입력
	// 로드하는 시간이 오래걸린다. / 변하지 않을거에 적용 
	
	//FClassFinder : 경로에있는 클래스를 가져온다.
	// 클래스를 경로로 가져올땐 경로 뒤에 _C를 붙여줘야 한다.
	
    // ===== 이동 관련 =====
	PawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("PawnMovement"));
	PawnMovement->MaxSpeed = MaxSpeed; // 최대로 낼 수 있는 이동 속도
	PawnMovement->Acceleration = 500.0f; // 얼마나 빠르게 속도가 증가할 지
	PawnMovement->Deceleration = 2000.0f; // 얼마나 빠르게 감속할 지 
	PawnMovement->TurningBoost = 8.0f; // 회전을 얼마나 민첩하게할 지
   
    // ===== 카메라 관련 =====
    SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
    SpringArmComp->SetupAttachment(RootComponent);
    SpringArmComp->SetUsingAbsoluteRotation(false);
	SpringArmComp->SetRelativeLocation(FVector(0.f, 0.f, 150.f));
    SpringArmComp->SetRelativeRotation(FRotator(-50.f, 0.f, 0.f));
    SpringArmComp->TargetArmLength = 10000.0f;
    SpringArmComp->bUsePawnControlRotation = false;
	SpringArmComp->bDoCollisionTest = false;

    CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
    CameraComp->SetupAttachment(SpringArmComp);
    CameraComp->bUsePawnControlRotation = false;




	CurrentHp = MaxHealth;
	UE_LOG(LogTemp, Warning, TEXT("플레이어 0 현재 체력 %f"), CurrentHp);
	


}
void ALMBpawnBase::BeginPlay()
{
	Super::BeginPlay();

}

// -----------------------------
// Movement & Rotation Logic
// -----------------------------
void ALMBpawnBase::OnInputMove(const FVector2D& MoveVector)
{
    CurrentMoveVector = MoveVector;

    // ---- 좌/우 회전 ----
    if (!FMath::IsNearlyZero(MoveVector.Y))
    {
        FRotator NewRot = GetActorRotation();
        NewRot.Yaw += MoveVector.Y * CameraRotationSpeed * GetWorld()->GetDeltaSeconds();
        SetActorRotation(NewRot);
    }

    // ---- 전/후 이동 ----
    if (!FMath::IsNearlyZero(MoveVector.X))
    {
        FVector Forward = GetActorForwardVector();
        AddMovementInput(Forward, MoveVector.X);
    }
}
// Optional: Tick에서 회전 보간 적용 가능
void ALMBpawnBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 필요시 추가 로직 가능 (예: 공격 중 이동 제한 등)
}

float ALMBpawnBase::TakeDamage(
	float DamageAmount,
	const FDamageEvent& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCauser
)
{
	// 방어력 적용
	float ActualDamage = FMath::Max(DamageAmount - Defence, 0.f);
	CurrentHp -= ActualDamage;

	UE_LOG(LogTemp, Warning, TEXT("%s took %f damage. Remaining HP: %f"), *GetName(), ActualDamage, CurrentHp);

	if (CurrentHp <= 0.f)
	{
		Die();
	}

	return ActualDamage;
}

void ALMBpawnBase::Die()
{
	UE_LOG(LogTemp, Warning, TEXT("%s has died"), *GetName());
	Destroy();
}
void ALMBpawnBase::ApplyMeshByPlayerIndex()
{
	UE_LOG(LogTemp, Warning, TEXT("ALMBpawnBase::ApplyMeshByPlayerIndex() 호출"));
}
