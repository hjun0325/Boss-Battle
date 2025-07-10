// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/WTCharacterPlayer.h"

// =============================================================
// 카메라 관련 (Camera)
// =============================================================
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

// =============================================================
// 입력 관련 (Input)
// =============================================================
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "WTCharacterControlData.h" // 컨트롤 데이터도 입력의 일부로 간주
#include "WTPlayerDataAsset.h"

// =============================================================
// 이동 및 AI 유틸리티 관련 (Movement & AI Utility)
// =============================================================
#include "GameFramework/CharacterMovementComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h" // 마우스 이동 등에 사용

// =============================================================
// 공격 및 스킬 관련 (Attack & Skill)
// =============================================================
#include "Item/WTBasicArrow.h"
#include "Item/WTUltimateArrow.h"
#include "CharacterCoolDown/WTCooldownManagerComponent.h" // 쿨다운은 스킬 시스템의 일부

// =============================================================
// 스탯 및 데미지 관련 (Stat & Damage)
// =============================================================
#include "CharacterStat/WTCharacterStatComponent.h"
#include "Kismet/GameplayStatics.h" // ApplyDamage 등 범용적인 게임플레이 함수

// =============================================================
// UI 및 이펙트 관련 (UI & VFX)
// =============================================================
#include "UI/WTHUDWidget.h"
#include "UI/WTHpBarWidget.h"
#include "UI/WTWidgetComponent.h"
#include "NiagaraFunctionLibrary.h"

// Trace Channel.
#define TESTTRACECHANNEL ECC_GameTraceChannel1

AWTCharacterPlayer::AWTCharacterPlayer()
{
	// 데이터 에셋 설정.
	static ConstructorHelpers::FObjectFinder<UWTPlayerDataAsset> PlayerDataRef(
		TEXT("/Game/WantedProject/Data/Player/DA_WTPlayerDataAsset.DA_WTPlayerDataAsset"));
	if (PlayerDataRef.Object)
	{
		PlayerData = PlayerDataRef.Object;
	}
	
	// 초기값 설정.
	CurrentCharacterControlType = ECharacterControlType::TopDown;
	CurrentSkillState=EUltimateSkillState::None;
	CachedDestination = FVector::ZeroVector;
	ShortPressThreshold = 0.3f;
	FollowTime = 0.0f;
	
	// 스프링암 설정.
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	
	// 카메라 설정.
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	// CooldownManager Component.
	CooldownManager = CreateDefaultSubobject<UWTCooldownManagerComponent>(TEXT("CooldownManager"));
	
	// Stat Component.
	Stat = CreateDefaultSubobject<UWTCharacterStatComponent>(TEXT("Stat"));
	
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

void AWTCharacterPlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// 데이터 애셋이 유효한지 확인.
	if (PlayerData)
	{
		GetMesh()->SetSkeletalMesh(PlayerData->PlayerMesh);
		GetMesh()->SetAnimClass(PlayerData->CharacterAnimBP);
		
		// Montages
		DeadMontage = PlayerData->DeadMontage;
		AttackMontage = PlayerData->AttackMontage;
		UltimateSkillMontage = PlayerData->UltimateSkillMontage;
        
		// VFX & SFX
		FXCursor = PlayerData->FXCursor;
		NSArrowCast = PlayerData->NSArrowCast;
		NSDash = PlayerData->NSDash;
		ArrowCastCue = PlayerData->ArrowCastCue;
		DashSoundCue = PlayerData->DashSoundCue;
        
		// Projectiles
		BasicArrowClass = PlayerData->BasicArrowClass;
		UltimateArrowClass = PlayerData->UltimateArrowClass;
	}
}

void AWTCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	// 입력 설정.
	SetCharacterControl(CurrentCharacterControlType);
	
	Stat->OnHpZero.AddUObject(this, &AWTCharacterPlayer::SetDead);
}

void AWTCharacterPlayer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	
	// Binding.
	// All.
	EnhancedInputComponent->BindAction(PlayerData->ChangeControlAction, ETriggerEvent::Triggered, this, &AWTCharacterPlayer::ChangeCharacterController);
	EnhancedInputComponent->BindAction(PlayerData->JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	EnhancedInputComponent->BindAction(PlayerData->DashAction, ETriggerEvent::Started, this, &AWTCharacterPlayer::Dash);
	EnhancedInputComponent->BindAction(PlayerData->UltimateSkillAction, ETriggerEvent::Started, this, &AWTCharacterPlayer::UltimateSkillStarted);
	EnhancedInputComponent->BindAction(PlayerData->UltimateSkillAction, ETriggerEvent::Triggered, this, &AWTCharacterPlayer::UltimateSkillCharging);
	EnhancedInputComponent->BindAction(PlayerData->UltimateSkillAction, ETriggerEvent::Completed, this, &AWTCharacterPlayer::UltimateSkillFiring);

	// TPS.
	EnhancedInputComponent->BindAction(PlayerData->TPSLookAction, ETriggerEvent::Triggered, this, &AWTCharacterPlayer::TPSLook);
	EnhancedInputComponent->BindAction(PlayerData->TPSMoveAction, ETriggerEvent::Triggered, this, &AWTCharacterPlayer::TPSMove);

	// TopDown.
	EnhancedInputComponent->BindAction(PlayerData->TopDownClickAction, ETriggerEvent::Started, this, &AWTCharacterPlayer::OnInputStarted);
	EnhancedInputComponent->BindAction(PlayerData->TopDownClickAction, ETriggerEvent::Triggered, this, &AWTCharacterPlayer::OnSetDestinationTriggered);
	EnhancedInputComponent->BindAction(PlayerData->TopDownClickAction, ETriggerEvent::Completed, this, &AWTCharacterPlayer::OnSetDestinationReleased);
	EnhancedInputComponent->BindAction(PlayerData->TopDownClickAction, ETriggerEvent::Canceled, this, &AWTCharacterPlayer::OnSetDestinationReleased);
	EnhancedInputComponent->BindAction(PlayerData->TopDownShotAction,ETriggerEvent::Started, this, &AWTCharacterPlayer::TopDownShot);
	EnhancedInputComponent->BindAction(PlayerData->TopDownShotAction,ETriggerEvent::Triggered, this, &AWTCharacterPlayer::TopDownShot);
	
}

void AWTCharacterPlayer::SetCharacterControl(ECharacterControlType NewCharacterControlType)
{
	// 변경할 컨트롤 타입에 대응하는 데이터 애셋 로드 (TMap으로부터).
	UWTCharacterControlData* NewCharacterControl = CharacterControlManager[NewCharacterControlType];
	check(NewCharacterControl);

	// 데이터 애셋을 사용해 관련 값 설정.
	SetCharacterControlData(NewCharacterControl);
	
	// 향상된 입력 시스템에 입력 매핑 컨텍스트를 추가.
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (auto SubSystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		SubSystem->ClearAllMappings();
		SubSystem->AddMappingContext(NewCharacterControl->InputMappingContext,0);
	}
	
	// 현재 사용 중인 캐릭터 컨트롤 타입 업데이트.
	CurrentCharacterControlType = NewCharacterControlType;
}

void AWTCharacterPlayer::SetCharacterControlData(const UWTCharacterControlData* InCharacterControlData)
{
	Super::SetCharacterControlData(InCharacterControlData);
	
	// SpringArm 관련 설정.
	SpringArm->TargetArmLength = InCharacterControlData->TargetArmLength;
	SpringArm->SetRelativeRotation(InCharacterControlData->RelativeRotation);
	SpringArm->bUsePawnControlRotation = InCharacterControlData->bUsePawnControlRotation;
	SpringArm->bInheritPitch = InCharacterControlData->bInheritPitch;	
	SpringArm->bInheritYaw = InCharacterControlData->bInheritYaw;
	SpringArm->bInheritRoll = InCharacterControlData->bInheritRoll;
	SpringArm->bDoCollisionTest = InCharacterControlData->bDoCollisionTest;
}

void AWTCharacterPlayer::ChangeCharacterController()
{
	// 사용할 캐릭터 컨트롤을 변경.
	if (CurrentCharacterControlType == ECharacterControlType::TopDown)
	{
		SetCharacterControl(ECharacterControlType::TPS);
	}
	else if (CurrentCharacterControlType == ECharacterControlType::TPS)
	{
		SetCharacterControl(ECharacterControlType::TopDown);
	}
}

// ---------------------Dash-----------------------
void AWTCharacterPlayer::Dash()
{
	// 대시 가능 여부 확인 (쿨타임 체크 용).
	if (!bCanDash) return;

	// 공격 중이거나 스킬 사용 중에는 대시를 할 수 없도록 막음.
	if (!CanAttack || CurrentSkillState != EUltimateSkillState::None)
	{
		return;
	}

	// 이전 이동(내비게이션 등) 정지.
	GetController()->StopMovement();
	
	// 대시 방향 계산. 캐릭터가 바라보는 방향.
	FVector DashLocation = GetActorForwardVector() * 3000.0f;
	
	// LaunchCharacter는 물리 기반으로 이동시켜 줌. 
	// X: 가속도 적용 여부 (true), Z 방향 유지 여부 (false)
	LaunchCharacter(DashLocation, true, false);

	// Niagara 이펙트 재생.
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NSDash,
		FVector(GetActorLocation()), FRotator(GetActorRotation()),
		FVector(1.0f, 1.0f, 1.0f), true);
	
	// 사운드 재생.
	UGameplayStatics::PlaySoundAtLocation(this, DashSoundCue, GetActorLocation());
	
	// 대쉬 잠금.
	bCanDash = false;

	// 대시 쿨다운 타이머 시작. 0.1초마다 ResetDashCooldown() 호출
	GetWorld()->GetTimerManager().SetTimer(DashCoolDownTimerHandle, this, &AWTCharacterPlayer::ResetDashCooldown, 0.1f, true);
}

void AWTCharacterPlayer::ResetDashCooldown()
{
	// 0.1초마다 호출되므로 0.1초씩 쿨다운 시간 감소
	DashCooldown -= 0.1f;

	// HUD가 존재할 경우 쿨다운 UI 갱신 (ex. 대시 게이지 바)
	if (HUDWidget)
	{
		HUDWidget->UpdateDashCooldown(DashCooldown / 2.0f); // 2초 기준으로 비율 계산
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("HUDWidget is null"));
	}
	
	// 쿨다운 완료
	if (DashCooldown <= 0.0f)
	{
		// 타이머 종료
		GetWorld()->GetTimerManager().ClearTimer(DashCoolDownTimerHandle);

		// 대시 가능 상태 복구
		bCanDash = true;

		// 쿨다운 초기화
		DashCooldown = 2.0f;
	}
}

// ---------------------TPS-----------------------
void AWTCharacterPlayer::TPSMove(const FInputActionValue& Value)
{
}

void AWTCharacterPlayer::TPSLook(const FInputActionValue& Value)
{
}

// ----------------------Move-----------------------
void AWTCharacterPlayer::OnInputStarted()
{
	// 공격하고 있을 때는 실행 하지 못하게 함.
	if (!CanAttack)
	{
		return;
	}
	
	// 이전 이동 정지.
	GetController()->StopMovement();
}

void AWTCharacterPlayer::OnSetDestinationTriggered()
{
	// 공격하고 있을 때는 실행 하지 못하게 함.
	if (!CanAttack)
	{
		return;
	}
	
	// 입력 버튼이 눌린 상태라는 것을 기록 (누르고 있는 시간 계산용).
	// 이 시간은 이후에 '짧게 눌렀는지', '길게 눌렀는지' 판단하는 데 사용.
	FollowTime += GetWorld()->GetDeltaSeconds();

	// 마우스 커서 아래에 충돌한 물체의 정보를 저장할 변수.
	FHitResult Hit;
	bool bHitSuccessful = false;
	
	// 현재 캐릭터를 조종하고 있는 PlayerController를 가져온다.
	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	// 마우스 커서 아래에 보이는 물체를 트레이스(충돌 검사) 한다.
	// 성공하면 Hit 변수에 위치 정보 등이 저장.
	bHitSuccessful = PlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);

	// 만약 충돌 검사가 성공했다면 (즉, 땅이나 물체를 클릭한 경우)
	if (bHitSuccessful)
	{
		// 해당 위치를 목적지로 저장.
		CachedDestination = Hit.Location;
	}

	// 캐릭터를 마우스 포인터 방향으로 이동시키기 (입력 지속 시 따라가기)
	FVector WorldDirection = (CachedDestination - GetActorLocation()).GetSafeNormal();
	AddMovementInput(WorldDirection, 1.0, false);

}

void AWTCharacterPlayer::OnSetDestinationReleased()
{
	// 공격하고 있을 때는 실행 하지 못하게 함.
	if (!CanAttack)
	{
		return;
	}
	
	// 짧게 눌렀을 경우 (탭 입력)
	if (FollowTime <= ShortPressThreshold)
	{
		// 해당 위치로 이동하고 파티클 이펙트를 생성
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), CachedDestination);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
	}

	// FollowTime 초기화
	FollowTime = 0.f;
}

// ----------------------Attack-----------------------
void AWTCharacterPlayer::TopDownShot()
{
	// 공격 중이거나 스킬 사용 중에는 할 수 없도록 막음.
	if (!CanAttack || CurrentSkillState != EUltimateSkillState::None)
	{
		return;
	}
	
	// 이전 이동을 멈추도록 명령
	GetController()->StopMovement(); // 컨트롤러 레벨에서 이동 중지 명령 (AI 등 포함)

	// 이동 자체를 비활성화 (물리적으로 멈추게 함)
	GetCharacterMovement()->SetMovementMode(MOVE_None);
	
	// 공격 중이니 다음 공격 금지.
	CanAttack = false;
	
	// 마우스 커서 아래에 충돌한 물체의 정보를 저장할 변수.
	FHitResult Hit;
	bool bHitSuccessful = false;
	
	// 현재 캐릭터를 조종하고 있는 PlayerController를 가져온다.
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	
	// 마우스 커서가 가리키는 화면 위치에서 월드로 선을 쏴서 충돌 검사
	// 충돌된 정보(Hit)에 좌표 등이 들어옴
	bHitSuccessful = PlayerController->GetHitResultUnderCursor(
		ECollisionChannel::ECC_Visibility, // 트레이스 채널
		true,                              // 복잡한 충돌 고려
		Hit                            // 결과 저장
	);
	
	if (bHitSuccessful)
	{
		FVector ActorLocation = GetActorLocation();   // 현재 캐릭터 위치
		CachedDestination = Hit.Location;            // 클릭한 목표 위치 저장

		// 수직 회전을 제거하여 수평으로만 회전하도록 Z축을 맞춤
		CachedDestination.Z = ActorLocation.Z;
		
		// 목표 방향 벡터 계산 후 정규화 (크기 1로 만듦)
		FVector Direction = (CachedDestination - ActorLocation).GetSafeNormal();

		// 유효한 방향일 경우 회전 적용
		if (!Direction.IsNearlyZero())
		{
			FRotator LookAtRotation = Direction.Rotation(); // 바라볼 회전값 계산
			SetActorRotation(LookAtRotation);               // 캐릭터 회전 적용
		}
	}
	
	// 공격 애니메이션 실행
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		const float AttackSpeedRate = 1.0f; // 재생 속도 설정
		AnimInstance->Montage_Play(AttackMontage, AttackSpeedRate); // 애니메이션 몽타주 실행
	}
}

// ----------------------UltimateSkill-----------------------
void AWTCharacterPlayer::UltimateSkillStarted()
{
	// 공격하고 있을 때는 실행 하지 못하게 함.
	if (!CanAttack)
	{
		return;
	}

	if (CurrentSkillState != EUltimateSkillState::None)
	{
		return;
	}
	
	if (CooldownManager->IsOnCooldown(ESkillIdentifier::UltimateAttack))
	{
		return;
	}
	
	CurrentSkillState=EUltimateSkillState::Charging;
	CurrentChargeTime = 0.0f;

	// UI에 "차징 시작!" 신호를 보냄
	OnChargeStateChanged.Broadcast(true);

	// 이전 이동을 멈추도록 명령
	GetController()->StopMovement(); // 컨트롤러 레벨에서 이동 중지 명령 (AI 등 포함)

	// 이동 자체를 비활성화 (물리적으로 멈추게 함)
	GetCharacterMovement()->SetMovementMode(MOVE_None);
	
	// 공격 애니메이션 실행
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		const float AttackSpeedRate = 1.0f; // 재생 속도 설정
		AnimInstance->Montage_Play(UltimateSkillMontage, AttackSpeedRate); // 애니메이션 몽타주 실행

		// StartSection → ChargingSection 으로 자동 전환
		AnimInstance->Montage_SetNextSection(FName("StartSection"), FName("ChargingSection"), UltimateSkillMontage);

		// ChargingSection → ChargingSection 루프 설정
		AnimInstance->Montage_SetNextSection(FName("ChargingSection"), FName("ChargingSection"), UltimateSkillMontage);
	}
}

void AWTCharacterPlayer::UltimateSkillCharging()
{
	if (CooldownManager->IsOnCooldown(ESkillIdentifier::UltimateAttack))
	{
		return;
	}
	
	// 마우스 커서 아래에 충돌한 물체의 정보를 저장할 변수.
	FHitResult Hit;
	bool bHitSuccessful = false;
	
	// 현재 캐릭터를 조종하고 있는 PlayerController를 가져온다.
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	
	// 마우스 커서가 가리키는 화면 위치에서 월드로 선을 쏴서 충돌 검사
	// 충돌된 정보(Hit)에 좌표 등이 들어옴
	bHitSuccessful = PlayerController->GetHitResultUnderCursor(
		ECollisionChannel::ECC_Visibility, // 트레이스 채널
		true,                              // 복잡한 충돌 고려
		Hit                            // 결과 저장
	);
	
	if (bHitSuccessful)
	{
		FVector ActorLocation = GetActorLocation();   // 현재 캐릭터 위치
		CachedDestination = Hit.Location;            // 클릭한 목표 위치 저장

		// 수직 회전을 제거하여 수평으로만 회전하도록 Z축을 맞춤
		CachedDestination.Z = ActorLocation.Z;
		
		// 목표 방향 벡터 계산 후 정규화 (크기 1로 만듦)
		FVector Direction = (CachedDestination - ActorLocation).GetSafeNormal();

		// 유효한 방향일 경우 회전 적용
		if (!Direction.IsNearlyZero())
		{
			FRotator LookAtRotation = Direction.Rotation(); // 바라볼 회전값 계산
			SetActorRotation(LookAtRotation);               // 캐릭터 회전 적용
		}
	}

	// 1. 마지막 프레임 이후 흐른 시간을 가져와 차징 시간에 더함
	// GetWorld()->GetDeltaSeconds()를 사용하는 것이 프레임에 더 안정적입니다.
	CurrentChargeTime += GetWorld()->GetDeltaSeconds();

	// 2. 차징 진행률(0.0 ~ 1.0)을 계산하여 UI에 방송
	float ChargeProgress = FMath::Clamp(CurrentChargeTime / Ultimate_MaxChargeTime, 0.0f, 1.0f);
	OnChargeProgressUpdated.Broadcast(ChargeProgress);

	// 3. 최대 차징 시간에 도달하면 자동으로 발사
	if (CurrentChargeTime >= Ultimate_MaxChargeTime)
	{
		// 키를 뗀 것과 똑같이 발사 함수를 호출합니다.
		// FInputActionValue는 비어있어도 괜찮습니다. 여기서는 호출 자체가 중요.
		UltimateSkillFiring();
	}
}

void AWTCharacterPlayer::UltimateSkillFiring()
{
	if (CooldownManager->IsOnCooldown(ESkillIdentifier::UltimateAttack))
	{
		return;
	}
	
	/*// 차징 중 상태가 아니었으면 아무것도 안 함
	if (CurrentState != ECharacterState::Charging) return;*/
	
	CurrentSkillState=EUltimateSkillState::Firing;

	// 2. UI에 "차징 끝!" 신호를 방송 (게이지 숨김)
	OnChargeStateChanged.Broadcast(false);
	
	// 3. 최종 데미지 배율 계산
	float ChargeProgress = FMath::Clamp(CurrentChargeTime / Ultimate_MaxChargeTime, 0.0f, 1.0f);
	FinalDamageMultiplier = FMath::Lerp(Ultimate_MinDamageMultiplier, Ultimate_MaxDamageMultiplier, ChargeProgress);
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance || !UltimateSkillMontage) return;

	const FName StartSectionName("StartSection");
	const FName FireSectionName("FireSection");
	
	// 재생 중이면 FireSection으로 이동
	if (AnimInstance->Montage_IsPlaying(UltimateSkillMontage))
	{
		FName CurrentSection = AnimInstance->Montage_GetCurrentSection(UltimateSkillMontage);

		// 현재 섹션이 'StartSection'인 경우
		if (CurrentSection == StartSectionName)
		{
			AnimInstance->Montage_SetNextSection(StartSectionName, FireSectionName, UltimateSkillMontage);
		}
		else
		{
			AnimInstance->Montage_JumpToSection(FireSectionName, UltimateSkillMontage);
		}
	}
}

// ----------------------Test-----------------------
void AWTCharacterPlayer::LineTraceSingle()
{
	// 충돌 결과를 저장할 구조체.
	FHitResult OutHitResult;
	
	// 라인 트레이스를 시작할 위치: 현재 액터 위치.
	FVector Start = GetActorLocation();
	// 캐릭터의 포워드 벡터 (정면 방향)
	FVector ForwardVector = GetActorForwardVector();
	// 끝 지점 (정면 방향 * 거리)
	FVector End = Start + ForwardVector * TraceDistance;

	// 라인 트레이스 시 사용할 충돌 쿼리 파라미터 설정
	// - false: 복잡한 충돌 사용 안 함
	// - this: 무시할 액터 (자기 자신)
	FCollisionQueryParams params(SCENE_QUERY_STAT(CustomTrace), false, this);

	// 단일 라인 트레이스를 수행
	// - Start부터 End까지
	// - TESTTRACECHANNEL 채널을 사용
	// - params에 정의된 조건 사용
	bool HitDetected = GetWorld()->LineTraceSingleByChannel(OutHitResult, Start, End, TESTTRACECHANNEL, params);

	if (HitDetected)
	{
		// 충돌이 감지되었을 때 콘솔에 메시지 출력
		FString str = TEXT("Hit Detected!");
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, str);

		// 충돌 지점에 디버그 점을 그림
		DrawDebugPoint(GetWorld(), OutHitResult.ImpactPoint, 20.0f, FColor::Red, false, DrawDebugLifeTime, 0);

		// 시작점 → 충돌 지점까지 빨간 선
		DrawDebugLine(GetWorld(), Start, OutHitResult.ImpactPoint, FColor::Red, false, DrawDebugLifeTime, 0, 1.0f);

		// 충돌 지점 → 목표 지점까지 초록 선 (남은 거리)
		DrawDebugLine(GetWorld(), OutHitResult.ImpactPoint, End, FColor::Green, false, DrawDebugLifeTime, 0, 1.0f);
	}
	else
    {
        // 아무것도 감지되지 않았을 경우 메시지 출력
        FString str = TEXT("Nothing Detected!");
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, str);
    
        // 시작점 → 끝점까지 초록색 선을 그림 (충돌 없음)
        DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, DrawDebugLifeTime, 0, 1.0f);
    }
}

void AWTCharacterPlayer::SetAttacking(bool bAttacking)
{
	CanAttack = bAttacking;
}

void AWTCharacterPlayer::FireArrow()
{
	// 화살이 발사될 소켓에서 위치를 얻음.
	FVector SocketLocation = GetMesh()->GetSocketLocation("arrow_socket");

	// 소켓 위치에서 약간 앞쪽으로 오프셋을 더해 발사 위치 설정 (캐릭터의 앞 방향 기준).
	FVector ForwardOffset = GetActorForwardVector() * 50.f;
	FVector SpawnLocation = SocketLocation + ForwardOffset;

	// 캐릭터의 현재 회전값을 발사체 회전값으로 사용.
	FRotator SpawnRotation = GetActorRotation();
	
	// 액터 스폰 파라미터 설정.
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnParams.Owner = this;           // 소유자 설정 (누가 발사했는지 식별)
	SpawnParams.Instigator = this;      // Instigator 설정 (피해 판정에 사용됨)
	
	// ArrowClass 타입의 화살 액터를 월드에 스폰.
	GetWorld()->SpawnActor<AWTBasicArrow>(BasicArrowClass, SpawnLocation, SpawnRotation, SpawnParams);

	// 나이아가라 이펙트를 발사 위치에서 재생.
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),                 // 월드 참조
		NSArrowCast,               // 사용할 이펙트 시스템
		FVector(SocketLocation),   // 재생 위치
		FRotator(GetActorRotation()), // 회전 방향
		FVector(1.0f, 1.0f, 1.0f), // 스케일
		true                       // 자동 소멸 여부
	);
	
	// 화살 발사 사운드를 현재 캐릭터 위치에서 재생.
	UGameplayStatics::PlaySoundAtLocation(this, ArrowCastCue, GetActorLocation());

	// 공격 중 이동을 멈췄었다면, 다시 걷기 모드로 이동 재활성화.
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

void AWTCharacterPlayer::SetupCharacterWidget(class UUserWidget* InUserWidget)
{
	// 필요한 위젯 정보 가져오기.
	UWTHpBarWidget* HpBarWidget = Cast<UWTHpBarWidget>(InUserWidget);
	if (HpBarWidget)
	{
		// 최대 체력 값 설정.
		HpBarWidget->SetMaxHp(Stat->GetMaxHp());
		
		// HP 퍼센트가 제대로 계산 되도록 현재 체력 설정.
		HpBarWidget->UpdateHpBar(Stat->GetCurrentHp());

		// 체력 변경 이벤트(델리게이트)에 함수 및 객체 정보 등록.
		Stat->OnHpChanged.AddUObject(HpBarWidget, &UWTHpBarWidget::UpdateHpBar);
	}
}

void AWTCharacterPlayer::SetupHUDWidget(class UWTHUDWidget* InHUDWidget)
{
	HUDWidget = Cast<UWTHUDWidget>(InHUDWidget);
	if (HUDWidget)
	{
		HUDWidget->UpdateDashCooldown(0.0f);

		HUDWidget->PlayerSetMaxHp(Stat->GetMaxHp());
		
		// Hp 정보 전달.
		HUDWidget->UpdatePlayerHpBar(Stat->GetCurrentHp());
		
		// 델리게이트 등록.
		Stat->OnHpChanged.AddUObject(HUDWidget, &UWTHUDWidget::UpdatePlayerHpBar);
		CooldownManager->OnCooldownStarted.AddUObject(HUDWidget, &UWTHUDWidget::OnCooldownStarted);
		OnChargeStateChanged.AddDynamic(HUDWidget, &UWTHUDWidget::HandleChargeStateChanged);
		OnChargeProgressUpdated.AddDynamic(HUDWidget, &UWTHUDWidget::HandleChargeProgressUpdated);
	}
}

void AWTCharacterPlayer::UltimateSkillArrow()
{
	// 화살이 발사될 소켓에서 위치를 얻음.
	FVector SocketLocation = GetMesh()->GetSocketLocation("arrow_socket");

	// 소켓 위치에서 약간 앞쪽으로 오프셋을 더해 발사 위치 설정 (캐릭터의 앞 방향 기준).
	FVector ForwardOffset = GetActorForwardVector() * 50.f;
	FVector SpawnLocation = SocketLocation + ForwardOffset;

	// 캐릭터의 현재 회전값을 발사체 회전값으로 사용.
	FRotator SpawnRotation = GetActorRotation();
	
	// 액터 스폰 파라미터 설정.
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnParams.Owner = this;           // 소유자 설정 (누가 발사했는지 식별)
	SpawnParams.Instigator = this;      // Instigator 설정 (피해 판정에 사용됨)
	
	// ArrowClass 타입의 화살 액터를 월드에 스폰.
	AWTUltimateArrow* UltimateArrow = GetWorld()->SpawnActor<AWTUltimateArrow>(UltimateArrowClass, SpawnLocation, SpawnRotation, SpawnParams);

	UltimateArrow->SetDamageMultiplier(FinalDamageMultiplier);
	
	// 나이아가라 이펙트를 발사 위치에서 재생.
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),                 // 월드 참조
		NSArrowCast,               // 사용할 이펙트 시스템
		FVector(SocketLocation),   // 재생 위치
		FRotator(GetActorRotation()), // 회전 방향
		FVector(1.0f, 1.0f, 1.0f), // 스케일
		true                       // 자동 소멸 여부
	);
	
	// 화살 발사 사운드를 현재 캐릭터 위치에서 재생.
	UGameplayStatics::PlaySoundAtLocation(this, ArrowCastCue, GetActorLocation());

	// 공격 중 이동을 멈췄었다면, 다시 걷기 모드로 이동 재활성화.
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	
	CurrentSkillState=EUltimateSkillState::None;

	const float UltimateCooldown = 15.0f; // 쿨타임 15초
	CooldownManager->StartCooldown(ESkillIdentifier::UltimateAttack, UltimateCooldown);
}

float AWTCharacterPlayer::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	Stat->ApplyDamage(DamageAmount);
    
	// 로그를 찍어 데미지가 정상적으로 들어오는지 확인합니다.
	UE_LOG(LogTemp, Log, TEXT("Player %s took %f damage. Current HP: %f"), *GetName(), DamageAmount, Stat ? Stat->GetCurrentHp() : 0.f);
    
	// 최종적으로 적용된 데미지 양을 반환합니다.
	return DamageAmount;
}

void AWTCharacterPlayer::SetDead()
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

void AWTCharacterPlayer::PlayDeadAnimation()
{
	// 몽타주 재생.
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		// 이미 재생 중인 몽타주가 있다면, 모두 종료.
		AnimInstance->StopAllMontages(0.0f);

		// 죽음 몽타주 재생.
		const float PlayRate = 1.0f;
		AnimInstance->Montage_Play(DeadMontage, PlayRate);   
	}
}

void AWTCharacterPlayer::UseDashSkill()
{
}

void AWTCharacterPlayer::UseUltimateSkill()
{
	if (CooldownManager->IsOnCooldown(ESkillIdentifier::UltimateAttack))
	{
		return;
	}

	const float UltimateCooldown = 15.0f; // 쿨타임 15초.
	CooldownManager->StartCooldown(ESkillIdentifier::UltimateAttack, UltimateCooldown);
}

UWTCooldownManagerComponent* AWTCharacterPlayer::GetCooldownManagerComponent()
{
	return CooldownManager;
}
