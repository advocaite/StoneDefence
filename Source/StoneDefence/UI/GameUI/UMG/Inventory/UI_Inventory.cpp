// Fill out your copyright notice in the Description page of Project Settings.


#include "UI_Inventory.h"
#include "Components/UniformGridSlot.h"
#include "Components/UniformGridPanel.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "UI_InventorySlot.h"

void UUI_Inventory::NativeConstruct()
{
	Super::NativeConstruct();

	LayoutInventroySlot(3, 7);
}

void UUI_Inventory::LayoutInventroySlot(int32 ColumNumber, int32 RowNumber)
{
	if (InventorySlotClass)
	{
		for (int32 MyRow = 0; MyRow < RowNumber; MyRow++)
		{
			//������Ʒ��
			for (int32 MyColum = 0; MyColum < ColumNumber; MyColum++)
			{
				if (UUI_InventorySlot *SlotWidget = CreateWidget<UUI_InventorySlot>(GetWorld(), InventorySlotClass))
				{
					if (UUniformGridSlot *GridSlot = SlotArrayInventory->AddChildToUniformGrid(SlotWidget))
					{
						GridSlot->SetColumn(MyColum);
						GridSlot->SetRow(MyRow);
						GridSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
						GridSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
					}
					InventorySlotArray.Add(SlotWidget);
				}
			}
		}

		const TArray<const FGuid*> ID = GetGameState()->GetBuildingTowersID();
		for (int32 i = 0; i < ColumNumber * RowNumber; i++)
		{
			InventorySlotArray[i]->GUID = *ID[i];
		}

		TArray<const FCharacterData*> Datas;
		if (GetGameState()->GetCharacterDataFromTable(Datas))
		{
			for (int32 i = 0; i < Datas.Num(); i++)
			{
				InventorySlotArray[i]->GetBuildingTower().TowerID = Datas[i]->ID;
				InventorySlotArray[i]->GetBuildingTower().NeedGold = Datas[i]->Glod;
				InventorySlotArray[i]->GetBuildingTower().MaxConstructionTowersCD = Datas[i]->ConstructionTime;
				InventorySlotArray[i]->GetBuildingTower().ICO = Datas[i]->Icon.LoadSynchronous();

				InventorySlotArray[i]->UpdateUI();
			}
		}
	}
}
