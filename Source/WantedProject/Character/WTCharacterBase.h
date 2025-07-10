// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WTCharacterBase.generated.h"

// 캐릭터 컨트롤 타입.
UENUM()
enum class ECharacterControlType : uint8
{
	TPS,
	TopDown
};

UCLASS()
class WANTEDPROJECT_API AWTCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AWTCharacterBase();
	
	virtual void SetCharacterControlData(const class UWTCharacterControlData* InCharacterControlData);

protected:
	UPROPERTY(EditAnywhere, Category = CharacterControl, meta = (AllowPrivateAccess = "true"))
	TMap<ECharacterControlType, class UWTCharacterControlData*> CharacterControlManager;
};
