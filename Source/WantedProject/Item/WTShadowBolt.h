// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WTShadowBolt.generated.h"

UCLASS()
class WANTEDPROJECT_API AWTShadowBolt : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWTShadowBolt();
	
	// 네크로맨서가 이 투사체를 발사할 때, 이 함수를 통해 최종 데미지를 설정해줍니다.
	void SetDamage(float NewDamage);

protected:
	// Called when the game starts or when spawned
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
	
protected:
	UPROPERTY(EditAnywhere)
	float DamageAmount = 30.0f;

	// 외형을 담당할 컴포넌트.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UNiagaraComponent> EffectComponent;

	// 총돌 범위 설정.
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USphereComponent> SphereComponent;

	// 발사체 설정.
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UProjectileMovementComponent> ProjectileMovement;
};
