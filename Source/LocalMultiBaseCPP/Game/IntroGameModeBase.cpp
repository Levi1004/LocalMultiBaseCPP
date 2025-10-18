// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/IntroGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Blueprint/UserWidget.h"

AIntroGameModeBase::AIntroGameModeBase()
{
    DefaultPawnClass = nullptr; // Intro 레벨은 Pawn 필요 없음
}

void AIntroGameModeBase::BeginPlay()
{
    Super::BeginPlay();

    if (StartWidgetClass)
    {
        ActiveWidget = CreateWidget<UUserWidget>(GetWorld(), StartWidgetClass);
        if (ActiveWidget)
        {
            ActiveWidget->AddToViewport();

            if (UButton* StartButton = Cast<UButton>(ActiveWidget->GetWidgetFromName(TEXT("StartButton"))))
            {
                StartButton->OnClicked.AddDynamic(this, &AIntroGameModeBase::OnStartClicked);
            }
        }
    }
}

void AIntroGameModeBase::OnStartClicked()
{
    if (ActiveWidget)
    {
        ActiveWidget->RemoveFromParent();
        ActiveWidget = nullptr;
    }

    UGameplayStatics::OpenLevel(GetWorld(), TEXT("MainMap")); // MainMap 이름으로 교체
}