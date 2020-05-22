// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "GameSaveSlotList.generated.h"

/**
 * 
 */

USTRUCT()
struct FSaveSlot
{
	GENERATED_USTRUCT_BODY()

	FSaveSlot();

	//�洢�����ݺ�����
	UPROPERTY(SaveGame)
		FText DateText;

	//�ؿ�����
	UPROPERTY(SaveGame)
		FText LevelName;

	//�½�����
	UPROPERTY(SaveGame)
		FText ChapterName;

	//�Ƿ�浵
	UPROPERTY(SaveGame)
		bool bSave;
};

USTRUCT()
struct FSaveSlotList
{
	GENERATED_USTRUCT_BODY()
	
	FSaveSlotList();

	UPROPERTY(SaveGame)
		TMap<int32, FSaveSlot> Slots;

	UPROPERTY(SaveGame)
		TMap<int32, float> DegreeOfCompletion;

	int32 GetSerialNumber();

	bool RemoveAtGameData(int32 SlotNumber);
	bool AddGameDataByNumber(int32 SlotNumber);
};

UCLASS()
class STONEDEFENCE_API UGameSaveSlotList : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(SaveGame)
		FSaveSlotList SlotList;
};