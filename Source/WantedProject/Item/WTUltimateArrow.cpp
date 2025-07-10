// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/WTUltimateArrow.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"

AWTUltimateArrow::AWTUltimateArrow()
{
	DamageAmount = 60.0f;
}

void AWTUltimateArrow::SetDamageMultiplier(float value)
{
	DamageAmount = value * DamageAmount;
}

void AWTUltimateArrow::BeginPlay()
{
	Super::BeginPlay();

	// Box 충돌 시 OnBeginOverlap함수 실행하는 델리게이트 등록.
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AWTUltimateArrow::OnBeginOverlap);
}

void AWTUltimateArrow::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag("Monster"))
	{
		UE_LOG(LogTemp, Warning, TEXT("%f"), DamageAmount);
		UGameplayStatics::ApplyDamage(
			OtherActor, // 피해를 받을 액터
			DamageAmount, // 피해량
			GetInstigatorController(), // 공격자의 컨트롤러
			this, // 공격을 가한 액터 (예: 화살, 총알)
			UDamageType::StaticClass() // 데미지 유형 (기본값은 UDamageType)
			);
	}
}
