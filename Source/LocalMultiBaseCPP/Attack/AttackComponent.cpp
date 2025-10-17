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

     float ScaleFactor = OwnerActor->GetActorScale3D().X; // 일반적으로 X, Y, Z 동일
     float ScaledRange = AttackRange * ScaleFactor;


     FVector Start = OwnerActor->GetActorLocation() + FVector(0, 0, 500); // 캐릭터 높이 보정
     FVector End = Start + OwnerActor->GetActorForwardVector() * ScaledRange;

     FHitResult Hit;
     FCollisionQueryParams Params;
     Params.AddIgnoredActor(OwnerActor);

     if (OwnerActor->GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
     {
         ALMBpawnPlayer* Player = Cast<ALMBpawnPlayer>(OwnerActor);
         float Damage = Player ? Player->AttackPower : 0.f;

         if (AActorMonsterBase* HitMonster = Cast<AActorMonsterBase>(Hit.GetActor()))
         {
             HitMonster->ApplyDamage(Damage, Player);
             UE_LOG(LogTemp, Warning, TEXT("%s hit Monster %s for %f damage"), *OwnerActor->GetName(), *HitMonster->GetName(), Damage);
         }
         else if (ABossMonster* HitBoss = Cast<ABossMonster>(Hit.GetActor())) 
         {
             HitBoss->ApplyDamage(Damage, Player);
             UE_LOG(LogTemp, Warning, TEXT("%s hit Boss %s for %f damage"), *OwnerActor->GetName(), *HitBoss->GetName(), Damage);
         }
     }

        // 디버그 라인
        DrawDebugLine(OwnerActor->GetWorld(), Start, End, FColor::Red, true, 10.f, 0, 10.f);
       // UE_LOG(LogTemp, Warning, TEXT("PerformAttack called! Start=%s End=%s"), *Start.ToString(), *End.ToString());
    }



