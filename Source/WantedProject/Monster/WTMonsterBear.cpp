// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/WTMonsterBear.h"
#include "AI/WTAIController.h"
#include "BrainComponent.h"
#include "UI/WTWidgetComponent.h"
#include "MonsterStat/WTMonsterStatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AWTMonsterBear::AWTMonsterBear()
{
	// 스켈레탈 메시 설정.
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> BearMeshRef(
		TEXT("/Game/InfinityBladeAdversaries/Enemy/Enemy_Bear/Enemy_Bear.Enemy_Bear"));
	if (BearMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(BearMeshRef.Object);
	}
}

void AWTMonsterBear::OnAcquiredFromPool_Implementation()
{
	// 액터를 보이게 하고, 충돌 및 틱을 활성화
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);
	
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	// [수정] 비활성화됐던 위젯을 다시 보이게 합니다.
	if (HpBar)
	{
		HpBar->SetHiddenInGame(false);
	}

	// 스탯 컴포넌트의 HP를 최대치로 리셋
	if (StatComponent)
	{
		StatComponent->InitializeStat(CurrentMonsterData.StatData);
	}

	// AI를 다시 동작시킴
	AWTAIController* AIController = Cast<AWTAIController>(GetController());
	if (AIController)
	{
		// BrainComponent가 없을 수도 있으므로, RunAI를 다시 호출하는 것이 더 안전합니다.
		AIController->RunAI(CurrentMonsterData.MainBehaviorTree);
	}
}

void AWTMonsterBear::OnReturnedToPool_Implementation()
{
	// AI 명령 전달 중지.
	AWTAIController* WTAIController = Cast<AWTAIController>(GetController());
	if (WTAIController)
	{
		WTAIController->StopAI();
	}

	// 액터를 숨기고, 충돌 및 틱을 비활성화
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);
}

void AWTMonsterBear::SetDead()
{
	Super::SetDead();
	
	OnReturnedToPool_Implementation();
}
