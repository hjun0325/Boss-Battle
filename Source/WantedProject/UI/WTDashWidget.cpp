// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WTDashWidget.h"
#include "Components/Image.h"
#include "Interface/WTCharacterWidgetInterface.h"

UWTDashWidget::UWTDashWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UWTDashWidget::SetPercent(float InPercent)
{
	if (RoundProgressbarInst == nullptr)
	{
		// CooldownMaterialBase를 기반으로 동적 머티리얼 인스턴스를 생성.
		RoundProgressbarInst = UMaterialInstanceDynamic::Create(CooldownMaterialBase, this);

		if (CooldownImage == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("CooldownImage is null"));
			return;
		}

		if (RoundProgressbarInst == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("RoundProgressbarInst is null"));
		}
		
		// 생성한 머티리얼 인스턴스를 이미지 위젯(Progress Bar)에 적용.
		CooldownImage->SetBrushFromMaterial(RoundProgressbarInst);
	}
	// (머티리얼 안에 Scalar Parameter 'Percent'가 있어야 함).
	// 'Percent'라는 파라미터 값을 설정.
	RoundProgressbarInst->SetScalarParameterValue("Percent", InPercent);
}

void UWTDashWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 이 함수가 호출되면, UI에 대한 대부분의 값이 초기화 되었다고 생각할 수 있다.
	
	// 위젯 참조 설정을 위해 이름으로 검색.
	CooldownImage = Cast<UImage>(GetWidgetFromName(TEXT("Cooldown")));
	ensure(CooldownImage);

	/*if (CooldownImage==nullptr)
	{
		return;
	}
	if (CooldownImage->GetBrush().GetResourceObject()==nullptr)
	{
		return;
	}*/

	CooldownMaterialBase = Cast<UMaterialInterface>(CooldownImage->GetBrush().GetResourceObject());
	ensure(CooldownMaterialBase);

	SetPercent(Percent);
	
	// 캐릭터에 내 정보(위젯)을 전달.
	// 강참조를 피하기 위해 인터페이스를 통해 우회해 전달(느슨한 결합).
	/*IWTCharacterWidgetInterface* CharacterWidget = Cast<IWTCharacterWidgetInterface>(OwningActor);
	if (CharacterWidget)
	{
		// 인터페이스를 통해서 캐릭터 내 정보(위젯)을 전달.
		CharacterWidget->SetupCharacterWidget(this);
	}*/
}
