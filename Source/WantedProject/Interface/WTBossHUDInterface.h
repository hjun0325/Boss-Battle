// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WTBossHUDInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UWTBossHUDInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class WANTEDPROJECT_API IWTBossHUDInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void SetupHUDWidget(class UWTHUDWidget* InHUDWidget) = 0;
};
