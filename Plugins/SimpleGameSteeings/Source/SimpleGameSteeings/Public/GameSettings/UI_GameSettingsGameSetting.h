// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/UI_SimpleGameSettingsCore.h"
#include "UI_GameSettingsGameSetting.generated.h"

/**
 * 
 */
UCLASS()
class SIMPLEGAMESETTINGS_API UUI_GameSettingsGameSetting : public UUI_SimpleGameSettingsCore
{
	GENERATED_BODY()
public:
	virtual	void SaveSettings();
	virtual	void LoadSettings();
};
