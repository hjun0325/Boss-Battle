// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_FindPatrolPos.h"
#include "ABAI.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Interface/WTMonsterAIInterface.h"

UBTTask_FindPatrolPos::UBTTask_FindPatrolPos()
{
}

EBTNodeResult::Type UBTTask_FindPatrolPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	// 내비게이션 시스템을 사용해 랜덤으로 이동 가능한 위치를 구한 뒤에
	// 블랙보드에 정찰 위치로 저장.

	// 비헤이비어트리를 소유하는 컨트롤러가 제어하는 폰 정보 가져오기.
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();

	// 유효한지 검사.
	if (!ControllingPawn)
	{
		return EBTNodeResult::Failed;
	}

	// 내비게이션 시스템을 사용하기 위한 포인터 가져오기.
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(
		ControllingPawn->GetWorld());
	
	// 유효한지 검사.
	if (!NavSystem)
	{
		return EBTNodeResult::Failed;
	}

	// 인터페이스로 형변환.
	IWTMonsterAIInterface* AIPawn = Cast<IWTMonsterAIInterface>(ControllingPawn);

	// 변환에 실패하면 실패 반환.
	if (!AIPawn)
	{
		return EBTNodeResult::Failed;
	}

	// 폰이 생성된 초기 위치 가져오기.
	FVector Origin = OwnerComp.GetBlackboardComponent()->GetValueAsVector(
		BBKEY_HOMEPOS);

	// AIPawn으로부터 정찰 반경 받아오기.
	float PatrolRadius = AIPawn->GetAIPatrolRadius();
	
	// 결과 저장을 위한 변수.
	FNavLocation NextPatrolPos;
	if (NavSystem->GetRandomPointInNavigableRadius(Origin, PatrolRadius, NextPatrolPos))
	{
		// 랜덤 위치가 잘 설정되면, 블랙보드에 저장.
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(
			BBKEY_PATROLPOS, NextPatrolPos.Location);

		return EBTNodeResult::Succeeded;
	}
	
	return EBTNodeResult::Failed;
}
