// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WTUserWidget.h"
#include "WTCooldownWidget.generated.h"

/**
 * 
 */
enum class ESkillIdentifier : uint8;
class UImage;
class UTextBlock;

UCLASS()
class WANTEDPROJECT_API UWTCooldownWidget : public UWTUserWidget
{
	GENERATED_BODY()

public:
	UWTCooldownWidget(const FObjectInitializer& ObjectInitializer);

	// 부모(메인 HUD)가 이 위젯을 초기화할 때 호출할 함수
	void SetupSkillIcon(class UWTCooldownManagerComponent* InManager, ESkillIdentifier InSkillID);
	
	// 스킬이 사용되었을 때 쿨타임 효과를 시작하는 함수
	UFUNCTION()
	void StartCooldownUI(float CooldownDuration);
	
protected:
	// 위젯이 생성될 때 호출
	virtual void NativeConstruct() override;

	// 매 프레임 호출 (필요할 때만 활성화)
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	// 디자이너에서 설정한 위젯들
	UPROPERTY()
	TObjectPtr<UImage> Image_SkillIcon;
	
	UPROPERTY()
	TObjectPtr<UImage> Image_CooldownOverlay;
	
	UPROPERTY()
	TObjectPtr<UTextBlock> TextBlock_CooldownTime;

private:
	// 쿨다운 매니저에 대한 참조
	UPROPERTY()
	TObjectPtr<class UWTCooldownManagerComponent> CooldownManager;

	UPROPERTY()
	ESkillIdentifier SkillID;
	
	float CooldownTotalDuration;
};
