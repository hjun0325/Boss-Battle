// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_RunSubTree.generated.h"

/**
 * 
 */
UCLASS()
class WANTEDPROJECT_API UBTTask_RunSubTree : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_RunSubTree();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// 에디터에서 실행할 서브 트리 애셋이 담긴 블랙보드 키를 지정할 수 있도록 합니다.
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector BehaviorTreeAssetKey;
};
