// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WTPoolableObjectInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UWTPoolableObjectInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class WANTEDPROJECT_API IWTPoolableObjectInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// 오브젝트 풀에서 '꺼내올 때(활성화 시)' 호출될 함수.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Poolable")
	void OnAcquiredFromPool();

	// 오브젝트 풀로 '반납할 때(비활성화 시)' 호출될 함수.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Poolable")
	void OnReturnedToPool();
};
