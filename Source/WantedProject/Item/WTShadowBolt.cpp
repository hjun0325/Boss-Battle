// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/WTShadowBolt.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"

AWTShadowBolt::AWTShadowBolt()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// 스피어 콜리전을 루트 컴포넌트로 설정.
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	RootComponent = SphereComponent;

	SphereComponent->SetCollisionObjectType(ECC_GameTraceChannel5); // WTShadowBolt.
	SphereComponent->SetCollisionProfileName(TEXT("WTShadowBolt")); // 커스텀 프로필 사용 권장

	// 파티클 컴포넌트를 생성하고 루트에 부착.
	EffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("EffectComponent"));
	EffectComponent->SetupAttachment(RootComponent);
	
	// 박스 컴포넌트 생성.
	//SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("BoxComponent"));
	//SphereComponent->SetupAttachment(ArrowMesh);
	//SphereComponent->SetSphereRadius(5.0f);
	//SphereComponent->SetRelativeLocation(FVector(0.f, 0.f, -55.0f));
	
	// 프로젝타일 무브먼트 컴포넌트 생성 및 설정.
	ProjectileMovement =
		CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->SetUpdatedComponent(RootComponent);
	ProjectileMovement->InitialSpeed = 1250.f;
	ProjectileMovement->MaxSpeed = 1250.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0.0f;
}

void AWTShadowBolt::BeginPlay()
{
	Super::BeginPlay();

	// Sphere 충돌 시 OnBeginOverlap함수 실행하는 델리게이트 등록.
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AWTShadowBolt::OnBeginOverlap);
}


void AWTShadowBolt::SetDamage(float NewDamage)
{
	DamageAmount = NewDamage;
}

void AWTShadowBolt::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("OnBeginOverlap"));
	if (OtherActor && OtherActor->ActorHasTag("Player"))
	{
		UE_LOG(LogTemp, Warning, TEXT("AWTShadowBolt::OnBeginOverlap"));
		// [수정 5] 데미지를 적용하고, 이 액터를 즉시 파괴하여 게임에서 사라지게 합니다.
		UGameplayStatics::ApplyDamage(
			OtherActor,
			DamageAmount, // C++로 설정된 변수 사용
			GetInstigatorController(),
			this,
			nullptr
		);
		Destroy();
	}
}

