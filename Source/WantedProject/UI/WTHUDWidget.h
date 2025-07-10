// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WTHUDWidget.generated.h"

class UWTDashWidget;
class UWTHpBarWidget;
class UWTCooldownWidget;
class UProgressBar;

UCLASS()
class WANTEDPROJECT_API UWTHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UWTHUDWidget(const FObjectInitializer& ObjectInitializer);

	// 대쉬 쿨이 업데이트될 때 사용할 함수.
	void UpdateDashCooldown(float Percent);

	void UpdatePlayerHpBar(float NewCurrentHp);
	void UpdateBossHpBar(float NewCurrentHp);

	void PlayerSetMaxHp(float MaxHp);
	void BossSetMaxHp(float MaxHp);
	
	// 쿨타임 시작 방송을 수신할 함수
	UFUNCTION()
	void OnCooldownStarted(ESkillIdentifier SkillID, float CooldownDuration);

	// 플레이어 캐릭터의 OnChargeStateChanged 델리게이트에 바인딩(연결)될 함수입니다.
	// 델리게이트에 연결하려면 반드시 UFUNCTION() 매크로가 필요합니다.
	UFUNCTION()
	void HandleChargeStateChanged(bool bIsCharging);

	// 플레이어 캐릭터의 OnChargeProgressUpdated 델리게이트에 바인딩될 함수입니다.
	UFUNCTION()
	void HandleChargeProgressUpdated(float Progress);

protected:
	virtual void NativeConstruct() override;

	// Dash 위젯.
	UPROPERTY()
	TObjectPtr<UWTDashWidget> Dash;

	// Player HpBar 위젯.
	UPROPERTY()
	TObjectPtr<UWTHpBarWidget> PlayerHpBar;

	// Boss HpBar 위젯.
	UPROPERTY()
	TObjectPtr<UWTHpBarWidget> BossHpBar;

	// Skill Icon 위젯.
	UPROPERTY()
	TObjectPtr<UWTCooldownWidget> SkillIcon;

	// ChargeGauge 위젯.
	UPROPERTY()
	TObjectPtr<UProgressBar> ChargeGauge;
};
