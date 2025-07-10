// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Monster/WTMonsterData.h"
#include "WTMonsterStatComponent.generated.h"

// 델리게이트 선언.
// 체력 값이 0이 되었을 때 발행할 델리게이트.
DECLARE_MULTICAST_DELEGATE(FOnHpZeroDelegate);

// 체력 변경이 발생할 때 발행할 델리게이트.
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHpChangedDelegate, float /*CurrentHp*/);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WANTEDPROJECT_API UWTMonsterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWTMonsterStatComponent();

	// Getter.
	FORCEINLINE float GetMaxHp() const { return MaxHp; }
	FORCEINLINE float GetCurrentHp() const { return CurrentHp; }

	// 스탯 초기화.
	void InitializeStat(const FMonsterStatData& InStatData);
	
	// 데미지 전달 함수.
	void ApplyDamage(float InDamage);
	
protected:
	// HP가 변경됐을 때 실행할 함수.
	void SetHp(float NewHp);

public:
	// 체력을 모두 소진했을 때 발행되는 델리게이트.
	FOnHpZeroDelegate OnHpZero;

	// 체력 변경 델리게이트.
	FOnHpChangedDelegate OnHpChanged;

protected:
	UPROPERTY(VisibleAnywhere, Category="Stat") float MaxHp;
	UPROPERTY(Transient, VisibleAnywhere, Category="Stat") float CurrentHp;
	UPROPERTY(VisibleAnywhere, Category="Stat") float AttackDamage;
};
