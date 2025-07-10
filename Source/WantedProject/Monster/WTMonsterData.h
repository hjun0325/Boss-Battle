#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "WTMonsterData.generated.h" // 매크로를 사용하기 위해 필요한 헤더.

// 스탯 전용 데이터 구조체.
USTRUCT(BlueprintType)
struct FMonsterStatData
{
	GENERATED_BODY()

	FMonsterStatData() : MaxHP(0.0f), AttackDamage(0.0f) {};
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxHP;
    
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AttackDamage;
};

// 근접 공격 정보 구조체.
USTRUCT(BlueprintType)
struct FMeleeAttackData {
	GENERATED_BODY()

	// --- 생성자 추가 ---
	FMeleeAttackData() :
		Montage(nullptr),
		DamageMultiplier(1.0f), AttackRange(0.0f), AttackAngle(0.0f) {}
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> Montage;

	// --- 판정 관련 데이터 추가 ---
	// 데미지 배율 설정.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DamageMultiplier;

	// 공격의 최대 사거리 (부채꼴의 반지름)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AttackRange; 

	// 공격의 전체 각도 (예: 90도, 120도)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AttackAngle;
};

// 원거리 공격 정보 구조체.
USTRUCT(BlueprintType)
struct FRangedAttackData {
	GENERATED_BODY()

	// --- 생성자 추가 ---
	FRangedAttackData() : Montage(nullptr), DamageMultiplier(0.0f) {}
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> Montage;

	// 데미지 배율 설정.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DamageMultiplier;

	// 발사할 투사체.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AActor> ProjectileClass;
};

// 소환 스킬 정보 구조체.
USTRUCT(BlueprintType)
struct FSummonSkillData {
	GENERATED_BODY()

	// --- 생성자 추가 ---
	FSummonSkillData() : Montage(nullptr), MinionClassToSummon(nullptr), MinionCount(0) {}
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> Montage;

	// 소환할 병사.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<ACharacter> MinionClassToSummon;

	// 병사 수.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MinionCount;
};

// 전체 몬스터 데이터를 담는 메인 구조체 (FTableRowBase 상속 필수).
// USTRUCT 매크로는 이 구조체를 언리얼 엔진에 노출시키는 역할.
// BlueprintType으로 지정하면 블루프린트에서도 이 구조체를 사용.
USTRUCT(BlueprintType)
struct FMonsterData : public FTableRowBase // FTableRowBase를 상속받아야 데이터 테이블의 행으로 사용 가능.
{
	// GENERATED_BODY() 매크로는 엔진이 필요한 코드를 자동으로 생성하기 위해 필수.
	GENERATED_BODY()

public:
	// UPROPERTY 매크로는 이 변수를 언리얼 에디터에 노출시켜 편집.
	// EditAnywhere: 에디터 어디서든 값을 수정.
	// BlueprintReadWrite: 블루프린트에서 값을 읽고 쓸 수 있다.
    
	// --- 기본 스탯 ---
	// 스탯 데이터를 변수로 포함
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stat)
	FMonsterStatData StatData;

	// --- 애니메이션 ---
	// 근접 공격.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	TArray<FMeleeAttackData> MeleeAttacks;

	// 원거리 공격.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	TArray<FRangedAttackData> RangedAttacks;

	// 소환 스킬
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	FSummonSkillData SummonSkill;

	// 죽음 몽타주 애셋.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	TObjectPtr<class UAnimMontage> DeadMontage;

	// --- 애님 블루프린트 ---
	// TSubclassOf<>는 특정 클래스(의 자식 클래스) 자체를 참조할 때 사용 (예: ABP_Bear 블루프린트 클래스)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TSubclassOf<class UAnimInstance> AnimBlueprint;
    
	// --- AI ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AI)
	TObjectPtr<class UBehaviorTree> MainBehaviorTree; // 기존 BehaviorTree 변수 이름을 변경

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AI)
	TObjectPtr<UBehaviorTree> CombatBehaviorTree; // 전투 로직 서브 트리
};