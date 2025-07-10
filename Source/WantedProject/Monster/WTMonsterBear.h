// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Monster/WTMonsterBase.h"
#include "Interface/WTPoolableObjectInterface.h"
#include "WTMonsterBear.generated.h"

/**
 * 
 */
UCLASS()
class WANTEDPROJECT_API AWTMonsterBear :
public AWTMonsterBase,
public IWTPoolableObjectInterface
{
	GENERATED_BODY()

public:
	AWTMonsterBear();

	// 인터페이스 함수 구현
	virtual void OnAcquiredFromPool_Implementation() override;
	virtual void OnReturnedToPool_Implementation() override;

protected:
	virtual void SetDead() override;
};
