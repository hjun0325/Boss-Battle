// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/WTMainMenuPlayerController.h"

#include "Blueprint/UserWidget.h" // 위젯 사용을 위해 필요
#include "Kismet/GameplayStatics.h" // 레벨 전환을 위해 필요

AWTMainMenuPlayerController::AWTMainMenuPlayerController()
{
}

void AWTMainMenuPlayerController::StartGame()
{
	// "MainMap"이라는 이름의 레벨을 엽니다.
	UGameplayStatics::OpenLevel(this, FName("MainMap1"));
}

void AWTMainMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	// 유효한지(에디터에서 설정 되었는지) 확인한다.
	if (MainMenuWidgetClass)
	{
		// 위젯을 생성한다.
		MainMenuWidgetInstance = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);

		if (MainMenuWidgetInstance)
		{
			// 위젯을 화면에 추가한다.
			MainMenuWidgetInstance->AddToViewport();

			// 마우스 커서를 표시한다.
			bShowMouseCursor = true;

			/*// 입력 모드를  UI전용으로 설정한다.
			FInputModeUIOnly InputMode;
			SetInputMode(InputMode);*/
		}
	}
}
