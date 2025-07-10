// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_UpdateAIState.generated.h"

/**
 * 
 */
UCLASS()
class WANTEDPROJECT_API UBTService_UpdateAIState : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_UpdateAIState();

protected:
	// 서비스가 주기적으로 실행할 함수.
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	// 비헤이비어 트리 에디터에서 어떤 블랙보드 키를 업데이트할지 지정할 수 있도록 변수를 노출.
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector DistanceToTargetKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector ActiveMinionCountKey;
};
