// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_MeleeAttack.h"
#include "AIController.h"
#include "Interface/WTMonsterAIInterface.h"

UBTTask_MeleeAttack::UBTTask_MeleeAttack()
{
	NodeName = TEXT("Melee Attack");

	// 기본값 설정
	MinAttackIndex = 0;
	MaxAttackIndex = 0;
}

EBTNodeResult::Type UBTTask_MeleeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	// AIController가 제어하는 폰.
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControllingPawn)
	{
		return EBTNodeResult::Failed;
	}

	// 인터페이스로 형변환.
	IWTMonsterAIInterface* AIPawn = Cast<IWTMonsterAIInterface>(ControllingPawn);
	if (!AIPawn)
	{
		return EBTNodeResult::Failed;
	}

	// 캐릭터가 공격을 종료할 때 실행할 델리게이트 선언.
	FAICharacterAttackFinished OnAttackFinished;

	// 람다 함수를 사용해 델리게이트에 바인딩.
	OnAttackFinished.BindLambda(
		[&]()
		{
			// 캐릭터의 공격이 끝날 때 TASK를 끝나는 값으로 전달.
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		});

	// 틱 함수로 체크하는 것 보다 델리게이트를 사용하는 것이 더 효과적.
	// 캐릭터가 공격을 종료할 때 실행할 델리게이트 전달(설정).
	AIPawn->SetAIAttackDelegate(OnAttackFinished);

	// 값을 잘못 입력한 경우.
	if (MinAttackIndex>MaxAttackIndex)
	{
		return EBTNodeResult::Failed;
	}

	// Min과 Max 사이에서 랜덤한 인덱스를 설정.
	const int32 RandomAttackIndex = FMath::RandRange(MinAttackIndex, MaxAttackIndex);
	
	// 몬스터의 근거리 공격 함수를 호출
	AIPawn->MeleeAttack(RandomAttackIndex);

	// 공격 명령 실행 후 곧바로 결과를 알 수 없기 때문에
	// 진행중(InProgress) 값을 반환.
	return EBTNodeResult::InProgress;
}
