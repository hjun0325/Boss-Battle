// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/WTCharacterBase.h"
#include "Interface/WTAttackAnimationInterface.h"
#include "Interface/WTCharacterWidgetInterface.h"
#include "Interface/WTCharacterHUDInterface.h"
#include "WTCharacterPlayer.generated.h"

// =============================================================
// 전방 선언 (Forward Declarations)
// - 컴파일 속도 향상을 위해 .h 파일에서는 클래스 이름만 미리 알려준다.
// =============================================================
class USpringArmComponent;
class UCameraComponent;
class UWTCharacterStatComponent;
class UWTCooldownManagerComponent;
class UWTWidgetComponent;
class UWTPlayerDataAsset;
class UInputAction;
class UAnimMontage;
class UWTHUDWidget;
class AWTBasicArrow;
class AWTUltimateArrow;
class UNiagaraSystem;

UENUM()
enum class EUltimateSkillState : uint8
{
	None,
	Charging,
	Firing
};

// 캐릭터의 현재 행동 상태를 나타내는 열거형
UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	Idle,       // 기본(다른 행동 가능) 상태
	Attacking,  // 공격 중
	Dashing,    // 대시 중
	Charging,   // 스킬 충전 중
	Stunned,    // 행동 불가 (피격 등)
	Dead        // 죽음
};

// 차징 상태 변경을 UI에 알릴 델리게이트 (시작/종료)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChargeStateChangedDelegate, bool, bIsCharging);
// 차징 진행률(%)을 UI에 알릴 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChargeProgressUpdatedDelegate, float, Progress);

UCLASS()
class WANTEDPROJECT_API AWTCharacterPlayer :
public AWTCharacterBase,
public IWTAttackAnimationInterface,
public IWTCharacterWidgetInterface,
public IWTCharacterHUDInterface
{
	GENERATED_BODY()

public:
	// 생성자.
	AWTCharacterPlayer();

	// 엔진 재정의 함수.
	virtual void PostInitializeComponents() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;
	
	// 컨트롤 타입에 변경에 따라 추가적으로 설정해야 하는 함수. 
	void SetCharacterControl(ECharacterControlType NewCharacterControlType);

	// 캐릭터 컨트롤 데이터 값으로 설정하는 함수.
	virtual void SetCharacterControlData(const UWTCharacterControlData* InCharacterControlData) override;
	
protected:
	// --- 입력 처리 함수 ---
	void ChangeCharacterController();
	void Dash();
	void ResetDashCooldown();
	void TPSMove(const struct FInputActionValue& Value);
	void TPSLook(const struct FInputActionValue& Value);
	void OnInputStarted();
	void OnSetDestinationTriggered();
	void OnSetDestinationReleased();
	void TopDownShot();
	void UltimateSkillStarted();
	void UltimateSkillCharging();
	void UltimateSkillFiring();

	// =============================================================
	// WTAttackAnimationInterface
	// =============================================================

	// LineTrace 함수.
	virtual void LineTraceSingle() override;

	// 어택 가능한지 설정하는 함수.
	virtual void SetAttacking(bool isAttacking) override;

	// 화살 발사 하는 함수.
	virtual void FireArrow() override;

	// 궁극기 스킬 화살 발사하는 함수.
	virtual void UltimateSkillArrow() override;

	// =============================================================
	// WTCharacterWidgetInterface
	// =============================================================
	
	// 캐릭터 위젯 설정 함수.
	virtual void SetupCharacterWidget(class UUserWidget* InUserWidget) override;

	// =============================================================
	// WTCharacterHUDInterface
	// =============================================================
	
	// HUD위젯 설정 함수.
	virtual void SetupHUDWidget(class UWTHUDWidget* InHUDWidget) override;

	// 쿨타임 매니저 컴포넌트 반환 함수.
	virtual UWTCooldownManagerComponent* GetCooldownManagerComponent() override;

	// =============================================================

	// 몬스터의 공격을 '수신'
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	// 죽음 상태 설정 함수.
	virtual void SetDead();

	// 죽는 애니메이션 재생 함수.
	void PlayDeadAnimation();

	// 스킬 함수들
	void UseDashSkill();
	void UseUltimateSkill();

public:
	// UI가 구독할 델리게이트들
	UPROPERTY(BlueprintAssignable, Category = "ChargeSkill")
	FOnChargeStateChangedDelegate OnChargeStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "ChargeSkill")
	FOnChargeProgressUpdatedDelegate OnChargeProgressUpdated;
	
protected:
	// --- 핵심 컴포넌트 ---
	// 카메라를 제어하는 스프링암 컴포넌트.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USpringArmComponent> SpringArm;

	// 플레이어가 보게 될 카메라 컴포넌트.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> Camera;

	// 캐릭터의 스탯(HP, 공격력 등)을 관리하는 컴포넌트.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UWTCharacterStatComponent> Stat;
	
	// 스킬 쿨타임을 관리하는 컴포넌트.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UWTCooldownManagerComponent> CooldownManager;
	
	// 머리 위에 HP 바를 표시하기 위한 위젯 컴포넌트.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UWTWidgetComponent> HpBar;

	// --- 데이터 및 상태 변수 ---
	// 플레이어의 모든 애셋 참조를 담고 있는 데이터 애셋.
	UPROPERTY(EditDefaultsOnly, Category = "Data")
	TObjectPtr<UWTPlayerDataAsset> PlayerData;

	// 캐릭터의 현재 행동 상태를 관리하는 핵심 변수.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	ECharacterState CurrentState;

	// 현재 카메라 시점을 관리하는 변수.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	ECharacterControlType CurrentCharacterControlType;

	// 궁극기 스킬 상태 관리 변수.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	EUltimateSkillState CurrentSkillState;

	// 죽음 몽타주 애셋.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> DeadMontage;
	
	// 공격 몽타주 애셋.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> AttackMontage;

	// 궁극기 몽타주 애셋.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> UltimateSkillMontage;

	// 기본 공격 화살.
	UPROPERTY(EditAnywhere)
	TSubclassOf<AWTBasicArrow> BasicArrowClass;

	// 궁극기 공격 화살.
	UPROPERTY(EditAnywhere)
	TSubclassOf<AWTUltimateArrow> UltimateArrowClass;
	
	// Effect Section.
	// 클릭 시 생성할 FX 이펙트 클래스.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UNiagaraSystem* FXCursor;
	
	// 나이아가라 시스템 화살 발사.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UNiagaraSystem* NSArrowCast;

	// 화살 발사 사운드.
	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* ArrowCastCue;
	
	// 나이아가라 시스템 대쉬
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UNiagaraSystem* NSDash;

	// 대쉬 사운드.
	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* DashSoundCue;
	
	// 짧은 클릭으로 인식할 시간 문턱 값.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	float ShortPressThreshold;
	
	// trace 길이.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Trace, meta = (AllowPrivateAccess = "true"))
	float TraceDistance = 3000.0f;

	// 디버그 유지 시간.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Trace, meta = (AllowPrivateAccess = "true"))
	float DrawDebugLifeTime = 10.0f;
	
	// 변경 사항을 전달하기 위한 HUD위젯 선언.
	UPROPERTY()
	TObjectPtr<class UWTHUDWidget> HUDWidget;
	
private:
	// 목적지 위치.
	FVector CachedDestination;

	bool bIsPreparingToAttack = true;

	// 터치 디바이스인지 여부 (1비트 플래그).
	uint8 bIsTouch : 1;

	// 클릭(또는 터치)이 얼마나 오래 지속되었는지 시간.
	float FollowTime;

	// 공격 체크 변수.
	bool CanAttack = true;
	
	// 대시 쿨타임 타이머 변수.
	FTimerHandle DashCoolDownTimerHandle;
	
	// 대시 가능 여부.
	bool bCanDash = true;
	
	// 대시 쿨타임.
	float DashCooldown = 2.0f;

	bool bShouldRotateToTarget = false;
	FVector RotateTargetLocation;

	// 차징 관련 변수들
	FTimerHandle ChargeTimerHandle; // 차징 시간을 측정할 타이머 핸들
	float CurrentChargeTime;        // 현재까지 충전한 시간
	float Ultimate_MaxChargeTime = 2.0f;
	float Ultimate_MinDamageMultiplier = 1.0f;
	float Ultimate_MaxDamageMultiplier = 2.0f;
	float FinalDamageMultiplier;
};
