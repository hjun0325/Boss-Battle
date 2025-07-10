// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_AttackRange.generated.h"

/**
 * 
 */
UCLASS()
class WANTEDPROJECT_API UBTDecorator_AttackRange : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_AttackRange();

protected:
	// 데코레이터 실행에 필요한 함수.
	virtual bool CalculateRawConditionValue(
		UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
