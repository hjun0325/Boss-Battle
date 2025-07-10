// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Monster/WTMonsterData.h"
#include "Interface/WTCharacterWidgetInterface.h"
#include "Interface/WTMonsterAIInterface.h"
#include "Interface/WTMonsterAttackInterface.h"
#include "WTMonsterBase.generated.h"

class UWTMonsterStatComponent;
class UDataTable;

UCLASS()
class WANTEDPROJECT_API AWTMonsterBase :
public ACharacter,
public IWTCharacterWidgetInterface,
public IWTMonsterAIInterface,
public IWTMonsterAttackInterface
{
	GENERATED_BODY()

public:
	AWTMonsterBase();
	
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

	// 데미지 처리 함수.
	virtual float TakeDamage(
		float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;

	// 캐릭터 위젯 설정 함수.
	virtual void SetupCharacterWidget(class UUserWidget* InUserWidget) override;
	
	// IABCharacterAIInterface 인터페이스 함수 구현.
	virtual float GetAIPatrolRadius() override;
	virtual float GetAIDetectRange() override;
	virtual float GetAIAttackRange() override;
	virtual float GetAITurnSpeed() override;

	// AI가 호출할 공격 함수들.
	virtual void MeleeAttack(int32 AttackIndex) override;
	virtual void RangedAttack(int32 AttackIndex) override;
	virtual void SummonSkill() override;

	// AI가 호출할 순간이동 함수.
	virtual void TeleportToTargetRear() override;

	// 스프린트 시작/종료 함수
	virtual void StartSprint() override;
	virtual void StopSprint() override;

	// 순간이동 시 사용할 이펙트 (사라질 때, 나타날 때)
	UPROPERTY(EditAnywhere, Category = "Boss|VFX")
	TObjectPtr<class UParticleSystem> TeleportOutVFX;
    
	UPROPERTY(EditAnywhere, Category = "Boss|VFX")
	TObjectPtr<class UParticleSystem> TeleportInVFX;

	// 원래의 걷기 속도를 저장해 둘 변수
	float OriginalWalkSpeed;

	// 캐릭터에서 델리게이트를 넘길 때 사용할 함수.
	virtual void SetAIAttackDelegate(
		const FAICharacterAttackFinished& InOnAttackFinished) override;

	/*// AI가 공격을 할 때 사용할 함수.
	virtual void AttackByAI() override;*/

	// AnimNotify에서 호출할 실제 데미지 판정 함수
	virtual void AttackHitCheck() override;

	// AnimNotify에서 호출할 구체 발사 함수.
	virtual void FireShadowBolt() override;
	
protected:
	// 죽음 상태 설정 함수.
	virtual void SetDead();

	// 죽는 애니메이션 재생 함수.
	void PlayDeadAnimation();

	// 애님 몽타주에서 제공하는 델리게이트와 파라미터 맞춤.
	void AttackActionEnd(class UAnimMontage* TargetMontage, bool IsProperlyEnded);

	void NotifyComboActionEnd();
	
	//-------------------------------------------------------------------------------

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	TObjectPtr<UWTMonsterStatComponent> StatComponent;
	
protected:
	// 죽은 뒤에 액터를 제거하기 전까지 대기할 시간 값.
	float DeadEventDelayTime = 5.0f;

	// UI Widget Section.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Widget, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UWTWidgetComponent> HpBar;
	
	// SetAIAttackDelegeate 함수로 전달된 델리게이트를 저장할 변수.
	FAICharacterAttackFinished OnAttackFinished;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
	TObjectPtr<UDataTable> MonsterDataTable;
    
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	FName MonsterID;

	// 데이터 테이블에서 읽어온 이 몬스터의 모든 정보를 저장할 변수.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data")
	FMonsterData CurrentMonsterData;

private:

	// 현재 실행 중인 근접 공격의 인덱스를 저장할 멤버 변수
	int32 CurrentMeleeAttackIndex;

	// 한 번의 공격에 여러 번 피해를 주는 것을 방지하기 위한 변수
	UPROPERTY()
	TArray<TObjectPtr<AActor>> HitActorsInSwing;

	// 디버깅 목적으로, 지정된 위치에 부채꼴을 그리는 헬퍼 함수
	void DrawDebugAttackSector(const FVector& Center, float Radius, float Angle, const FColor& Color);
};
