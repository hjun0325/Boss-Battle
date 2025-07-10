// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Sprint.generated.h"

/**
 * 
 */
UCLASS()
class WANTEDPROJECT_API UBTTask_Sprint : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_Sprint();

protected:
	virtual EBTNodeResult::Type ExecuteTask(
		UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	
	// 에디터에서 스프린트를 켤지 끌지 지정할 변수
	UPROPERTY(EditAnywhere, Category = "Sprint")
	bool bShouldSprint;
};
