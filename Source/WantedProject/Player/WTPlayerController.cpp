// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/WTPlayerController.h"
//#include "Blueprint/UserWidget.h"
#include "UI/WTHUDWidget.h"

AWTPlayerController::AWTPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	// 위젯 블루프린트 애셋에서 클래스 정보 로드.
	static ConstructorHelpers::FClassFinder<UWTHUDWidget> WTHUDWidgetRef(
		TEXT("/Game/WantedProject/UI/WBP_WTHUD.WBP_WTHUD_C"));
	if (WTHUDWidgetRef.Class)
	{
		WTHUDWidgetClass = WTHUDWidgetRef.Class;
	}
}

void AWTPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 위젯 생성.
	WTHUDWidget = CreateWidget<UWTHUDWidget>(this, WTHUDWidgetClass);

	// 위젯이 문제 없이 생성 됐으면,
	if (WTHUDWidget)
	{
		// 위젯을 화면에 추가해 UI가 보일 수 있도록 설정.
		WTHUDWidget->AddToViewport();
	}
}

void AWTPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	/*// 입력 모드를 게임 전용으로 설정.
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);*/

	/*// 게임 플레이와 UI 상호작용이 모두 가능한 모드로 설정.
	FInputModeGameAndUI InputMode;
	SetInputMode(InputMode);*/
}
