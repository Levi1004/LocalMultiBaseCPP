// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GameOverGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Blueprint/UserWidget.h"

AGameOverGameModeBase::AGameOverGameModeBase()
{
	// GameOver Level에서는 Pawn 스폰 안 함
	DefaultPawnClass = nullptr;
}

void AGameOverGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	if (GameOverWidgetClass)
	{
		ActiveWidget = CreateWidget<UUserWidget>(GetWorld(), GameOverWidgetClass);
		if (ActiveWidget)
		{
			ActiveWidget->AddToViewport();

			// UI 내 Restart 버튼 찾기
			if (UButton* RestartButton = Cast<UButton>(ActiveWidget->GetWidgetFromName(TEXT("RestartButton"))))
			{
				RestartButton->OnClicked.AddDynamic(this, &AGameOverGameModeBase::OnRestartClicked);
			}
		}
	}
}

void AGameOverGameModeBase::OnRestartClicked()
{
	if (ActiveWidget)
	{
		ActiveWidget->RemoveFromParent();
		ActiveWidget = nullptr;
	}

	// Intro Level 열기
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("IntroLevel"));
}
