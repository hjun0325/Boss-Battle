// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "WTAIController.generated.h"

/**
 * 
 */
UCLASS()
class WANTEDPROJECT_API AWTAIController : public AAIController
{
	GENERATED_BODY()

public:
	AWTAIController();

	// 외부에서 실행할 BehaviorTree 애셋을 받아오기.
	void RunAI(class UBehaviorTree* InBehaviorTree);
	void StopAI();

protected:
	// 컨트롤러가 폰에 빙의했을 때 실행되는 이벤트 함수.
	virtual void OnPossess(APawn* InPawn) override;
};
