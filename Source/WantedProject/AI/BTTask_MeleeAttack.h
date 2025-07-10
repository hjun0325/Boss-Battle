// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MeleeAttack.generated.h"

/**
 * 
 */
UCLASS()
class WANTEDPROJECT_API UBTTask_MeleeAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_MeleeAttack();

	// 태스크 실행 함수.
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "Attack")
	int32 MinAttackIndex;
	
	UPROPERTY(EditAnywhere, Category = "Attack")
	int32 MaxAttackIndex;
};
