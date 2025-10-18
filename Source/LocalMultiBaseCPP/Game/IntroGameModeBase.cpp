// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/IntroGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Blueprint/UserWidget.h"
#include "Engine/GameInstance.h"

AIntroGameModeBase::AIntroGameModeBase()
{
    DefaultPawnClass = nullptr; // Intro 레벨에서는 Pawn 필요 없음
}

void AIntroGameModeBase::BeginPlay()
{
    Super::BeginPlay();

    if (!StartWidgetClass) return;

    ActiveWidget = CreateWidget<UUserWidget>(GetWorld(), StartWidgetClass);
    if (ActiveWidget)
    {
        ActiveWidget->AddToViewport();

        // Start 버튼 찾아서 클릭 이벤트 연결
        if (UButton* StartButton = Cast<UButton>(ActiveWidget->GetWidgetFromName(TEXT("StartButton"))))
        {
            StartButton->OnClicked.AddDynamic(this, &AIntroGameModeBase::OnStartClicked);
        }
    }
}

void AIntroGameModeBase::OnStartClicked()
{
    // 기존 LocalPlayer 제거 (중복 스폰 방지)
    if (UGameInstance* GI = GetGameInstance())
    {
        TArray<ULocalPlayer*> LocalPlayers = GI->GetLocalPlayers();
        for (ULocalPlayer* LP : LocalPlayers)
        {
            GI->RemoveLocalPlayer(LP);
        }
    }

    // Start UI 제거
    if (ActiveWidget)
    {
        ActiveWidget->RemoveFromParent();
        ActiveWidget = nullptr;
    }

    // MainMap으로 이동
    UGameplayStatics::OpenLevel(GetWorld(), FName("MainMap"));
}