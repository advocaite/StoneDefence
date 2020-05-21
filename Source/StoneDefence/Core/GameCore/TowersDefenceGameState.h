// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../RuleOfTheGameState.h"
#include "../../Data/CharacterData.h"
#include "TowersDefenceGameState.generated.h"

extern FCharacterData CharacterDataNULL;
class ARuleOfTheCharacter;
class AMonsters;
class ATowers;
class UGameSaveData;
/**
 * 
 */
UCLASS()
class STONEDEFENCE_API ATowersDefenceGameState : public ARuleOfTheGameState
{
	GENERATED_BODY()
	
	//塔数据
	UPROPERTY()
		UDataTable* AITowerCharacterData;

	//怪物数据
	UPROPERTY()
		UDataTable* AIMonsterCharacterData;

	virtual void BeginPlay() override;
public:
	ATowersDefenceGameState();
	UFUNCTION(BlueprintCallable, Category = Sapwn)
		AMonsters *SpawnMonster(int32 CharacterID, int32 CharacterLevel, const FVector &Location, const FRotator &Rotator);
	
	UFUNCTION(BlueprintCallable, Category = Sapwn)
		ATowers *SpawnTower(int32 CharacterID, int32 CharacterLevel, const FVector &Location, const FRotator &Rotator);
	
	UFUNCTION(BlueprintCallable, Category = SaveData)
		bool SaveGameData(int32 SaveNumber);

	UFUNCTION(BlueprintCallable, Category = SaveData)
		bool ReadGameData(int32 SaveNumber);
protected:
	ARuleOfTheCharacter *SpawnCharacter(int32 CharacterID, int32 CharacterLevel, UDataTable *InCharacterData, const FVector &Location, const FRotator &Rotator);

	template<class T>
	T *SpawnCharacter(int32 CharacterID, int32 CharacterLevel, UDataTable *InCharacterData, const FVector &Location, const FRotator &Rotator)
	{
		return Cast<T>(SpawnCharacter(CharacterID, CharacterLevel, InCharacterData, Location, Rotator));
	}

public:
	const FCharacterData &AddCharacterData(const uint32 &ID, const FCharacterData &Data);
	bool RemoveCharacterData(const uint32 &ID);
	FCharacterData &GetCharacterData(const uint32 &ID);

protected:
	UGameSaveData *GetSaveData();

private:
	UPROPERTY()
		UGameSaveData *SaveData;
};
