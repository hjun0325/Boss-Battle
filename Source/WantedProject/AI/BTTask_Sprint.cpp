// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_Sprint.h"
#include "AIController.h"
#include "Interface/WTMonsterAIInterface.h"

UBTTask_Sprint::UBTTask_Sprint()
{
}

EBTNodeResult::Type UBTTask_Sprint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// AIController가 제어하는 폰.
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControllingPawn)
	{
		return EBTNodeResult::Failed;
	}
	
	// 인터페이스로 형변환.
	IWTMonsterAIInterface* AIPawn = Cast<IWTMonsterAIInterface>(ControllingPawn);
	if (AIPawn)
	{
		if (bShouldSprint)
		{
			AIPawn->StartSprint();
		}
		else
		{
			AIPawn->StopSprint();
		}
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
