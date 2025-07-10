// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_Detect.h"
#include "AIController.h"
#include "Interface/WTMonsterAIInterface.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ABAI.h"

UBTService_Detect::UBTService_Detect()
{
	// 에디터에서 보이는 이름 설정.
	NodeName = TEXT("Detect");

	// Tick이 실행되는 주기 설정.
	Interval = 1.0f;
}

void UBTService_Detect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// AIController가 제어하는 폰.
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	
	// 제어하는 폰이 없으면 함수 종료.
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

	// 월드 정보.
	UWorld* World = ControllingPawn->GetWorld();

	// 감지 시작 위치.
	FVector Center = ControllingPawn->GetActorLocation();
	
	// 충돌 감지 결과 저장 변수.
	TArray<FOverlapResult> OverlapResults;

	// 감지 범위 설정.
	float DetectRadius = AIPawn->GetAIDetectRange();

	// 충돌 제외 목록.
	FCollisionQueryParams CollisionQueryParams(
	   SCENE_QUERY_STAT(Detect),
	   false,
	   ControllingPawn
	);

	// 감지 처리.
	bool bResult = World->OverlapMultiByChannel(
		OverlapResults,
		Center,
		FQuat::Identity,
		ECC_GameTraceChannel4,
		FCollisionShape::MakeSphere(DetectRadius),
		CollisionQueryParams);

	//UE_LOG(LogTemp, Warning, TEXT("%d"), OverlapResults.Num());
	
	// 충돌이 감지되면.
	if (bResult)
	{
		// 감지된 플레이어에 대해 처리 진행.
		for (auto const& OverlapResult : OverlapResults)
		{
			// 감지된 액터를 폰으로 형변환.
			APawn* Pawn = Cast<APawn>(OverlapResult.GetActor());

			// 플레이어인지 확인.
			if (Pawn && Pawn->GetController()->IsPlayerController())
			{
				// 감지한 플레이어를 블랙보드에 저장.
				OwnerComp.GetBlackboardComponent()->SetValueAsObject(
					BBKEY_TARGET, Pawn);

				// 디버그 드로우를 활용해 감지 영역을 녹색으로 표시.
				DrawDebugSphere(
					World,
					Center,
					DetectRadius,
					16,
					FColor::Green,
					false,
					0.2f);

				// 감지된 지점을 녹색으로 표시.
				DrawDebugPoint(
					World,
					Pawn->GetActorLocation(),
					10.0f,
					FColor::Green,
					false,
					0.2f);

				// 감지된 방향을 녹색으로 표시.
				DrawDebugLine(
					World,
					ControllingPawn->GetActorLocation(),
					Pawn->GetActorLocation(),
					FColor::Green,
					false,
					0.2f);

				return;
			}
		}
	}

	// 플레이어 감지 실패.
	OwnerComp.GetBlackboardComponent()->SetValueAsObject(
		BBKEY_TARGET, nullptr);

	// 디버그 드로우를 활용해 감지 영역을 빨간색으로 표시.
	DrawDebugSphere(
		World,
		Center,
		DetectRadius,
		16,
		FColor::Red,
		false,
		0.2f);
}
