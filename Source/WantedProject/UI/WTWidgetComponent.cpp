// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WTWidgetComponent.h"
#include "WTUserWidget.h"

void UWTWidgetComponent::InitWidget()
{
	// InitWidget 함수가 호출되면 WidgetClass 정보를 바탕으로 위젯 인스턴스를 생성함.
	// 내부에서 CreateWidget 함수를 실행.
	Super::InitWidget();

	// Super::InitWidget() 함수 호출 이후에는 위젯 인스턴스가 있다고 확인할 수 있음.
	UWTUserWidget* WTUserWidget = Cast<UWTUserWidget>(GetWidget());
	if (WTUserWidget)
	{
		WTUserWidget->SetOwningActor(GetOwner());
	}
}
