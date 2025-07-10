// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_BossLRMove.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

UBTTask_BossLRMove::UBTTask_BossLRMove()
{
	// 비헤이비어 트리 에디터에 표시될 노드 이름.
	NodeName = TEXT("Boss LR Move Location");
    
	// 기본값 설정.
	OrbitDistance = 1000.0f;
	StrafeDistance = 50.0f;
}

EBTNodeResult::Type UBTTask_BossLRMove::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// 비헤이비어트리를 소유하는 컨트롤러가 제어하는 폰 정보 가져오기.
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControllingPawn)
	{
		return EBTNodeResult::Failed;
	}
	
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Blackboard)
	{
		return EBTNodeResult::Failed;
	}
	
	// 블랙보드에서 타겟 액터(플레이어)를 가져온다.
	AActor* TargetActor = Cast<AActor>(Blackboard->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!TargetActor)
	{
		return EBTNodeResult::Failed;
	}
	
	// --- 목표 위치 계산 시작 ---
	const FVector MyLocation = ControllingPawn->GetActorLocation();
	const FVector TargetLocation = TargetActor->GetActorLocation();

	// 1. 플레이어로부터 나를 향하는 방향 벡터를 구합니다. (거리 유지를 위해)
	const FVector DirectionFromTarget = (MyLocation - TargetLocation).GetSafeNormal();

	// 2. 플레이어를 중심으로 하는 원 위의 내 이상적인 위치를 계산합니다.
	const FVector IdealOrbitLocation = TargetLocation + DirectionFromTarget * OrbitDistance;
    
	// 3. 좌/우 중 이동할 방향을 랜덤으로 결정합니다.
	const float RandomSign = FMath::RandBool() ? 1.0f : -1.0f;

	// 4. 이상적인 위치에서 좌/우로 이동할 변위 벡터를 계산합니다.
	const FVector StrafeVector = ControllingPawn->GetActorRightVector() * RandomSign * StrafeDistance;

	// 5. 최종 목표 지점을 계산합니다.
	const FVector FinalDestination = IdealOrbitLocation + StrafeVector;

	// 6. 계산된 위치가 실제로 이동 가능한 곳인지 내비게이션 시스템에 확인합니다.
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (NavSystem)
	{
		FNavLocation NavLocation;
		// FinalDestination 주변 반경 100 유닛 내에서 이동 가능한 지점을 찾습니다.
		if (NavSystem->GetRandomReachablePointInRadius(FinalDestination, 100.0f, NavLocation))
		{
			// 7. 찾은 위치를 블랙보드의 DestinationKey에 저장합니다.
			Blackboard->SetValueAsVector(DestinationKey.SelectedKeyName, NavLocation.Location);
			// 태스크 성공! 이제 Move To가 이 위치로 이동할 것입니다.
			return EBTNodeResult::Succeeded;
		}
	}
	
	// 이동 가능한 위치를 찾지 못했으므로 태스크 실패
	return EBTNodeResult::Failed;
}
