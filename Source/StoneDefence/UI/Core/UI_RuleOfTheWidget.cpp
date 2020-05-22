// Fill out your copyright notice in the Description page of Project Settings.


#include "UI_RuleOfTheWidget.h"
#include "../../Core/GameCore/TowersDefenceGameState.h"

UUI_RuleOfTheWidget::UUI_RuleOfTheWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GUID = FGuid::NewGuid();
}

ATowersDefenceGameState * UUI_RuleOfTheWidget::GetGameState()
{
	if (GetWorld())
	{
		return GetWorld()->GetGameState<ATowersDefenceGameState>();
	}

	return nullptr;
}
