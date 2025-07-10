// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WTPlayerDataAsset.generated.h"

// 전방 선언
class USkeletalMesh; 
class UAnimInstance;
class UInputAction;
class UAnimMontage;
class UNiagaraSystem;
class USoundBase;
class AWTBasicArrow;
class AWTUltimateArrow;

/**
 * 
 */
UCLASS()
class WANTEDPROJECT_API UWTPlayerDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
  
public:
	// 캐릭터 외형
    UPROPERTY(EditDefaultsOnly, Category = "Character Visuals")
    TObjectPtr<USkeletalMesh> PlayerMesh;

    UPROPERTY(EditDefaultsOnly, Category = "Character Visuals")
    TSubclassOf<UAnimInstance> CharacterAnimBP;

    // 입력 액션
    UPROPERTY(EditDefaultsOnly, Category = "Input Actions") TObjectPtr<UInputAction> ChangeControlAction;
    UPROPERTY(EditDefaultsOnly, Category = "Input Actions") TObjectPtr<UInputAction> JumpAction;
    UPROPERTY(EditDefaultsOnly, Category = "Input Actions") TObjectPtr<UInputAction> DashAction;
    UPROPERTY(EditDefaultsOnly, Category = "Input Actions") TObjectPtr<UInputAction> TPSMoveAction;
    UPROPERTY(EditDefaultsOnly, Category = "Input Actions") TObjectPtr<UInputAction> TPSLookAction;
    UPROPERTY(EditDefaultsOnly, Category = "Input Actions") TObjectPtr<UInputAction> TopDownClickAction;
    UPROPERTY(EditDefaultsOnly, Category = "Input Actions") TObjectPtr<UInputAction> TopDownShotAction;
    UPROPERTY(EditDefaultsOnly, Category = "Input Actions") TObjectPtr<UInputAction> UltimateSkillAction;

    // 애니메이션 몽타주
    UPROPERTY(EditDefaultsOnly, Category = "Montages") TObjectPtr<UAnimMontage> DeadMontage;
    UPROPERTY(EditDefaultsOnly, Category = "Montages") TObjectPtr<UAnimMontage> AttackMontage;
    UPROPERTY(EditDefaultsOnly, Category = "Montages") TObjectPtr<UAnimMontage> UltimateSkillMontage;
    
    // 이펙트 및 사운드
    UPROPERTY(EditDefaultsOnly, Category = "VFX & SFX") TObjectPtr<UNiagaraSystem> FXCursor;
    UPROPERTY(EditDefaultsOnly, Category = "VFX & SFX") TObjectPtr<UNiagaraSystem> NSArrowCast;
    UPROPERTY(EditDefaultsOnly, Category = "VFX & SFX") TObjectPtr<UNiagaraSystem> NSDash;
    UPROPERTY(EditDefaultsOnly, Category = "VFX & SFX") TObjectPtr<USoundBase> ArrowCastCue;
    UPROPERTY(EditDefaultsOnly, Category = "VFX & SFX") TObjectPtr<USoundBase> DashSoundCue;
    
    // 투사체 클래스
    UPROPERTY(EditDefaultsOnly, Category = "Projectile") TSubclassOf<AWTBasicArrow> BasicArrowClass;
    UPROPERTY(EditDefaultsOnly, Category = "Projectile") TSubclassOf<AWTUltimateArrow> UltimateArrowClass;
};
