// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WTUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class WANTEDPROJECT_API UWTUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 위젯 소유 액터를 설정할 때 사용할 함수.
	FORCEINLINE void SetOwningActor(AActor* NewOwner) {OwningActor = NewOwner;}

protected:
	// 위젯을 소유하는 액터 참조 변수.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Actor)
	TObjectPtr<AActor> OwningActor;
};
