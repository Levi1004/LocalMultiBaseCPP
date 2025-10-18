// Fill out your copyright notice in the Description page of Project Settings.


#include "Attack/AttackComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Actor.h"
#include "Pawn/LMBpawnPlayer.h"
#include "Monster/ActorMonsterBase.h"
#include "Monster/BossMonster.h"

// Sets default values for this component's properties
 UAttackComponent::UAttackComponent()
 {
        PrimaryComponentTick.bCanEverTick = false;
 }

 void UAttackComponent::PerformAttack(AActor* OwnerActor, float AttackRange)
 {
     if (!OwnerActor) return;

     float ScaleFactor = OwnerActor->GetActorScale3D().X;
     float ScaledRange = AttackRange * ScaleFactor;

     // 캐릭터 중심 높이 + 보정
     FVector Start = OwnerActor->GetActorLocation() + FVector(0, 0, 300);

     // 앞으로만 Sweep
     float TraceDistance = 2000.f; // 캐릭터 앞으로 얼마까지 공격할지
     FVector End = Start + OwnerActor->GetActorForwardVector() * TraceDistance;

     FCollisionQueryParams Params;
     Params.AddIgnoredActor(OwnerActor);

     // SphereTrace 반경
     float TraceRadius = 800.f;

     // 여러 개의 충돌 결과를 받을 배열
     TArray<FHitResult> HitResults;

     // SphereTrace로 여러 몬스터 감지 (앞쪽만)
     if (OwnerActor->GetWorld()->SweepMultiByChannel(
         HitResults,
         Start,
         End,
         FQuat::Identity,
         ECC_Visibility,
         FCollisionShape::MakeSphere(TraceRadius),
         Params))
     {
         ALMBpawnPlayer* Player = Cast<ALMBpawnPlayer>(OwnerActor);
         float Damage = Player ? Player->AttackPower : 0.f;

         for (const FHitResult& Hit : HitResults)
         {
             AActor* HitActor = Hit.GetActor();
             if (!HitActor) continue;

             if (AActorMonsterBase* HitMonster = Cast<AActorMonsterBase>(HitActor))
             {
                 HitMonster->ApplyDamage(Damage, Player);
                 UE_LOG(LogTemp, Warning, TEXT("%s hit Monster %s for %f damage"), *OwnerActor->GetName(), *HitMonster->GetName(), Damage);
             }
             else if (ABossMonster* HitBoss = Cast<ABossMonster>(HitActor))
             {
                 HitBoss->ApplyDamage(Damage, Player);
                 UE_LOG(LogTemp, Warning, TEXT("%s hit Boss %s for %f damage"), *OwnerActor->GetName(), *HitBoss->GetName(), Damage);
             }
         }
     }

     // 디버그용: SphereTrace 위치와 반경 시각화 (캐릭터 바로 앞쪽)
     FVector SphereCenter = Start + OwnerActor->GetActorForwardVector() * (TraceDistance / 2);
     DrawDebugSphere(
         OwnerActor->GetWorld(),
         SphereCenter,
         TraceRadius,
         12,
         FColor::Red,
         false,
         2.f
     );

     // 공격 라인 디버그 (선으로 확인용)
     DrawDebugLine(OwnerActor->GetWorld(), Start, End, FColor::Green, false, 2.f, 0, 5.f);
 }