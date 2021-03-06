// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.


#include "StoneDefenceGameMode.h"
#include "Core/GameCore/TowersDefenceGameState.h"
#include "Core/GameCore/TowersDefencePlayerController.h"
#include "Core/GameCore/TowerDefenceGameCamera.h"
#include "Core/GameCore/TowersDefencePlayerState.h"
#include "UI/GameUI/Core/RuleofTheHUD.h"
#include "StoneDefenceUtils.h"
#include "Character/Core/RuleOfTheCharacter.h"
#include "Character/CharacterCore/Towers.h"
#include "Items/SpawnPoint.h"
#include "Character/CharacterCore/Monsters.h"
#include "Engine/DataTable.h"
#include "Bullet/RuleOfTheBullet.h"
#include "Core/GameCore/TowersDefenceGameInstance.h"

AStoneDefenceGameMode::AStoneDefenceGameMode()
{
	GameStateClass = ATowersDefenceGameState::StaticClass();
	PlayerControllerClass = ATowersDefencePlayerController::StaticClass();
	DefaultPawnClass = ATowerDefenceGameCamera::StaticClass();
	PlayerStateClass = ATowersDefencePlayerState::StaticClass();
	HUDClass = ARuleofTheHUD::StaticClass();
}

void AStoneDefenceGameMode::BeginPlay()
{
	Super::BeginPlay();





	if (UTowersDefenceGameInstance *InGameInstance = GetWorld()->GetGameInstance<UTowersDefenceGameInstance>())
	{
		if (ATowersDefenceGameState *InGameState = GetGameState<ATowersDefenceGameState>())
		{
			if (InGameInstance->GetCurrentSaveSlotNumber() == INDEX_NONE &&
				InGameInstance->GetGameType() == EGameSaveType::NONE)
			{
				InitData();
			}
			else //通过存档读取的数据
			{
				//从存档中读取数据
				InitDataFormArchives();

				//清除存档痕迹
				InGameInstance->ClearSaveMark();

				//还原我们场景中的角色
				for (auto &Tmp : InGameState->GetSaveData()->CharacterDatas)
				{
					if (Tmp.Value.Team == ETeam::RED)
					{
						SpawnTower(Tmp.Value.ID, Tmp.Value.Location, Tmp.Value.Rotator, Tmp.Key);
					}
					else
					{
						SpawnMonster(Tmp.Value.ID, Tmp.Value.Location, Tmp.Value.Rotator, Tmp.Key);
					}
				}
			}
		}
	}
}


void AStoneDefenceGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	//更新玩家的数据
	UpdatePlayerData(DeltaSeconds);

	//更新游戏场景的规则
	UpdateGameData(DeltaSeconds);

	//更新生成怪物
	UpdateMonstersRule(DeltaSeconds);

	//更新技能
	UpdateSkill(DeltaSeconds);

	//更新装备栏
	UpdateInventory(DeltaSeconds);

	//更新玩家的技能
	UpdatePlayerSkill(DeltaSeconds);
}

ATowers * AStoneDefenceGameMode::SpawnTower(int32 CharacterID, int32 CharacterLevel, const FVector &Location, const FRotator &Rotator)
{
	return SpawnCharacter<ATowers>(CharacterID, CharacterLevel, GetGameState<ATowersDefenceGameState>()->AITowerCharacterData, Location, Rotator);
}

ATowers * AStoneDefenceGameMode::SpawnTower(int32 CharacterID, const FVector &Location, const FRotator &Rotator, const FGuid &InCharacterGuid /*= FGuid()*/)
{
	return SpawnCharacter<ATowers>(CharacterID, 1, GetGameState<ATowersDefenceGameState>()->AITowerCharacterData, Location, Rotator, InCharacterGuid);
}

AMonsters * AStoneDefenceGameMode::SpawnMonster(int32 CharacterID, int32 CharacterLevel, const FVector &Location, const FRotator &Rotator)
{
	return SpawnCharacter<AMonsters>(CharacterID, CharacterLevel, GetGameState<ATowersDefenceGameState>()->AIMonsterCharacterData, Location, Rotator);
}

AMonsters * AStoneDefenceGameMode::SpawnMonster(int32 CharacterID, const FVector &Location, const FRotator &Rotator, const FGuid &InCharacterGuid /*= FGuid()*/)
{
	return SpawnCharacter<AMonsters>(CharacterID, 1, GetGameState<ATowersDefenceGameState>()->AIMonsterCharacterData, Location, Rotator, InCharacterGuid);
}
int32 GetMonsterLevel(UWorld *InWorld)
{
	struct FDifficultyDetermination
	{
		FDifficultyDetermination()
			:Level(0)
			, Combination(0.f)
			, Attack(0.f)
			, Defense(0.f)
			, Variance(0.f)
		{

		}
		float Level;
		float Combination;
		float Attack;
		float Defense;
		float Variance;
	};

	auto GetDifficultyDetermination = [](TArray<ARuleOfTheCharacter*> &RuleOfTheCharacter)->FDifficultyDetermination
	{
		int32 Index = 0;
		FDifficultyDetermination DifficultyDetermination;
		for (ARuleOfTheCharacter *Tmp : RuleOfTheCharacter)
		{
			if (Tmp->IsActive())
			{
				//拿到等级
				DifficultyDetermination.Level += (float)Tmp->GetCharacterData()->Lv;
				DifficultyDetermination.Attack += Tmp->GetCharacterData()->GetAttack();
				DifficultyDetermination.Defense += Tmp->GetCharacterData()->GetArmor();
				Index++;
			}
		}

		DifficultyDetermination.Level /= Index; //平均值代表着 怪物的起始等级
		DifficultyDetermination.Attack /= Index; //攻击力平均值
		DifficultyDetermination.Defense /= Index; //防御力平均值

		for (ARuleOfTheCharacter *Tmp : RuleOfTheCharacter)
		{
			if (Tmp->IsActive())
			{
				float InValue = (float)Tmp->GetCharacterData()->Lv - DifficultyDetermination.Level;
				DifficultyDetermination.Variance += InValue * InValue;
			}
		}

		DifficultyDetermination.Variance /= Index;

		return DifficultyDetermination;
	};

	TArray<ARuleOfTheCharacter*> Towers;
	TArray<ARuleOfTheCharacter*> Monsters;
	StoneDefenceUtils::GetAllActor<ATowers>(InWorld, Towers);
	StoneDefenceUtils::GetAllActor<AMonsters>(InWorld, Monsters);

	FDifficultyDetermination TowersDD = GetDifficultyDetermination(Towers);
	FDifficultyDetermination MonsterDD = GetDifficultyDetermination(Monsters);

	int32 ReturnLevel = TowersDD.Level;
	if (TowersDD.Attack > MonsterDD.Attack)
	{
		ReturnLevel++;
	}
	if (TowersDD.Defense > MonsterDD.Defense)
	{
		ReturnLevel++;
	}
	//ReturnLevel += FMath::Abs(2 - FMath::Sqrt(TowersDD.Variance / 10.f));

	return ReturnLevel;
}

void AStoneDefenceGameMode::UpdateMonstersRule(float DeltaSeconds)
{
	if (ATowersDefenceGameState *InGameState = GetGameState<ATowersDefenceGameState>())
	{
		if (!InGameState->GetGameData().bCurrentLevelMissionSuccess)
		{
			if (!InGameState->GetGameData().bGameOver)
			{
				if (InGameState->GetGameData().PerNumberOfMonsters.Num())
				{
					InGameState->GetGameData().CurrentSpawnMosnterTime += DeltaSeconds;
					if (InGameState->GetGameData().IsAllowSpawnMosnter())
					{
						InGameState->GetGameData().ResetSpawnMosnterTime();
						int32 MonsterLevel = GetMonsterLevel(GetWorld());

						if (ARuleOfTheCharacter* MyMonster = SpawnMonster(0, MonsterLevel, FVector::ZeroVector, FRotator::ZeroRotator))
						{
							TArray<ASpawnPoint *> MosnterSpawnPoints;
							for (ASpawnPoint* TargetPoint : StoneDefenceUtils::GetAllActor<ASpawnPoint>(GetWorld()))
							{
								if (MyMonster->GetTeamType() == TargetPoint->Team)
								{
									MosnterSpawnPoints.Add(TargetPoint);
								}
							}
							ASpawnPoint *TargetPoint = MosnterSpawnPoints[FMath::RandRange(0, MosnterSpawnPoints.Num() - 1)];
							MyMonster->SetActorLocationAndRotation(TargetPoint->GetActorLocation(), TargetPoint->GetActorRotation());
							InGameState->GetGameData().StageDecision();
						}
					}
				}
			}
		}
		else
		{

		}
	}
}

void AStoneDefenceGameMode::SpawnMainTowersRule()
{
	for (ASpawnPoint *TargetPoint : StoneDefenceUtils::GetAllActor<ASpawnPoint>(GetWorld()))
	{
		if (TargetPoint->Team == ETeam::RED)
		{
			SpawnTower(0, 1, TargetPoint->GetActorLocation(), TargetPoint->GetActorRotation());
		}
	}
}

void AStoneDefenceGameMode::InitDataFormArchives()
{
	if (ATowersDefenceGameState *InGameState = GetGameState<ATowersDefenceGameState>())
	{
		//初始化游戏数据
		InGameState->GetSaveData();

		//初始化存储的列表
		InGameState->GetGameSaveSlotList();

		//初始化玩家数据表
		StoneDefenceUtils::CallUpdateAllClient(GetWorld(), [&](ATowersDefencePlayerController *MyPlayerController)
		{
			if (ATowersDefencePlayerState *InPlayerState = MyPlayerController->GetPlayerState<ATowersDefencePlayerState>())
			{
				InPlayerState->GetSaveData();//初始化玩家数据表
			}
		});
	}
}

void AStoneDefenceGameMode::InitData()
{
	if (ATowersDefenceGameState *InGameState = GetGameState<ATowersDefenceGameState>())
	{
		InGameState->GetGameData().AssignedMonsterAmount();
		//生成主塔
		SpawnMainTowersRule();
	}
}

ARuleOfTheCharacter *AStoneDefenceGameMode::SpawnCharacter(
	int32 CharacterID,
	int32 CharacterLevel,
	UDataTable *InCharacterData,
	const FVector &Location,
	const FRotator &Rotator,
	const FGuid &InCharacterGuid)
{
	ARuleOfTheCharacter * InCharacter = nullptr;

	if (ATowersDefenceGameState *InGameState = GetGameState<ATowersDefenceGameState>())
	{
		if (InCharacterData)
		{
			TArray<FCharacterData*> Datas;
			InCharacterData->GetAllRows(TEXT("CharacterData"), Datas);
			auto GetCharacterData = [&](int32 ID) ->const FCharacterData*
			{
				for (auto &Tmp : Datas)
				{
					if (Tmp->ID == ID)
					{
						return Tmp;
					}
				}
				return nullptr;
			};

			if (const FCharacterData *CharacterData = GetCharacterData(CharacterID))
			{
				//https://blog.csdn.net/qq_29523119/article/details/84455486
				UClass *NewClass = CharacterData->CharacterBlueprintKey.LoadSynchronous();
				if (GetWorld() && NewClass)
				{
					if (ARuleOfTheCharacter *RuleOfTheCharacter = GetWorld()->SpawnActor<ARuleOfTheCharacter>(NewClass, Location, Rotator))
					{
						if (InCharacterGuid == FGuid())//新出来的对象
						{
							RuleOfTheCharacter->ResetGUID();
							if (FCharacterData *CharacterDataInst = InGameState->AddCharacterData(RuleOfTheCharacter->GUID, *CharacterData))
							{
								CharacterDataInst->UpdateHealth();

								if (CharacterLevel > 1)
								{
									for (int32 i = 0; i < CharacterLevel; i++)
									{
										CharacterDataInst->UpdateLevel();
									}
								}

								//初始化被动技能
								RuleOfTheCharacter->InitSkill();

								//注册相应的队伍
								RuleOfTheCharacter->RegisterTeam();
							}
						}
						else//还原的对象
						{
							RuleOfTheCharacter->GUID = InCharacterGuid;
						}
						InCharacter = RuleOfTheCharacter;
					}
				}
			}
		}
	}
	return InCharacter;
}

void AStoneDefenceGameMode::UpdateSkill(float DeltaSeconds)
{
	//auto GetTeam = [&](TArray<TPair<FGuid, FCharacterData>*> &TeamArray,TPair<FGuid, FCharacterData> &Onwer,float InRange,bool bReversed = false)
	//{
	//	auto TeamIner = [&](TPair<FGuid, FCharacterData> &Target)
	//	{
	//		if (InRange != 0)
	//		{
	//			float Distance = (Target.Value.Location - Onwer.Value.Location).Size();
	//			if (Distance <= InRange)
	//			{
	//				TeamArray.Add(&Target);
	//			}
	//		}
	//		else
	//		{
	//			TeamArray.Add(&Target);
	//		}
	//	};

	if (ATowersDefenceGameState *InGameState = GetGameState<ATowersDefenceGameState>())
	{


		//获取范围 有效友军
		//auto GetTeam = [&](TArray<TPair<FGuid, FCharacterData>*> &TeamArray, TPair<FGuid, FCharacterData> &InOwner, float InRange, bool bReversed = false)
		//{
		//	auto TeamIner = [&](TPair<FGuid, FCharacterData> &Target)
		//	{
		//		if (InRange != 0)
		//		{
		//			float Distance = (Target.Value.Location - InOwner.Value.Location).Size();
		//			if (Distance <= InRange)
		//			{
		//				TeamArray.Add(&Target);
		//			}
		//		}
		//		else
		//		{
		//			TeamArray.Add(&Target);
		//		}
		//	};

		//	for (auto &Tmp : InGameState->GetSaveData()->CharacterDatas)
		//	{
		//		if (bReversed)
		//		{
		//			if (Tmp.Value.Team != InOwner.Value.Team)
		//			{
		//				TeamIner(Tmp);
		//			}
		//		}
		//		else
		//		{
		//			if (Tmp.Value.Team == InOwner.Value.Team)
		//			{
		//				TeamIner(Tmp);
		//			}
		//		}
		//	}
		//};

		////多个角色添加同样技能
		//auto AddSkills = [&](TArray<TPair<FGuid, FCharacterData>*> &RecentForces, FSkillData &InSkill)
		//{
		//	//for (auto &CharacterElement : RecentForces)
		//	//{
		//	//	InGameState->AddSkill(*CharacterElement, InSkill);
		//	//}
		//	for (auto &CharacterElement : RecentForces)
		//	{
		//		InGameState->AddSkill(*CharacterElement, InSkill);
		//	}
		//};

		////寻找最近的那个数据目标
		//auto FindRangeTargetRecently = [&](const TPair<FGuid, FCharacterData> &InOwner,bool bReversed = false) -> TPair<FGuid, FCharacterData>*
		//{
		//	float TargetDistance = 99999999;
		//	FGuid Index;

		//	auto InitTargetRecently = [&](TPair<FGuid, FCharacterData> &Pair)
		//	{
		//		FVector Location = Pair.Value.Location;
		//		FVector TmpVector = Location - InOwner.Value.Location;
		//		float Distance = TmpVector.Size();

		//		if (Distance < TargetDistance && Pair.Value.Health > 0)
		//		{
		//			Index = Pair.Key;
		//			TargetDistance = Distance;
		//		}
		//	};

		//	for (auto &Tmp : InGameState->GetSaveData()->CharacterDatas)
		//	{
		//		if (InOwner.Key != Tmp.Key) //排除自己
		//		{
		//			if (bReversed)
		//			{
		//				//寻找敌人
		//				if (InOwner.Value.Team != Tmp.Value.Team)
		//				{
		//					InitTargetRecently(Tmp);
		//				}
		//			}
		//			else
		//			{
		//				//寻找友军
		//				if (InOwner.Value.Team == Tmp.Value.Team)
		//				{
		//					InitTargetRecently(Tmp);
		//				}		
		//			}	
		//		}
		//	}

		//	if (Index != FGuid())
		//	{
		//		for (auto &GameTmp : InGameState->GetSaveData()->CharacterDatas)
		//		{
		//			if (GameTmp.Key == Index)
		//			{
		//				return &GameTmp;
		//			}
		//		}
		//	}

		//	return nullptr;
		//};

		//获取的模板
		const TArray<FSkillData*> &SkillDataTemplate = InGameState->GetSkillDataFromTable();

		for (auto &Tmp : InGameState->GetSaveData()->CharacterDatas)
		{
			if (Tmp.Value.Health > 0.f)
			{
				//计算和更新
				TArray<FGuid> RemoveSkill;
				for (auto &SkillTmp : Tmp.Value.AdditionalSkillData)
				{
					SkillTmp.Value.SkillDuration -= DeltaSeconds;

					if (SkillTmp.Value.SkillType.SkillType == ESkillType::BURST)
					{
						RemoveSkill.Add(SkillTmp.Key);
					}

					//时间到了自然移除
					if (SkillTmp.Value.SkillType.SkillType == ESkillType::SECTION ||
						SkillTmp.Value.SkillType.SkillType == ESkillType::ITERATION)
					{

						if (SkillTmp.Value.SkillDuration <= 0)
						{
							RemoveSkill.Add(SkillTmp.Key);
						}
					}

					//迭代 进行持续更新技能
					if (SkillTmp.Value.SkillType.SkillType == ESkillType::ITERATION)
					{
						SkillTmp.Value.SkillDurationTime += DeltaSeconds;
						if (SkillTmp.Value.SkillDurationTime >= 1.0f)
						{
							SkillTmp.Value.SkillDurationTime = 0.0f;


							//通知客户端进行特效子弹播放
							StoneDefenceUtils::CallUpdateAllClient(GetWorld(), [&](ATowersDefencePlayerController *MyPlayerController) {
								MyPlayerController->SpawnBullet_Client(Tmp.Key, SkillTmp.Value.ID);
							});
						}
					}
				}

				//清理
				for (FGuid &RemoveID : RemoveSkill)
				{
					//通知客户端移除技能
					StoneDefenceUtils::CallUpdateAllClient(GetWorld(), [&](ATowersDefencePlayerController *MyPlayerController) {
						MyPlayerController->RemoveSkillSlot_Server(Tmp.Key, RemoveID);
					});
					Tmp.Value.AdditionalSkillData.Remove(RemoveID);
				}

				TArray<FSkillData> RemoveSkills;
				//技能释放
				for (auto &InSkill : Tmp.Value.CharacterSkill)
				{
					InSkill.CDTime += DeltaSeconds;

					//触发
					if (InSkill.CDTime >= InSkill.CD)
					{
						InSkill.CDTime = 0.f;
						if (!InSkill.bBecomeEffective)
						{
// 							if (InSkill.SkillType.SkillAttackType == ESkillAttackType::MULTIPLE)
// 							{
// 								TArray< TPair<FGuid, FCharacterData>*> Recent;
// 								if (InSkill.SkillType.TargetType == ESkillTargetType::FRIENDLY_FORCES)
// 								{
// 									GetTeam(Recent, Tmp, InSkill.AttackRange);
// 								}
// 								else if (InSkill.SkillType.TargetType == ESkillTargetType::ENEMY)
// 								{
// 									GetTeam(Recent, Tmp, InSkill.AttackRange, true);
// 								}
// 								if (Recent.Num())
// 								{
// 									AddSkills(Recent, InSkill);
// 								}
// 							}
// 							else if (InSkill.SkillType.SkillAttackType == ESkillAttackType::SINGLE)
// 							{
// 								TPair<FGuid, FCharacterData>* Recent = nullptr;
// 								if (InSkill.SkillType.TargetType == ESkillTargetType::FRIENDLY_FORCES)
// 								{
// 									Recent = FindRangeTargetRecently(Tmp);
// 								}
// 								else if (InSkill.SkillType.TargetType == ESkillTargetType::ENEMY)
// 								{
// 									Recent = FindRangeTargetRecently(Tmp, true);
// 								}
// 								if (Recent)
// 								{
// 									InGameState->AddSkill(*Recent, InSkill);
// 								}
// 							}
							InSkill.bBecomeEffective = true;
						}
						else
						{
							RemoveSkills.Add(InSkill);
						}
					}
				}
				for (auto &InSkill : RemoveSkills)
				{
					Tmp.Value.CharacterSkill.Remove(InSkill);
					if (InSkill.SubmissionSkillRequestType == ESubmissionSkillRequestType::AUTO)
					{
						//Call客户端 进行特效子弹播放
						StoneDefenceUtils::CallUpdateAllClient(GetWorld(), [&](ATowersDefencePlayerController *MyPlayerController)
						{
							MyPlayerController->SpawnBullet_Client(Tmp.Key, InSkill.ID);
						});
					}
				}				
			}
			
		}
	}
}

void AStoneDefenceGameMode::UpdatePlayerData(float DeltaSeconds)
{
	if (ATowersDefenceGameState *InGameState = GetGameState<ATowersDefenceGameState>())
	{
		StoneDefenceUtils::CallUpdateAllClient(GetWorld(), [&](ATowersDefencePlayerController *MyPlayerController)
		{
			if (ATowersDefencePlayerState *InPlayerState = MyPlayerController->GetPlayerState<ATowersDefencePlayerState>())
			{
				//游戏金币更新
				InPlayerState->GetPlayerData().GameGoldTime += DeltaSeconds;
				if (InPlayerState->GetPlayerData().IsAllowIncrease())
				{
					InPlayerState->GetPlayerData().GameGoldTime = 0.0f;
					InPlayerState->GetPlayerData().GameGold++;
				}


			}
		}
		);
	}
}

void AStoneDefenceGameMode::UpdateGameData(float DeltaSeconds)
{
	if (ATowersDefenceGameState *InGameState = GetGameState<ATowersDefenceGameState>())
	{
		if (InGameState->GetGameData().GameCount <= 0.f)
		{
			InGameState->GetGameData().bGameOver = true;
		}
		else
		{
			InGameState->GetGameData().GameCount -= DeltaSeconds;
		}

		int32 TowersNum = 0;
		TArray<ARuleOfTheCharacter*> InTowers;
		StoneDefenceUtils::GetAllActor<ATowers>(GetWorld(), InTowers);
		for (ARuleOfTheCharacter *Tower : InTowers)
		{
			if (Tower->IsActive())
			{
				TowersNum++;
			}
		}

		if (TowersNum == 0)
		{
			InGameState->GetGameData().bGameOver = true;
		}
	}
}

void AStoneDefenceGameMode::UpdatePlayerSkill(float DeltaSeconds)
{
	if (ATowersDefenceGameState *InGameState = GetGameState<ATowersDefenceGameState>())
	{
		StoneDefenceUtils::CallUpdateAllClient(GetWorld(), [&](ATowersDefencePlayerController *MyPlayerController)
		{
			if (ATowersDefencePlayerState *InPlayerState = MyPlayerController->GetPlayerState<ATowersDefencePlayerState>())
			{
				for (auto &Tmp : InPlayerState->GetSaveData()->PlayerSkillDatas)
				{
					if (Tmp.Value.IsValid())
					{
						if (Tmp.Value.CDTime > 0.f)
						{
							Tmp.Value.CDTime -= DeltaSeconds;
							Tmp.Value.bBecomeEffective = true;

							//通知客户端更新我们的装备CD
							StoneDefenceUtils::CallUpdateAllClient(GetWorld(), [&](ATowersDefencePlayerController *MyPlayerController)
							{
								MyPlayerController->UpdatePlayerSkill_Client(Tmp.Key, true);
							});
						}
						else if (Tmp.Value.bBecomeEffective)
						{
							Tmp.Value.bBecomeEffective = false;

							//通知客户端更新我们的装备CD
							StoneDefenceUtils::CallUpdateAllClient(GetWorld(), [&](ATowersDefencePlayerController *MyPlayerController)
							{
								MyPlayerController->UpdatePlayerSkill_Client(Tmp.Key, false);
							});
						}
					}
				}
			}
		}
		);
	}
}

void AStoneDefenceGameMode::UpdateInventory(float DeltaSeconds)
{
	if (ATowersDefenceGameState *InGameState = GetGameState<ATowersDefenceGameState>())
	{
		StoneDefenceUtils::CallUpdateAllClient(GetWorld(), [&](ATowersDefencePlayerController *MyPlayerController)
		{
			if (ATowersDefencePlayerState *InPlayerState = MyPlayerController->GetPlayerState<ATowersDefencePlayerState>())
			{
				for (auto &Tmp : InPlayerState->GetSaveData()->BuildingTowers)
				{
					if (Tmp.Value.IsValid())
					{
						if (!Tmp.Value.bLockCD)
						{
							if (!Tmp.Value.bDragICO)
							{
								if (Tmp.Value.CurrentConstrictionTowersCD > 0)
								{
									Tmp.Value.CurrentConstrictionTowersCD -= DeltaSeconds;
									Tmp.Value.bCallUpdateTowersInfo = true;

									//通知客户端更新我们的装备CD
									StoneDefenceUtils::CallUpdateAllClient(GetWorld(), [&](ATowersDefencePlayerController *MyPlayerController)
									{
										MyPlayerController->UpdateInventory_Client(Tmp.Key, true);
									});
								}
								else if (Tmp.Value.bCallUpdateTowersInfo)
								{
									Tmp.Value.bCallUpdateTowersInfo = false;
									//准备构建的塔
									Tmp.Value.TowersPerpareBuildingNumber--;
									Tmp.Value.TowersConstructionNumber++;

									//通知客户端更新我们的装备CD
									StoneDefenceUtils::CallUpdateAllClient(GetWorld(), [&](ATowersDefencePlayerController *MyPlayerController)
									{
										MyPlayerController->UpdateInventory_Client(Tmp.Key, false);
									});

									if (Tmp.Value.TowersPerpareBuildingNumber > 0)
									{
										Tmp.Value.ResetCD();
									}
								}
							}
						}
					}
				}
			}
		}
		);
	}
}
