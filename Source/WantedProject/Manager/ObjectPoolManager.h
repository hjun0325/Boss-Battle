// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObjectPoolManager.generated.h"

// 풀링할 오브젝트의 정보를 담는 구조체
USTRUCT(BlueprintType)
struct FObjectPool
{
	GENERATED_BODY()

	// 특정 클래스의 액터들을 담아두는 배열
	UPROPERTY()
	TArray<TObjectPtr<AActor>> PooledObjects;
};

UCLASS()
class WANTEDPROJECT_API AObjectPoolManager : public AActor
{
	GENERATED_BODY()
	
public:
	AObjectPoolManager();

	// 풀에서 비활성화된 액터를 하나 가져오는 함수
	UFUNCTION(BlueprintCallable, Category = "Object Pool")
	AActor* AcquirePooledObject(TSubclassOf<AActor> ObjectClass);

protected:
	virtual void BeginPlay() override;

protected:
	// 에디터에서 풀링할 클래스와 수량을 미리 지정
	UPROPERTY(EditAnywhere, Category = "Object Pool")
	TMap<TSubclassOf<AActor>, int32> PoolableClasses;

private:
	// 실제 오브젝트들을 저장할 맵
	UPROPERTY()
	TMap<TSubclassOf<AActor>, FObjectPool> PoolMap;

};
