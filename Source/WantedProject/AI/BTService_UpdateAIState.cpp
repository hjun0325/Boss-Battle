// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_UpdateAIState.h"
#include "ABAI.h"
#include "AIController.h"
#include "Interface/WTMonsterAIInterface.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_UpdateAIState::UBTService_UpdateAIState()
{
	NodeName = TEXT("Update AI State");
	Interval = 0.2f; // 0.2초마다 실행
	RandomDeviation = 0.05f; // 실행 시간 약간의 랜덤성 부여
}

void UBTService_UpdateAIState::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	// AIController가 제어하는 폰.
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControllingPawn)
	{
		return;
	}
	
	// 인터페이스로 형변환.
	IWTMonsterAIInterface* AIPawn = Cast<IWTMonsterAIInterface>(ControllingPawn);
	if (!AIPawn)
	{
		return;
	}

	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Blackboard)
	{
		return;
	}
	
	// 타겟과의 거리 업데이트.
	AActor* TargetActor = Cast<AActor>(Blackboard->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (TargetActor)
	{
		float Distance = FVector::Dist(ControllingPawn->GetActorLocation(), TargetActor->GetActorLocation());
		Blackboard->SetValueAsFloat(DistanceToTargetKey.SelectedKeyName, Distance);
	}

}
