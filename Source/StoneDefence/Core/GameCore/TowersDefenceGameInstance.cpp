// Fill out your copyright notice in the Description page of Project Settings.


#include "TowersDefenceGameInstance.h"
#include "Modules/ModuleManager.h"
#include "SimpleScreenLoading.h"
#include "../RuleOfTheGameState.h"

UTowersDefenceGameInstance::UTowersDefenceGameInstance()
	:ISimpleArchivesInterface()
{

}

int32 UTowersDefenceGameInstance::GetSaveSlotNumber() const
{
	int32 SlotNumber = 20;
	if (ARuleOfTheGameState *InGameState = GetGameState())
	{
		SlotNumber = InGameState->GetSaveSlotList().Slots.Num();
	}

	return SlotNumber;
}

bool UTowersDefenceGameInstance::SaveGameData(int32 SaveNumber)
{
	return false;
}

bool UTowersDefenceGameInstance::ReadGameData(int32 SaveNumber)
{
	return false;
}

UWorld * UTowersDefenceGameInstance::GetSimpleWorld() const
{
	return GetWorld();
}

FSaveSlot * UTowersDefenceGameInstance::GetSaveSlot(int32 SaveNumber)
{
	if (ARuleOfTheGameState *InGameState = GetGameState())
	{
		return InGameState->GetSaveSlot(SaveNumber);
	}

	return nullptr;
}

void UTowersDefenceGameInstance::Init()
{
	Super::Init();

	FSimpleScreenLoadingModule &SimpleScreenLoadingModule = FModuleManager::LoadModuleChecked<FSimpleScreenLoadingModule>("SimpleScreenLoading");
	SimpleScreenLoadingModule.SetupScreenLoading();
}

ARuleOfTheGameState * UTowersDefenceGameInstance::GetGameState() const
{
	return GetSafeWorld() == nullptr ? nullptr : GetSafeWorld()->GetGameState<ARuleOfTheGameState>();
}

UWorld* UTowersDefenceGameInstance::GetSafeWorld() const
{
	if (!GetWorld())
	{
		return GEngine->GetWorld();
	}

	return GetWorld();
}
