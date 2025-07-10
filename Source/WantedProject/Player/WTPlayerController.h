// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "WTPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class WANTEDPROJECT_API AWTPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AWTPlayerController();

protected:
	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* InPawn) override;
	
	// HUD Section.
protected:
	// 생성할 위젯의 클래스 정보.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	TSubclassOf<class UWTHUDWidget> WTHUDWidgetClass;

	// 생성한 위젯을 객체 정보를 저장할 변수.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
	TObjectPtr<class UWTHUDWidget> WTHUDWidget;
};
