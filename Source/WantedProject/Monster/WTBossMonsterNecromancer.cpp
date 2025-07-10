// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/WTBossMonsterNecromancer.h"
#include "MonsterStat/WTMonsterStatComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/WTAIController.h"
#include "Item/WTShadowBolt.h"
#include "Manager/ObjectPoolManager.h"
#include "Kismet/GameplayStatics.h"
#include "UI/WTHUDWidget.h"

AWTBossMonsterNecromancer::AWTBossMonsterNecromancer()
{
	// 스켈레탈 메시 설정.
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> NecromancerMeshRef(
		TEXT("/Game/ParagonSevarog/Characters/Heroes/Sevarog/Meshes/Sevarog.Sevarog"));
	if (NecromancerMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(NecromancerMeshRef.Object);
	}

	// 마법 구체 액터 설정.
	static ConstructorHelpers::FClassFinder<AWTShadowBolt> BoltClassRef(
		TEXT("/Game/WantedProject/BluePrint/BP_WTShadowBolt.BP_WTShadowBolt_C"));
	if (BoltClassRef.Class)
	{
		BoltClass = BoltClassRef.Class;
	}

	// 태그 설정.
	Tags.Add(FName("Boss"));

	// 기본값 설정.
	CurrentPhase = EBossPhase::Phase1_Ranged;
	PhaseChangeHealthPercentage = 0.5f;
}

void AWTBossMonsterNecromancer::BeginPlay()
{
	Super::BeginPlay();

	// --- 오브젝트 풀 매니저 찾기 ---
	// 현재 월드에 있는 AObjectPoolManager 클래스의 첫 번째 액터를 찾아옵니다.
	// 레벨에 단 하나의 매니저만 배치했으므로 이 방법이 유효합니다.
	PoolManager = Cast<AObjectPoolManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AObjectPoolManager::StaticClass()));

	// 만약을 대비한 안전장치. 만약 레벨에 ObjectPoolManager가 배치되지 않았다면 로그를 남깁니다.
	if (!PoolManager)
	{
		UE_LOG(LogTemp, Error, TEXT("%s cannot find ObjectPoolManager in the world!"), *GetName());
	}
}

// 데미지 받았을 때 호출되는 함수.
float AWTBossMonsterNecromancer::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// 1페이즈인 경우.
	if (CurrentPhase == EBossPhase::Phase1_Ranged)
	{
		if (StatComponent)
		{
			// 현재 체력 비율 계산.
			float CurrentHpRatio = StatComponent->GetCurrentHp() / StatComponent->GetMaxHp();

			// 체력 비율이 설정 값보다 낮아지면 2페이즈로 전환.
			if (CurrentHpRatio < PhaseChangeHealthPercentage)
			{
				EnterPhase2();
			}
		}
	}
	
	return DamageAmount;
}

void AWTBossMonsterNecromancer::SummonSkill()
{
	Super::SummonSkill();

	if (!PoolManager)
	{
		UE_LOG(LogTemp, Error, TEXT("CRITICAL ERROR: PoolManager is NULL in PerformSummon! Cannot perform summon."));
		return;
	}
	
	const FSummonSkillData& SummonData = CurrentMonsterData.SummonSkill;
	
	// 추후 AnimNotify를 통해 몽타주의 특정 시점에 아래 로직을 호출.
	for (int32 i = 0; i < SummonData.MinionCount; ++i)
	{
		// 오브젝트 풀 매니저에게 미니언 클래스를 전달하며 '재고'를 요청.
		AActor* MinionActor = PoolManager->AcquirePooledObject(SummonData.MinionClassToSummon);
        
		if (MinionActor)
		{
			// 원하는 위치에 미니언을 배치. (활성화는 미니언이 스스로 함)
			FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 200.0f + GetActorRightVector() * (i * 200.0f - 200.0f);
			MinionActor->SetActorLocation(SpawnLocation);
		}
	}
}

void AWTBossMonsterNecromancer::FireShadowBolt()
{
	// 화살이 발사될 소켓에서 위치를 얻음.
	FVector SocketLocation = GetMesh()->GetSocketLocation("BoltSocket");

	// 발사 위치 설정.
	FVector SpawnLocation = SocketLocation;

	// 캐릭터의 현재 회전값을 발사체 회전값으로 사용.
	FRotator SpawnRotation = GetActorRotation();
	
	// 액터 스폰 파라미터 설정.
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnParams.Owner = this;           // 소유자 설정 (누가 발사했는지 식별)
	SpawnParams.Instigator = this;      // Instigator 설정 (피해 판정에 사용됨)
	
	// ArrowClass 타입의 화살 액터를 월드에 스폰.
	GetWorld()->SpawnActor<AWTShadowBolt>(BoltClass, SpawnLocation, SpawnRotation, SpawnParams);

	/*// 나이아가라 이펙트를 발사 위치에서 재생.
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),                 // 월드 참조
		NSArrowCast,               // 사용할 이펙트 시스템
		FVector(SocketLocation),   // 재생 위치
		FRotator(GetActorRotation()), // 회전 방향
		FVector(1.0f, 1.0f, 1.0f), // 스케일
		true                       // 자동 소멸 여부
	);*/
	
	/*// 화살 발사 사운드를 현재 캐릭터 위치에서 재생.
	UGameplayStatics::PlaySoundAtLocation(this, ArrowCastCue, GetActorLocation());*/

	// 공격 중 이동을 멈췄었다면, 다시 걷기 모드로 이동 재활성화.
	//GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

void AWTBossMonsterNecromancer::SetupHUDWidget(class UWTHUDWidget* InHUDWidget)
{
	if (InHUDWidget)
	{
		InHUDWidget->BossSetMaxHp(StatComponent->GetMaxHp());
		
		// Hp 정보 전달.
		InHUDWidget->UpdateBossHpBar(StatComponent->GetCurrentHp());
		
		// 델리게이트 등록.
		StatComponent->OnHpChanged.AddUObject(InHUDWidget, &UWTHUDWidget::UpdateBossHpBar);
	}
}

void AWTBossMonsterNecromancer::SetDead()
{
	Super::SetDead();

	// AI 명령 전달 중지.
	AWTAIController* WTAIController = Cast<AWTAIController>(GetController());
	if (WTAIController)
	{
		WTAIController->StopAI();
	}
}

// 2페이즈 진입 로직을 처리하는 함수.
void AWTBossMonsterNecromancer::EnterPhase2()
{
	CurrentPhase = EBossPhase::Phase2_Hybrid;

	// 포효 몽타주 및 이펙트 사운드  출력.
	// 시네마틱 구현. 전환 연출.

	// --- AI에게 상태 변화를 알림. ---
	AWTAIController* MyAIController = Cast<AWTAIController>(GetController());
	if (MyAIController)
	{
		// 컨트롤러로부터 블랙보드 컴포넌트를 가져온다.
		UBlackboardComponent* BlackboardComponent = MyAIController->GetBlackboardComponent();
		if (BlackboardComponent)
		{
			// 블랙보드 키 값(CurrentPhase)을 업데이트.
			BlackboardComponent->SetValueAsEnum(TEXT("CurrentPhase"), (uint8)CurrentPhase);
		}
	}
}
