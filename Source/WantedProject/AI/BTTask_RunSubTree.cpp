// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_RunSubTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "AIController.h"

UBTTask_RunSubTree::UBTTask_RunSubTree()
{
	NodeName = TEXT("Run Sub-Tree From Blackboard");
}

EBTNodeResult::Type UBTTask_RunSubTree::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Blackboard)
	{
		return EBTNodeResult::Failed;
	}

	// 블랙보드 키에서 UObject를 가져온다.
	UObject* AssetObject = Blackboard->GetValueAsObject(BehaviorTreeAssetKey.SelectedKeyName);
	if (!AssetObject)
	{
		UE_LOG(LogTemp, Warning, TEXT("BTTask_RunSubTreeFromBB: BehaviorTreeAssetKey is null!"));
		return EBTNodeResult::Failed;
	}

	// 가져온 UObject를 UBehaviorTree로 형변환.
	UBehaviorTree* SubTreeAsset = Cast<UBehaviorTree>(AssetObject);
	if (!SubTreeAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("BTTask_RunSubTreeFromBB: Object in key is not a BehaviorTree!"));
		return EBTNodeResult::Failed;
	}

	// 3. 현재 비헤이비어 트리 컴포넌트가 이 서브 트리를 실행하도록 합니다.
	// 이 함수를 호출하면, 현재 트리의 실행이 중단되고 새로운 서브 트리가 시작됩니다.
	// 서브 트리가 끝나면 원래 트리로 돌아오게 하려면 더 복잡한 로직이 필요하지만,
	// 우리의 '상태' 기반 설계에서는 이 방식으로 충분합니다.
	OwnerComp.StartTree(*SubTreeAsset, EBTExecutionMode::Looped);
    
	// StartTree가 즉시 실행 흐름을 바꾸므로, 여기서는 Succeeded를 반환하여
	// 현재 태스크가 성공적으로 '전환' 명령을 내렸음을 알립니다.
	return EBTNodeResult::Succeeded;
}
