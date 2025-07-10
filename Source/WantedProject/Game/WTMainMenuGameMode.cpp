// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/WTMainMenuGameMode.h"

AWTMainMenuGameMode::AWTMainMenuGameMode()
{
	// PlayerControllerClass.
	ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerRef(
		TEXT("/Game/WantedProject/BluePrint/BP_WTMainMenuPlayerController.BP_WTMainMenuPlayerController_C"));
	if (PlayerControllerRef.Class)
	{
		PlayerControllerClass = PlayerControllerRef.Class;
	}
}
