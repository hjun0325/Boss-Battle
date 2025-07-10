// Fill out your copyright notice in the Description page of Project Settings.


#include "WTGameMode.h"

AWTGameMode::AWTGameMode()
{
	// DefaultPawnClass.
	ConstructorHelpers::FClassFinder<APawn> DefaultPawnRef(
		TEXT("/Game/WantedProject/BluePrint/BP_WTCharacter.BP_WTCharacter_C"));
	if (DefaultPawnRef.Class)
	{
		DefaultPawnClass = DefaultPawnRef.Class;
	}

	// PlayerControllerClass.
	ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerRef(
		TEXT("/Script/WantedProject.WTPlayerController"));
	if (PlayerControllerRef.Class)
	{
		PlayerControllerClass = PlayerControllerRef.Class;
	}
}
