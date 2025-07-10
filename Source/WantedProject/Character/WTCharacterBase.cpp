// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/WTCharacterBase.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "WTCharacterControlData.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AWTCharacterBase::AWTCharacterBase()
{
 	// 해당 액터가 Tick()을 호출할 수 있도록 허용하는 플래그
	PrimaryActorTick.bCanEverTick = true;

	//PrimaryActorTick.bStartWithTickEnabled = true;

	// 태그 설정.
	Tags.Add(FName("Player"));
	
	// 컨트롤러의 회전을 받아서 설정하는 모드를 모두 해제.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;	
	bUseControllerRotationRoll = false;

	// 무브먼트 설정.
	
	// 캐릭터가 이동하는 방향을 바라보게 자동으로 회전한다.
	// bUseControllerRotationYaw = false여야 자연스럽게 동작한다.
	GetCharacterMovement()->bOrientRotationToMovement = true;
	
	GetCharacterMovement()->RotationRate = FRotator(0.0f,1080.0f,0.0f);
	GetCharacterMovement()->JumpZVelocity = 500.0f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;
	
	// 캐릭터 이동을 특정 평면(Plane)에 제한한다.
	GetCharacterMovement()->bConstrainToPlane = true;
	// 게임 시작할 때 캐릭터를 평면(Plane) 위에 자동으로 맞춘다.
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// 컴포넌트 설정.
	GetCapsuleComponent()->SetCapsuleHalfHeight(88.0f);
	//GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));

	// 메시의 콜리전은 NoCollision 설정 (주로 랙돌에 사용됨).
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));
	GetMesh()->SetRelativeLocationAndRotation(
		FVector(0.0f, 0.0f, -88.0f),
		FRotator(0.0f, -90.0f, 0.0f));

	// Control Data 설정.
	static ConstructorHelpers::FObjectFinder<UWTCharacterControlData> TPSDataRef(
		TEXT("/Game/WantedProject/CharacterControl/DA_TPS.DA_TPS"));
	if (TPSDataRef.Object)
	{
		CharacterControlManager.Add(
			ECharacterControlType::TPS,
			TPSDataRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UWTCharacterControlData> TopDownDataRef(
		TEXT("/Game/WantedProject/CharacterControl/DA_TopDown.DA_TopDown"));
	if (TopDownDataRef.Object)
	{
		CharacterControlManager.Add(
			ECharacterControlType::TopDown,
			TopDownDataRef.Object);
	}
}

void AWTCharacterBase::SetCharacterControlData(const UWTCharacterControlData* InCharacterControlData)
{
	// Pawn.
	bUseControllerRotationYaw = InCharacterControlData->bUseControllerRotationYaw;
	
	// CharacterMovement.
	GetCharacterMovement()->bOrientRotationToMovement =
		InCharacterControlData->bOrientRotationToMovement;
	GetCharacterMovement()->bUseControllerDesiredRotation =
		InCharacterControlData->bUseControllerDesiredRotation;
	GetCharacterMovement()->RotationRate =
		InCharacterControlData->RotationRate;
}
