// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WTHUDWidget.h"

#include "WTCooldownWidget.h"
#include "Kismet/GameplayStatics.h"
#include "WTDashWidget.h"
#include "WTHpBarWidget.h"
#include "CharacterCoolDown/WTCooldownManagerComponent.h"
#include "Components/ProgressBar.h"
#include "Interface/WTBossHUDInterface.h"
#include "Interface/WTCharacterHUDInterface.h"

UWTHUDWidget::UWTHUDWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UWTHUDWidget::UpdateDashCooldown(float Percent)
{
	if (Dash==nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UWTHUDWidget::UpdateDashCooldown"));
	}
	else
	{
		Dash->SetPercent(Percent);		
	}
}

void UWTHUDWidget::UpdatePlayerHpBar(float NewCurrentHp)
{
	PlayerHpBar->UpdateHpBar(NewCurrentHp);
}

void UWTHUDWidget::UpdateBossHpBar(float NewCurrentHp)
{
	BossHpBar->UpdateHpBar(NewCurrentHp);
}

void UWTHUDWidget::PlayerSetMaxHp(float MaxHp)
{
	PlayerHpBar->SetMaxHp(MaxHp);
}

void UWTHUDWidget::BossSetMaxHp(float MaxHp)
{
	BossHpBar->SetMaxHp(MaxHp);
}

void UWTHUDWidget::OnCooldownStarted(ESkillIdentifier SkillID, float CooldownDuration)
{
	// 어떤 스킬의 쿨타임이 시작되었는지 확인하고, 해당하는 아이콘 위젯에게 알려줍니다.
	switch (SkillID)
	{
	case ESkillIdentifier::Dash:
		break;
	case ESkillIdentifier::UltimateAttack:
		if (SkillIcon) SkillIcon->StartCooldownUI(CooldownDuration);
		break;
	default:
		break;
	}
}

void UWTHUDWidget::HandleChargeStateChanged(bool bIsCharging)
{
	if (!ChargeGauge) return;

	ChargeGauge->SetVisibility(bIsCharging ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

	// 차징이 끝났다면, 게이지를 0으로 리셋.
	if (!bIsCharging)
	{
		ChargeGauge->SetPercent(0.0f);
	}
}

void UWTHUDWidget::HandleChargeProgressUpdated(float Progress)
{
	if (!ChargeGauge) return;
	
	// 전달받은 Progress 값(0.0 ~ 1.0)을 프로그레스 바의 퍼센트 값으로 설정.
	ChargeGauge->SetPercent(Progress);
}

void UWTHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Dash = Cast<UWTDashWidget>(GetWidgetFromName(TEXT("WidgetDash")));
	ensure(Dash);

	PlayerHpBar = Cast<UWTHpBarWidget>(GetWidgetFromName(TEXT("WidgetPlayerHpBar")));
	ensure(PlayerHpBar);

	BossHpBar = Cast<UWTHpBarWidget>(GetWidgetFromName(TEXT("WidgetBossHpBar")));
	ensure(BossHpBar);

	SkillIcon = Cast<UWTCooldownWidget>(GetWidgetFromName(TEXT("WidgetSkillIcon")));
	ensure(SkillIcon);

	ChargeGauge = Cast<UProgressBar>(GetWidgetFromName(TEXT("PB_ChargeGauge")));
	ensure(ChargeGauge);
	
	ChargeGauge->SetVisibility(ESlateVisibility::Hidden);
	
	//GetOwningPlayer()->GetPawn();
	IWTCharacterHUDInterface* PlayerHUDPawn = Cast<IWTCharacterHUDInterface>(GetOwningPlayerPawn());
	if (PlayerHUDPawn)
	{
		SkillIcon->SetupSkillIcon(PlayerHUDPawn->GetCooldownManagerComponent(), ESkillIdentifier::UltimateAttack);
		PlayerHUDPawn->SetupHUDWidget(this);
	}

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Boss"), FoundActors);
	
	if (FoundActors.Num() > 0 && FoundActors[0])
	{
		AActor* BossActor = FoundActors[0];
		IWTBossHUDInterface* BossHUDPawn = Cast<IWTBossHUDInterface>(BossActor);
		if (BossHUDPawn)
		{
			BossHUDPawn->SetupHUDWidget(this);
		}
	}
}
