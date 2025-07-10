// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Monster/WTMonsterBase.h"
#include "Interface/WTBossHUDInterface.h"
#include "WTBossMonsterNecromancer.generated.h"

// 보스의 페이즈를 나타낼 열거형
UENUM(BlueprintType)
enum class EBossPhase : uint8
{
	Phase1_Ranged, // 원거리 패턴만 사용하는 1페이즈
	Phase2_Hybrid  // 근/원거리 하이브리드 2페이즈
};

/**
 * 
 */
UCLASS()
class WANTEDPROJECT_API AWTBossMonsterNecromancer :
public AWTMonsterBase,
public IWTBossHUDInterface
{
	GENERATED_BODY()

public:
	AWTBossMonsterNecromancer();

	/*// AI 비헤이비어 트리에서 호출할 순간이동 함수
	UFUNCTION(BlueprintCallable, Category = "AI")
	void TeleportToTarget();*/

protected:
	// 게임 시작 시 한 번만 호출.
	virtual void BeginPlay() override;
	
	// 페이즈 전환 체크를 위해 TakeDamage 함수를 재정의(override)
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	// SummonSkill.
	virtual void SummonSkill() override;

	virtual void FireShadowBolt() override;
	
	virtual void SetupHUDWidget(class UWTHUDWidget* InHUDWidget) override;

	virtual void SetDead() override;
	
	// 찾은 ObjectPoolManager를 저장할 포인터 변수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pool")
	TObjectPtr<class AObjectPoolManager> PoolManager;
	
private:
	// 페이즈 2로 전환될 때 한 번만 호출될 내부 함수
	void EnterPhase2();

	// 현재 페이즈를 저장하는 변수
	EBossPhase CurrentPhase;
    
	// 페이즈가 전환되는 체력 퍼센티지 (예: 0.5f = 50%)
	UPROPERTY(EditDefaultsOnly, Category = "Boss")
	float PhaseChangeHealthPercentage;

	/*// 순간이동 시 사용할 파티클 이펙트
	UPROPERTY(EditDefaultsOnly, Category = "Boss")
	TObjectPtr<class UParticleSystem> TeleportEffect;*/
	
	// 마법 구체.
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWTShadowBolt> BoltClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> ActorClass;
};
