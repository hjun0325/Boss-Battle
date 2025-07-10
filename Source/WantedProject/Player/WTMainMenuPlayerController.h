// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "WTMainMenuPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class WANTEDPROJECT_API AWTMainMenuPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AWTMainMenuPlayerController();
	
	// 시작 버튼을 눌렀을 때 호출될 함수.
	// UFUNCTION(BlueprintCallable)을 통해 블루프린트에서도 이 함수를 호출할 수 있게 됩니다.
	UFUNCTION(BlueprintCallable)
	void StartGame();
	
protected:
	// 레벨이 시작될 때 호출되는 함수.
	virtual void BeginPlay() override;

private:
	// 에디터에서 설정할 UI 위젯 블루프린트의 클래스를 담을 변수.
	// TSubclassOf는 특정 클래스의 자식 클래스만 담을 수 있도록 제한.
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> MainMenuWidgetClass;

	// 생성된 위젯의 인스턴스를 담을 변수.
	UPROPERTY()
	class UUserWidget* MainMenuWidgetInstance;
};
