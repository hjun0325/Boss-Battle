// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "WTWidgetComponent.generated.h"

/**
 * 
 */
UCLASS()
class WANTEDPROJECT_API UWTWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

protected:
	// 위젯이 초기화될 때 호출되는 함수.
	virtual void InitWidget() override;
};
