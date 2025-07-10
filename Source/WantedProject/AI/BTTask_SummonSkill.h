// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SummonSkill.generated.h"

/**
 * 
 */
UCLASS()
class WANTEDPROJECT_API UBTTask_SummonSkill : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_SummonSkill();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
