// Fill out your copyright notice in the Description page of Project Settings.


#include "UI_HallMenuSystem.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UUI_HallMenuSystem::NativeConstruct()
{
	Super::NativeConstruct();
	GameStartButton			->OnClicked.AddDynamic(this, &UUI_HallMenuSystem::BindGameStart);
	SecretTerritoryButton	->OnClicked.AddDynamic(this, &UUI_HallMenuSystem::BindSecretTerritory);
	HistoryButton			->OnClicked.AddDynamic(this, &UUI_HallMenuSystem::BindHistory);
	GameSettingsButton		->OnClicked.AddDynamic(this, &UUI_HallMenuSystem::BindGameSettings);
	TutorialWebsiteButton	->OnClicked.AddDynamic(this, &UUI_HallMenuSystem::BindTutorialWebsite);
	BrowserButton			->OnClicked.AddDynamic(this, &UUI_HallMenuSystem::BindBrowser);
	SpecialContentButton	->OnClicked.AddDynamic(this, &UUI_HallMenuSystem::BindSpecialContent);
	QuitGameButton			->OnClicked.AddDynamic(this, &UUI_HallMenuSystem::BindQuitGame);
}

void UUI_HallMenuSystem::BindGameStart()
{
	UGameplayStatics::OpenLevel(GetWorld(), "SelectLevel");
}

void UUI_HallMenuSystem::BindSecretTerritory()
{

}

void UUI_HallMenuSystem::BindHistory()
{

}

void UUI_HallMenuSystem::BindGameSettings()
{

}

void UUI_HallMenuSystem::BindTutorialWebsite()
{

}

void UUI_HallMenuSystem::BindBrowser()
{

}

void UUI_HallMenuSystem::BindSpecialContent()
{

}

void UUI_HallMenuSystem::BindQuitGame()
{

}
