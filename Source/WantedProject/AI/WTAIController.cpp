// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/WTAIController.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ABAI.h"

AWTAIController::AWTAIController()
{
}

void AWTAIController::RunAI(UBehaviorTree* InBehaviorTree)
{
	// 전달받은 비헤이비어 트리가 유효한지, 그리고 그 안에 블랙보드가 연결되어 있는지 확인
	if (InBehaviorTree && InBehaviorTree->GetBlackboardAsset())
	{
		// 블랙보트 컴포넌트 포인터.
		UBlackboardComponent* BlackboardPtr = nullptr;

		// 사용할 블랙보드 지정.
		if (UseBlackboard(InBehaviorTree->GetBlackboardAsset(), BlackboardPtr))
		{
			// 시작할 때 NPC 위치를 블랙보드에 HomePos에 저장.
			Blackboard->SetValueAsVector(BBKEY_HOMEPOS, GetPawn()->GetActorLocation());

			// 1페이즈로 블랙보드에 저장.
			
			// 전달 받은 비헤이비어 트리를 실행.
			bool RunResult = RunBehaviorTree(InBehaviorTree);

			// 실행에 실패하면 확인.
			ensure(RunResult);
		}
	}
}

void AWTAIController::StopAI()
{
	// 중지를 위해 실행 중인 BehaviorTreeComponent 가져오기.
	UBehaviorTreeComponent* BTComponent = Cast<UBehaviorTreeComponent>(BrainComponent);

	// BT 중지.
	if (BTComponent)
	{
		BTComponent->StopTree();
	}
}

void AWTAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	// OnPossess 시점에 바로 AI를 실행하지 않는다.
	// AMonsterBase가 데이터 테이블을 읽고 준비가 되었을 때 RunAI를 호출해줄 것이기 때문.
}
