// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WTCooldownWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "CharacterCoolDown/WTCooldownManagerComponent.h"

UWTCooldownWidget::UWTCooldownWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UWTCooldownWidget::SetupSkillIcon(class UWTCooldownManagerComponent* InManager, ESkillIdentifier InSkillID)
{
	CooldownManager = InManager;
	SkillID = InSkillID;
}

void UWTCooldownWidget::StartCooldownUI(float CooldownDuration)
{
	CooldownTotalDuration = CooldownDuration;
	if (CooldownTotalDuration > 0.f)
	{
		// 쿨타임 UI를 보이게 하고, 틱을 활성화하여 업데이트를 시작.
		Image_CooldownOverlay->SetVisibility(ESlateVisibility::Visible);
		TextBlock_CooldownTime->SetVisibility(ESlateVisibility::Visible);
		FWidgetTransform CurrentTransform = Image_CooldownOverlay->GetRenderTransform();
		CurrentTransform.Scale = FVector2D(1.0f, 1.0f);
		Image_CooldownOverlay->SetRenderTransform(CurrentTransform);
		//SetIsTickable(true);
	}
}

void UWTCooldownWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Image_SkillIcon = Cast<UImage>(GetWidgetFromName(TEXT("SkillImage")));
	ensure(Image_SkillIcon);
	
	Image_CooldownOverlay = Cast<UImage>(GetWidgetFromName(TEXT("Cooldown")));
	ensure(Image_CooldownOverlay);

	TextBlock_CooldownTime = Cast<UTextBlock>(GetWidgetFromName(TEXT("CooldownTime")));
	ensure(TextBlock_CooldownTime);
	
	// 처음에는 쿨타임 UI를 숨기고, 틱을 꺼두어 성능을 아낀다.
	Image_CooldownOverlay->SetVisibility(ESlateVisibility::Hidden);
	TextBlock_CooldownTime->SetVisibility(ESlateVisibility::Hidden);
	/*SetTickableWhenPaused(false);
	SetIsTickable(false);*/
}

void UWTCooldownWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!CooldownManager) return;
	
	const float RemainingTime = CooldownManager->GetRemainingCooldown(SkillID);
	if (RemainingTime > 0.f)
	{
		// 남은 시간을 텍스트로 표시. (소수점 한 자리까지)
		TextBlock_CooldownTime->SetText(FText::FromString(FString::Printf(TEXT("%.1f"), RemainingTime)));

		float Progress = RemainingTime / CooldownTotalDuration;
		FWidgetTransform CurrentTransform = Image_CooldownOverlay->GetRenderTransform();
		CurrentTransform.Scale = FVector2D(1.0f, Progress); // Y축 스케일을 0부터 1까지 변화
		Image_CooldownOverlay->SetRenderTransform(CurrentTransform);
	}
	else
	{
		// 쿨타임이 끝나면 UI를 다시 숨기고, 틱을 비활성화.
		Image_CooldownOverlay->SetVisibility(ESlateVisibility::Hidden);
		TextBlock_CooldownTime->SetVisibility(ESlateVisibility::Hidden);
		//SetIsTickable(false);
	}
}
