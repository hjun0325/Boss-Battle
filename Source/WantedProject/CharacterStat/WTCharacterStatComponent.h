// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WTCharacterStatComponent.generated.h"

// 델리게이트 선언.
// 체력 값이 0이 되었을 때 발행할 델리게이트.
DECLARE_MULTICAST_DELEGATE(FOnHpZeroDelegate);

// 체력 변경이 발생할 때 발행할 델리게이트.
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHpChangedDelegate, float /*CurrentHp*/);

/*// 스탯 정보 변경이 발생할 때 발행할 델리게이트.
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnStatChangedDelegate,
	const FABCharacterStat& /*BaseStat#1#, const FABCharacterStat& /*ModifierStat#1#);*/

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WANTEDPROJECT_API UWTCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWTCharacterStatComponent();
	
	// Getter.
	FORCEINLINE float GetMaxHp() const { return MaxHp; }
	FORCEINLINE float GetCurrentHp() const { return CurrentHp; }
	
	// 데미지 전달 함수.
	float ApplyDamage(float InDamage);

protected:
	// HP가 변경됐을 때 실행할 함수.
	void SetHp(float NewHp);

public:
	// 체력을 모두 소진했을 때 발행되는 델리게이트.
	FOnHpZeroDelegate OnHpZero;

	// 체력 변경 델리게이트.
	FOnHpChangedDelegate OnHpChanged;

	/*// 스탯 변경 델리게이트.
	FOnStatChangedDelegate OnStatChanged;*/
	
protected:
	// 아래의 속성들은 기본 값이기도 하지만
	// 배치한 캐릭터마다 서로 다른 값을 가지고 있기 때문에
	// 인스턴스마다 별도로 수행되는 것이 좋다. (VisibleInstanceOnly)
	UPROPERTY(VisibleInstanceOnly, Category="Stat")
	float MaxHp;

	// 스탯 컴포넌트와 같은 오브젝트를 저장할 때 속성들이 보통 디스크에 저장되는데
	// 현재 Hp값은 계속 새롭게 지정되므로 디스크에 저장할 필요가 없다.
	// 저장할 때 불필요한 공간이 낭비되지 않도록 지정 (Transient)
	UPROPERTY(Transient, VisibleInstanceOnly, Category="Stat")
	float CurrentHp;
};
