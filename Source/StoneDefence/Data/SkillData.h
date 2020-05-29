#pragma once

#include "CoreMinimal.h"
#include "Core/DataCore.h"
#include "../StoneDefenceType.h"
#include "SkillData.generated.h"


class UTexture2D;

USTRUCT(BlueprintType)
struct FSkillData : public FDataCore
{
	GENERATED_BODY()

	FSkillData();

	UPROPERTY(EditDefaultsOnly, Category = "Attribute")
		FSkillType SkillType;

	UPROPERTY(EditDefaultsOnly, Category = "Attribute")
		TAssetPtr<class UTexture2D> Icon;

	//添加金币
	UPROPERTY(EditDefaultsOnly, Category = "Skill Profit")
		float Glod;

	//添加生命值
	UPROPERTY(EditDefaultsOnly, Category = "Skill Profit")
		float Health;

	//添加攻击力
	UPROPERTY(EditDefaultsOnly, Category = "Skill Profit")
		float PhysicalAttack;

	//添加护甲
	UPROPERTY(EditDefaultsOnly, Category = "Skill Profit")
		float Armor;

	//0 代表全场景所有敌人 
	UPROPERTY(EditDefaultsOnly, Category = "Skill Profit")
		float AttackRange;

	//技能持续时间
	UPROPERTY(EditDefaultsOnly, Category = "Dynamic Skills")
		float MaxSkillDuration;

	//技能段 技能效果维持一定阶段
	UPROPERTY()//"Dynamic Skills"
		float SkillDuration;

	//技能持续时间
	UPROPERTY(EditDefaultsOnly, Category = "Main Skill")
		float CD;
};
