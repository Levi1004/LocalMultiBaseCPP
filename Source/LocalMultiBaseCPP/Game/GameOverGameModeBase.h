// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "GameOverGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class LOCALMULTIBASECPP_API AGameOverGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGameOverGameModeBase();

protected:
	virtual void BeginPlay() override;

	// Restart 버튼 클릭 시 호출
	UFUNCTION()
	void OnRestartClicked();

protected:
	// GameOver UI 블루프린트 연결용
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> GameOverWidgetClass;

private:
	// 현재 화면에 띄운 UI
	UPROPERTY()
	UUserWidget* ActiveWidget = nullptr;
};