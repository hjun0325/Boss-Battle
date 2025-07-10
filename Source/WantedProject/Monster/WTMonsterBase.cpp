// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/WTMonsterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MonsterStat/WTMonsterStatComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "UI/WTHpBarWidget.h"
#include "UI/WTWidgetComponent.h"
#include "AI/WTAIController.h"
#include "WTMonsterData.h"
#include "Kismet/KismetSystemLibrary.h" // SphereOverlapActors를 위해 포함
#include "Kismet/GameplayStatics.h"      // ApplyDamage를 위해 포함
#include "DrawDebugHelpers.h"
#include "NavigationSystem.h"

// Sets default values
AWTMonsterBase::AWTMonsterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// 태그 설정.
	Tags.Add(FName("Monster"));

	// AIController 클래스 설정.
	AIControllerClass = AWTAIController::StaticClass();
	
	// AI 적용 대상 설정. (빙의 모드 설정)
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
	// 컴포넌트 설정.
	GetCapsuleComponent()->SetCapsuleHalfHeight(88.0f);
	GetCapsuleComponent()->SetCapsuleRadius(100.0f);
	GetCapsuleComponent()->SetCollisionObjectType(ECC_GameTraceChannel2); // WTMonster.
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("WTMonster"));

	// 메시의 콜리전은 NoCollision 설정 (주로 랙돌에 사용됨).
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));
	GetMesh()->SetRelativeLocationAndRotation(
		FVector(0.0f, 0.0f, -88.0f),
		FRotator(0.0f, -90.0f, 0.0f));

	// Stat Component.
	StatComponent = CreateDefaultSubobject<UWTMonsterStatComponent>(TEXT("StatComponent"));
	
	// Widget Component.
	HpBar = CreateDefaultSubobject<UWTWidgetComponent>(TEXT("Widget"));
	// 컴포넌트 계층 설정 및 상대 위치 설정 (머리 위로 보일 수 있게).
	HpBar->SetupAttachment(GetMesh());
	HpBar->SetRelativeLocation(FVector(0.0f, 0.0f, 200.0f));

	// 사용할 위젯 클래스 정보 설정.
	static ConstructorHelpers::FClassFinder<UUserWidget> HpBarWidgetRef(
		TEXT("/Game/WantedProject/UI/WBP_HpBar.WBP_HpBar_C"));
	if (HpBarWidgetRef.Class)
	{
		// 위젯 컴포넌트는 위젯의 클래스 정보를 바탕으로 자체적으로 인스턴스를 생성함.
		HpBar->SetWidgetClass(HpBarWidgetRef.Class);

		// 2D 모드로 그리기.
		HpBar->SetWidgetSpace(EWidgetSpace::Screen);

		// 크기 설정.
		HpBar->SetDrawSize(FVector2D(150.0f,15.0f));

		// 콜리전 끄기.
		HpBar->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AWTMonsterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	StatComponent->OnHpZero.AddUObject(this, &AWTMonsterBase::SetDead);
}

float AWTMonsterBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
                                 class AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	StatComponent->ApplyDamage(DamageAmount);
	
	return DamageAmount;
}

void AWTMonsterBase::SetupCharacterWidget(class UUserWidget* InUserWidget)
{
	// 필요한 위젯 정보 가져오기.
	UWTHpBarWidget* HpBarWidget = Cast<UWTHpBarWidget>(InUserWidget);
	if (HpBarWidget)
	{
		// 최대 체력 값 설정.
		HpBarWidget->SetMaxHp(StatComponent->GetMaxHp());
		
		// HP 퍼센트가 제대로 계산 되도록 현재 체력 설정.
		HpBarWidget->UpdateHpBar(StatComponent->GetCurrentHp());

		// 체력 변경 이벤트(델리게이트)에 함수 및 객체 정보 등록.
		StatComponent->OnHpChanged.AddUObject(HpBarWidget, &UWTHpBarWidget::UpdateHpBar);
	}
}

void AWTMonsterBase::SetDead()
{
	// 무브먼트 컴포넌트 끄기.
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	
	// 콜리전 끄기.
	SetActorEnableCollision(false);
	
	// 죽는 애니메이션 재생.
	PlayDeadAnimation();

	// 죽었을 때 HpBar(위젯) 사라지도록 처리.
	HpBar->SetHiddenInGame(true);
}

void AWTMonsterBase::PlayDeadAnimation()
{
	// 몽타주 재생.
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		// 이미 재생 중인 몽타주가 있다면, 모두 종료.
		AnimInstance->StopAllMontages(0.0f);

		// 죽음 몽타주 재생.
		const float PlayRate = 1.0f;
		AnimInstance->Montage_Play(CurrentMonsterData.DeadMontage, PlayRate);   
	}
}

float AWTMonsterBase::GetAIPatrolRadius()
{
	return 800.0f;
}

float AWTMonsterBase::GetAIDetectRange()
{
	return 1500.0f;
}

float AWTMonsterBase::GetAIAttackRange()
{
	return 200.0f;
}

float AWTMonsterBase::GetAITurnSpeed()
{
	return 2.0f;
}

void AWTMonsterBase::MeleeAttack(int32 AttackIndex)
{
	if (!CurrentMonsterData.MeleeAttacks.IsValidIndex(AttackIndex)) return;

	// 이동 비활성화.
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	// 현재 근접공격 인덱스 저장.
	CurrentMeleeAttackIndex = AttackIndex;

	// 새로운 공격이 시작될 때마다, 이전에 맞았던 액터 목록을 초기화.
	HitActorsInSwing.Empty();

	// 인덱스 값의 공격 종류 설정.
	const FMeleeAttackData& AttackData = CurrentMonsterData.MeleeAttacks[AttackIndex];

	// 몽타주 재생.
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		const float AttackSpeedRate = 1.0f;
		AnimInstance->Montage_Play(AttackData.Montage, AttackSpeedRate);
	}
	
	// 몽타주 재생이 시작되면, 재생이 종료될 때 호출되는 델리게이트에 등록.
	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &AWTMonsterBase::AttackActionEnd);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, AttackData.Montage);
}

void AWTMonsterBase::RangedAttack(int32 AttackIndex)
{
	// 이동 비활성화.
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	
	if (!CurrentMonsterData.RangedAttacks.IsValidIndex(AttackIndex)) return;

	const FRangedAttackData& AttackData = CurrentMonsterData.RangedAttacks[AttackIndex];

	// 몽타주 재생.
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		const float AttackSpeedRate = 1.0f;
		AnimInstance->Montage_Play(AttackData.Montage, AttackSpeedRate);
	}

	// 몽타주 재생이 시작되면, 재생이 종료될 때 호출되는 델리게이트에 등록.
	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &AWTMonsterBase::AttackActionEnd);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, AttackData.Montage);
}

void AWTMonsterBase::SummonSkill()
{
	// 이동 비활성화.
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	
	const FSummonSkillData& SummonData = CurrentMonsterData.SummonSkill;

	// 소환할 병사가 유효한지 체크.
	if (!SummonData.MinionClassToSummon)
	{
		return;
	}

	// 몽타주 재생.
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		const float AttackSpeedRate = 1.0f;
		AnimInstance->Montage_Play(SummonData.Montage, AttackSpeedRate);
	}

	// 몽타주 재생이 시작되면, 재생이 종료될 때 호출되는 델리게이트에 등록.
	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &AWTMonsterBase::AttackActionEnd);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, SummonData.Montage);
}

void AWTMonsterBase::TeleportToTargetRear()
{
	AAIController* AIController = Cast<AAIController>(GetController());
	UBlackboardComponent* Blackboard = AIController ? AIController->GetBlackboardComponent() : nullptr;
	if (!Blackboard) return;
	
	AActor* TargetActor = Cast<AActor>(Blackboard->GetValueAsObject(TEXT("Target")));
	if (!TargetActor) return;
	
	// 사라지는 이펙트를 현재 위치에 재생.
	if (TeleportOutVFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TeleportOutVFX, GetActorLocation());
	}
	
	// 플레이어의 등 뒤, 약 2미터 떨어진 위치를 목표 지점으로 계산.
	const float DistanceBehind = 500.0f;
	FVector Destination = TargetActor->GetActorLocation() - (TargetActor->GetActorForwardVector() * DistanceBehind);

	UE_LOG(LogTemp, Warning, TEXT("%s"),*Destination.ToString());
	// [디버깅 코드 추가] 계산된 목표 지점을 5초간 파란색 구체로 그립니다.
	DrawDebugSphere(GetWorld(), Destination, 50.f, 12, FColor::Blue, false, 5.0f, 0, 2.0f);

	// 계산된 위치가 실제로 이동 가능한 곳인지 내비게이션 시스템에 확인합니다.
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	FNavLocation NavLocation;
	bool bIsLocationFound = NavSystem->GetRandomReachablePointInRadius(Destination, 200.0f, NavLocation);
	if (bIsLocationFound)
	{
		UE_LOG(LogTemp, Warning, TEXT("e"));
		// 4. 유효한 위치로 액터를 즉시 이동(Teleport)시킵니다.
		TeleportTo(NavLocation.Location, GetActorRotation());

		// 5. 나타나는 이펙트를 새로운 위치에 재생합니다.
		if (TeleportInVFX)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TeleportInVFX, NavLocation.Location);
		}
	}
}

void AWTMonsterBase::StartSprint()
{
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	if (MoveComp)
	{
		// 원래 속도를 백업해둡니다.
		OriginalWalkSpeed = MoveComp->MaxWalkSpeed;
		// 속도를 일시적으로 3배 증가시킵니다. (값은 조절 가능)
		MoveComp->MaxWalkSpeed *= 3.0f;
	}
}

void AWTMonsterBase::StopSprint()
{
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	if (MoveComp)
	{
		// 속도를 원래대로 복구합니다.
		MoveComp->MaxWalkSpeed = OriginalWalkSpeed;
	}
}

void AWTMonsterBase::SetAIAttackDelegate(const FAICharacterAttackFinished& InOnAttackFinished)
{
	OnAttackFinished = InOnAttackFinished;
}

void AWTMonsterBase::AttackHitCheck()
{
	UE_LOG(LogTemp, Log, TEXT("1"));
	
	// 현재 실행 중인 공격의 데이터.
	if (!CurrentMonsterData.MeleeAttacks.IsValidIndex(CurrentMeleeAttackIndex)) return;
	const FMeleeAttackData& CurrentAttackData = CurrentMonsterData.MeleeAttacks[CurrentMeleeAttackIndex];

	// 최종 데미지 및 공격 위치 계산
	const float FinalDamage = CurrentMonsterData.StatData.AttackDamage * CurrentAttackData.DamageMultiplier;
	
	// 오버랩으로 찾아낼 액터의 타입을 지정합니다. (플레이어와 같은 Pawn 타입만)
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	// 자기 자신을 공격하는 것을 방지하기 위해, 무시할 액터 목록에 자신을 추가합니다.
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	
	// 오버랩된 액터들을 담을 빈 배열을 준비합니다.
	TArray<AActor*> OverlappedActors;

	// 4. 스피어 오버랩(Sphere Overlap) 실행1
	// 지정된 위치와 반경 안에 있는 모든 폰(Pawn)을 찾아 OverlappedActors 배열에 담습니다.
	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		GetActorLocation(),
		CurrentAttackData.AttackRange, // 최대 사거리를 반경으로 사용
		ObjectTypes,
		nullptr,
		ActorsToIgnore,
		OverlappedActors
	);
	
	DrawDebugAttackSector(GetActorLocation(), CurrentAttackData.AttackRange, CurrentAttackData.AttackAngle, FColor::Red);

	const FVector MyLocation = GetActorLocation();
	const FVector MyForwardVector = GetActorForwardVector();
	
	// 6. 오버랩된 모든 액터에게 데미지 적용
	for (AActor* OverlappedActor : OverlappedActors)
	{
		// 몬스터로부터 타겟을 향하는 방향 벡터를 구합니다.
		const FVector DirectionToTarget = (OverlappedActor->GetActorLocation() - MyLocation).GetSafeNormal();

		// 3. 몬스터의 정면 방향과 타겟 방향 사이의 각도를 계산합니다.
		// FVector::DotProduct는 두 벡터 사이 각도의 코사인 값을 반환합니다.
		const float DotProduct = FVector::DotProduct(MyForwardVector, DirectionToTarget);
		// 데이터 테이블에 설정된 부채꼴의 절반 각도에 해당하는 코사인 임계값을 구합니다.
		const float AngleThreshold = FMath::Cos(FMath::DegreesToRadians(CurrentAttackData.AttackAngle / 2.0f));

		
		// 4. 만약 두 벡터 사이 각도의 코사인 값이 임계값보다 크거나 같다면,
		//    이것은 타겟이 부채꼴 각도 내에 있다는 의미입니다.
		if (DotProduct >= AngleThreshold)
		{
			// 5. 부채꼴 범위 안에 있는 타겟 중, 아직 피해를 입지 않은 대상에게만 데미지를 적용합니다.
			if (!HitActorsInSwing.Contains(OverlappedActor))
			{
				UGameplayStatics::ApplyDamage(OverlappedActor, FinalDamage, GetController(), this, nullptr);
				HitActorsInSwing.Add(OverlappedActor);
				
				// [추가] 실제로 맞은 대상은 초록색 선으로 연결하여 명확하게 표시
				DrawDebugLine(GetWorld(), GetActorLocation(), OverlappedActor->GetActorLocation(), FColor::Green, false, 2.0f, 0, 2.0f);
			}
		}
	}
}

void AWTMonsterBase::FireShadowBolt()
{
}

void AWTMonsterBase::DrawDebugAttackSector(const FVector& Center, float Radius, float Angle, const FColor& Color)
{
	const int32 NumSegments = 20; // 부채꼴의 호를 몇 개의 선으로 그릴지 결정 (많을수록 부드러움)
	const float AngleIncrement = FMath::DegreesToRadians(Angle / NumSegments);
	const FVector Forward = GetActorForwardVector();

	// 부채꼴의 시작 방향 벡터 계산 (몬스터 정면에서 -Angle/2 만큼 회전)
	FVector StartDirection = Forward.RotateAngleAxis(-Angle / 2.0f, FVector::UpVector);

	FVector PrevPoint = Center + StartDirection * Radius;

	// 루프를 돌면서 짧은 선들을 이어 붙여 호(arc)를 그립니다.
	for (int32 i = 1; i <= NumSegments; ++i)
	{
		FVector CurrentDirection = StartDirection.RotateAngleAxis(FMath::RadiansToDegrees(AngleIncrement * i), FVector::UpVector);
		FVector CurrentPoint = Center + CurrentDirection * Radius;
		DrawDebugLine(GetWorld(), PrevPoint, CurrentPoint, Color, false, 2.0f, 0, 1.0f);
		PrevPoint = CurrentPoint;
	}

	// 부채꼴의 양쪽 끝을 중심으로 연결하는 선을 그립니다.
	DrawDebugLine(GetWorld(), Center, Center + StartDirection * Radius, Color, false, 2.0f, 0, 1.0f);
	DrawDebugLine(GetWorld(), Center, PrevPoint, Color, false, 2.0f, 0, 1.0f);
}


void AWTMonsterBase::AttackActionEnd(class UAnimMontage* TargetMontage, bool IsProperlyEnded)
{
	// 캐릭터 무브먼트 컴포넌트 모드 복구.
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	// 공격이 끝나면 NotifyComboActionEnd 함수 호출.
	NotifyComboActionEnd();
}

void AWTMonsterBase::NotifyComboActionEnd()
{
	// 전달 받은 델리게이트 실행.
	OnAttackFinished.ExecuteIfBound();
}

void AWTMonsterBase::BeginPlay()
{
	Super::BeginPlay();

	if (!MonsterDataTable || MonsterID.IsNone()) return;
	
	const FMonsterData* FoundData = MonsterDataTable->FindRow<FMonsterData>(MonsterID, TEXT(""));
	if (!FoundData) return;
	CurrentMonsterData = *FoundData;

	// 1. 스탯 컴포넌트 초기화 및 죽음 이벤트 구독
	StatComponent->InitializeStat(CurrentMonsterData.StatData);
	StatComponent->OnHpZero.AddUObject(this, &AWTMonsterBase::SetDead);

	// 2. 애님 블루프린트 설정
	GetMesh()->SetAnimInstanceClass(CurrentMonsterData.AnimBlueprint);
	
	// 3. AI 비헤이비어 트리 실행
	// 현재 폰에 빙의된 컨트롤러를 우리가 만든 AWTAIController 타입으로 가져온다.
	AWTAIController* AIController = Cast<AWTAIController>(GetController());
	if (AIController)
	{
		// 데이터 테이블에서 읽어온 BehaviorTree 애셋이 유효한지 확인.
		if (CurrentMonsterData.MainBehaviorTree)
		{
			// 컨트롤러의 RunAI 함수에 해당 비헤이비어 트리를 전달하여 실행을 명령.
			AIController->RunAI(CurrentMonsterData.MainBehaviorTree);
		}
		
		UBlackboardComponent* Blackboard = AIController->GetBlackboardComponent();
		if (Blackboard)
		{
			// [핵심 로직] "CombatBTAsset"이라는 키에 전투 서브 트리 애셋 정보를 저장
			Blackboard->SetValueAsObject(TEXT("CombatBTAsset"), CurrentMonsterData.CombatBehaviorTree);
		}
	}
}
