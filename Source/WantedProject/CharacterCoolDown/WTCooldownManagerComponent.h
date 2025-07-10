// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WTCooldownManagerComponent.generated.h"

// 스킬 식별을 위한 열거형.
UENUM(BlueprintType)
enum class ESkillIdentifier : uint8
{
	None,
	Dash,
	UltimateAttack
};

// 스킬 ID를 파라미터로 전달하는 델리게이트를 선언.
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCooldownStartedDelegate, ESkillIdentifier /*Skill*/, float /*CooldownDuration*/);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WANTEDPROJECT_API UWTCooldownManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWTCooldownManagerComponent();

	// 특정 스킬의 쿨타임을 시작.
	void StartCooldown(ESkillIdentifier Skill, float CooldownDuration);
	
	// 특정 스킬이 현재 쿨타임 중인지 확인.
	UFUNCTION(BlueprintCallable, Category = "Cooldown")
	bool IsOnCooldown(ESkillIdentifier Skill);

	// 특정 스킬의 남은 쿨타임을 초 단위로 가져온다. (UI 표시용)
	UFUNCTION(BlueprintCallable, Category = "Cooldown")
	float GetRemainingCooldown(ESkillIdentifier Skill);
	
	// 다른 클래스들이 구독할 수 있는 '쿨타임 시작' 방송 채널.
	FOnCooldownStartedDelegate OnCooldownStarted;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	// 각 스킬의 쿨타임이 '끝나는 시간'을 저장하는 맵.
	UPROPERTY()
	TMap<ESkillIdentifier, float> CooldownEndTimeMap;
};
