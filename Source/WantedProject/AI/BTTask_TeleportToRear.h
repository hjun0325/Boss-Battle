// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_TeleportToRear.generated.h"

/**
 * 
 */
UCLASS()
class WANTEDPROJECT_API UBTTask_TeleportToRear : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_TeleportToRear();

protected:
	virtual EBTNodeResult::Type ExecuteTask(
		UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
};
