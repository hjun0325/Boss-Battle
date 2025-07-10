// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WTUserWidget.h"
#include "WTDashWidget.generated.h"

/**
 * 
 */
UCLASS()
class WANTEDPROJECT_API UWTDashWidget : public UWTUserWidget
{
	GENERATED_BODY()

public:
	UWTDashWidget(const FObjectInitializer& ObjectInitializer);

	// 대시 쿨타임 퍼센트 설정 함수.
	void SetPercent(float InPercent);

protected:
	// UMG가 초기화될 때 호출되는 함수.
	virtual void NativeConstruct() override;

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float Percent=0.0f;

protected:
	// 쿨타임 이미지 위젯.
	UPROPERTY()
	TObjectPtr<class UImage> CooldownImage;

	// 머티리얼의 공통 기반 클래스 (UMaterial과 UMaterialInstance의 부모 클래스)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cooldown")
	TObjectPtr<class UMaterialInterface> CooldownMaterialBase;

	// 런타임에 머티리얼 파라미터를 변경할 수 있는 머티리얼 인스턴스.
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> RoundProgressbarInst;
};
