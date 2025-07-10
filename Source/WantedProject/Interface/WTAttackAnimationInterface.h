// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WTAttackAnimationInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UWTAttackAnimationInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class WANTEDPROJECT_API IWTAttackAnimationInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// LineTrace 함수.
	virtual void LineTraceSingle() = 0;

	// Attack 설정 및 발사 함수.
	virtual void SetAttacking(bool CanAttack) = 0;
	virtual void FireArrow() = 0;

	// UltimateSkill 설정 및 발사 함수.
	virtual void UltimateSkillArrow() = 0;
};
