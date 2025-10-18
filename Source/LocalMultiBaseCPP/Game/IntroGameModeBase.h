// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "IntroGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class LOCALMULTIBASECPP_API AIntroGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AIntroGameModeBase();

protected:
    virtual void BeginPlay() override;

    // Start 버튼용 UI 클래스
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> StartWidgetClass;

private:
    UPROPERTY()
    UUserWidget* ActiveWidget;

    UFUNCTION()
    void OnStartClicked();
};