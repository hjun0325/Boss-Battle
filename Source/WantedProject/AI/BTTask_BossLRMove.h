// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_BossLRMove.generated.h"

/**
 * 
 */
UCLASS()
class WANTEDPROJECT_API UBTTask_BossLRMove : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_BossLRMove();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// --- 에디터에서 설정할 파라미터들 ---

	// 타겟(플레이어) 정보를 읽어올 블랙보드 키
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;

	// 계산된 이동 목표 지점을 저장할 블랙보드 키
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector DestinationKey;

	// 플레이어와 유지할 이상적인 거리
	UPROPERTY(EditAnywhere, Category = "AI")
	float OrbitDistance;

	// 한 번에 좌/우로 이동할 거리
	UPROPERTY(EditAnywhere, Category = "AI")
	float StrafeDistance;
};
