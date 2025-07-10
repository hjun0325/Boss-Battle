// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WTMonsterAIInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UWTMonsterAIInterface : public UInterface
{
	GENERATED_BODY()
};

// 공격 종료 델리게이트.
DECLARE_DELEGATE(FAICharacterAttackFinished);

/**
 * 
 */
class WANTEDPROJECT_API IWTMonsterAIInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.public:
public:
	// 몬스터가 필수적으로 구현해야할 함수.
	virtual float GetAIPatrolRadius() = 0;
	virtual float GetAIDetectRange() = 0;
	virtual float GetAIAttackRange() = 0;
	virtual float GetAITurnSpeed() = 0;

	// AI가 호출할 공격 함수들.
	virtual void MeleeAttack(int32 AttackIndex) = 0;
	virtual void RangedAttack(int32 AttackIndex) = 0;
	virtual void SummonSkill() = 0;

	// AI가 호출할 순간 이동 함수.
	virtual void TeleportToTargetRear() = 0;

	// 스프린트 시작/종료 함수
	virtual void StartSprint() = 0;
	virtual void StopSprint() = 0;

	// 몬스터에서 델리게이트를 넘길 때 사용할 함수.
	virtual void SetAIAttackDelegate(
		const FAICharacterAttackFinished& InOnAttackFinished) = 0;
};
