// Fill out your copyright notice in the Description page of Project Settings.


#include "Attack/AttackComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Actor.h"
#include "Pawn/LMBpawnPlayer.h"
#include "Monster/ActorMonsterBase.h"

// Sets default values for this component's properties
 UAttackComponent::UAttackComponent()
 {
        PrimaryComponentTick.bCanEverTick = false;
 }

 void UAttackComponent::PerformAttack(AActor * OwnerActor, float AttackRange)
    {
     if (!OwnerActor) return;

     FVector Start = OwnerActor->GetActorLocation() + FVector(0, 0, 100); // 캐릭터 높이 보정
     FVector End = Start + OwnerActor->GetActorForwardVector() * AttackRange;

     FHitResult Hit;
     FCollisionQueryParams Params;
     Params.AddIgnoredActor(OwnerActor);

     if (OwnerActor->GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
     {
         if (AActorMonsterBase* HitMonster = Cast<AActorMonsterBase>(Hit.GetActor()))
         {
             ALMBpawnPlayer* Player = Cast<ALMBpawnPlayer>(OwnerActor);
             float Damage = 0.f;

             if (Player)
             {
                 Damage = Player->AttackPower;
                 HitMonster->ApplyDamage(Damage, Player);
                 UE_LOG(LogTemp, Warning, TEXT("%s hit %s for %f damage"), *OwnerActor->GetName(), *HitMonster->GetName(), Damage);
             }
         }
     }

        // 디버그 라인
        DrawDebugLine(OwnerActor->GetWorld(), Start, End, FColor::Red, true, 10.f, 0, 10.f);
       // UE_LOG(LogTemp, Warning, TEXT("PerformAttack called! Start=%s End=%s"), *Start.ToString(), *End.ToString());
    }



