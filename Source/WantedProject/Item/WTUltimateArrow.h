// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/WTArrow.h"
#include "WTUltimateArrow.generated.h"

/**
 * 
 */
UCLASS()
class WANTEDPROJECT_API AWTUltimateArrow : public AWTArrow
{
	GENERATED_BODY()

public:
	AWTUltimateArrow();

	void SetDamageMultiplier(float value);
	
protected:
	virtual void BeginPlay() override;
	
	// OnComponentBeginOverlap 델리게이트에 의해 실행될 Overlap시 처리할 함수.
	UFUNCTION()
	void OnBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult);
};
