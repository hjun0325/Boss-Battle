// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterCoolDown/WTCooldownManagerComponent.h"

// Sets default values for this component's properties
UWTCooldownManagerComponent::UWTCooldownManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UWTCooldownManagerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UWTCooldownManagerComponent::StartCooldown(ESkillIdentifier Skill, float CooldownDuration)
{
	// 유효하지 않은 스킬이나 쿨타임이 0이하이면 무시.
	if (Skill == ESkillIdentifier::None || CooldownDuration <= 0.0f) return;

	// 현재 게임 시간에 쿨타임 지속 시간을 더하여, 쿨타임이 "끝나는 시간'을 계산.
	const float CurrentTime = GetWorld()->GetTimeSeconds();
	const float EndTime = CurrentTime + CooldownDuration;

	// 맵에 해당 스킬의 쿨타임 종료 시간을 기록(또는 갱신).
	CooldownEndTimeMap.Add(Skill, EndTime);
	
	// 쿨타임을 기록한 후, 이 스킬의 쿨타임이 시작되었음을 외부에 방송.
	OnCooldownStarted.Broadcast(Skill, CooldownDuration);
}

bool UWTCooldownManagerComponent::IsOnCooldown(ESkillIdentifier Skill)
{
	// 맵에 해당 스킬의 기록이 있는지 먼저 확인한다.
	if (CooldownEndTimeMap.Contains(Skill))
	{
		// 현재 시간이 기록된 종료 시간보다 작다면, 아직 쿨타임 중.
		const float CurrentTime = GetWorld()->GetTimeSeconds();
		return CurrentTime < CooldownEndTimeMap[Skill];
	}
	
	return false;
}

float UWTCooldownManagerComponent::GetRemainingCooldown(ESkillIdentifier Skill)
{
	if (CooldownEndTimeMap.Contains(Skill))
	{
		const float CurrentTime = GetWorld()->GetTimeSeconds();
		const float EndTime = CooldownEndTimeMap[Skill];

		// 남은 시간을 계산하여 반환. 0보다 작으면 0을 반환.
		return FMath::Max(0.0f,EndTime - CurrentTime);
	}
	return 0.0f;
}
