// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStat/WTCharacterStatComponent.h"

// Sets default values for this component's properties
UWTCharacterStatComponent::UWTCharacterStatComponent()
{
	MaxHp = 200.0f;
	SetHp(MaxHp);
}

float UWTCharacterStatComponent::ApplyDamage(float InDamage)
{
	// 데미지 처리.
	const float PrevHp = CurrentHp;

	// 데미지 보정 값.
	// 오류로 인해 전달된 데미지가 음수일 수 있음.
	// 음수인 경우는 0설정.
	const float ActualDamage = FMath::Clamp<float>(InDamage, 0.0f ,InDamage);
	
	// 데미지 계산.
	SetHp(PrevHp - ActualDamage);

	// 죽었는지 (체력을 모두 소진했는지) 확인.
	if (CurrentHp <= KINDA_SMALL_NUMBER)
	{
		// 이벤트 발행.
		OnHpZero.Broadcast();
	}
	
	return ActualDamage;
}

void UWTCharacterStatComponent::SetHp(float NewHp)
{
	// 현재 체력 업데이트.
	CurrentHp = FMath::Clamp<float>(NewHp, 0.0f, MaxHp);
	
	// 체력 변경 이벤트 발행.
	OnHpChanged.Broadcast(CurrentHp);
}
