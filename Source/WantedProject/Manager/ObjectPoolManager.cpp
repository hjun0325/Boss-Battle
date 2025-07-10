// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/ObjectPoolManager.h"
#include "Interface/WTPoolableObjectInterface.h"

AObjectPoolManager::AObjectPoolManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AObjectPoolManager::BeginPlay()
{
	Super::BeginPlay();
	
	UE_LOG(LogTemp, Warning, TEXT("ObjectPoolManager: Initializing pool..."));

	// 게임이 시작될 때, PoolableClasses 맵에 지정된 정보대로 오브젝트들을 미리 생성.
	for (auto const& [ObjectClass, PoolSize] : PoolableClasses)
	{
		if (ObjectClass)
		{
			UE_LOG(LogTemp, Log, TEXT("Found Class '%s' to pool. Size: %d"), *ObjectClass->GetName(), PoolSize);
			
			FObjectPool NewPool;
			for (int32 i = 0; i < PoolSize; ++i)
			{
				// 스폰 파라미터를 설정.
				FActorSpawnParameters SpawnParams;
				
				// 충돌 처리 방법을 '충돌을 무시하고 항상 스폰'으로 변경.
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				
				// 월드에 액터를 스폰.
				AActor* NewObj = GetWorld()->SpawnActor<AActor>(ObjectClass, GetActorLocation(), FRotator::ZeroRotator,SpawnParams);
				if (NewObj)
				{
					// 매니저는 '반납하라'는 명령만 내리고, 구체적인 방법은 오브젝트가 알아서 처리합니다.
					if (NewObj->GetClass()->ImplementsInterface(UWTPoolableObjectInterface::StaticClass()))
					{
						IWTPoolableObjectInterface::Execute_OnReturnedToPool(NewObj);
					}
					NewPool.PooledObjects.Add(NewObj);
				}
			}
			PoolMap.Add(ObjectClass, NewPool);
		}
	}
}

AActor* AObjectPoolManager::AcquirePooledObject(TSubclassOf<AActor> ObjectClass)
{
	// Find 함수를 사용하여 해당 클래스의 풀에 대한 포인터를 가져온다.
	FObjectPool* Pool = PoolMap.Find(ObjectClass);

	// 만약 풀이 존재한다면,
	if (Pool)
	{
		// 해당 풀에 있는 모든 오브젝트를 하나씩 확인.
		for (AActor* Obj : Pool->PooledObjects)
		{
			// 오브젝트가 유효하고, 틱이 꺼져있는지(비활성 상태인지) 확인.
			if (Obj && !Obj->IsActorTickEnabled()) 
			{
				// 찾았으면, 반환하기 직전에 '활성화' 함수를 호출.
				if (Obj->GetClass()->ImplementsInterface(UWTPoolableObjectInterface::StaticClass()))
				{
					IWTPoolableObjectInterface::Execute_OnAcquiredFromPool(Obj);
				}
                
				// 깨어난 오브젝트를 요청한 곳에 전달.
				return Obj;
			}
		}
	}
    
	// 만약 루프를 다 돌았는데도 사용 가능한 오브젝트가 없다면(모두 사용 중이라면),
	UE_LOG(LogTemp, Warning, TEXT("No available object in pool for class %s"), *ObjectClass->GetName());
	return nullptr;
}
