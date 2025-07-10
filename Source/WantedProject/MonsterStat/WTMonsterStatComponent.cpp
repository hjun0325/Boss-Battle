// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterStat/WTMonsterStatComponent.h"

// Sets default values for this component's properties
UWTMonsterStatComponent::UWTMonsterStatComponent()
{
}

void UWTMonsterStatComponent::InitializeStat(const FMonsterStatData& InStatData)
{
	MaxHp = InStatData.MaxHP;
	AttackDamage = InStatData.AttackDamage;
	SetHp(MaxHp);
}

void UWTMonsterStatComponent::ApplyDamage(float InDamage)
{
	// 데미지 계산.
	SetHp(CurrentHp - InDamage);
}

void UWTMonsterStatComponent::SetHp(float NewHp)
{
	// 현재 체력 업데이트.
	CurrentHp = FMath::Clamp<float>(NewHp, 0.0f, MaxHp);
	
	// 체력 변경 이벤트 발행.
	OnHpChanged.Broadcast(CurrentHp);

	// 죽었는지 (체력을 모두 소진했는지) 확인.
	if (CurrentHp <= KINDA_SMALL_NUMBER)
	{
		// 이벤트 발행.
		OnHpZero.Broadcast();
	}
}
