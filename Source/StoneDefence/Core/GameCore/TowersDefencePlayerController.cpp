// Fill out your copyright notice in the Description page of Project Settings.


#include "TowersDefencePlayerController.h"
#include "TowerDefenceGameCamera.h"
#include "../../StoneDefenceGameMode.h"
#include "TowersDefenceGameState.h"
#include "TowersDefencePlayerState.h"
#include "../../Global/UI_Data.h"

ATowersDefencePlayerController::ATowersDefencePlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
}

void ATowersDefencePlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	float ScreenSpeed = 20.f;
	ScreenMoveUnits.ListenScreenMove(this, ScreenSpeed);

	if (TowerDoll)
	{
		if (MouseTaceHit.Location != FVector::ZeroVector)
		{
			MouseTaceHit = FHitResult();
		}

		FHitResult TaceOutHit;
		GetHitResultUnderCursor(ECollisionChannel::ECC_GameTraceChannel4, true, TaceOutHit);
		TowerDoll->SetActorLocation(TaceOutHit.Location);
	}
	else
	{
		GetHitResultUnderCursor(ECollisionChannel::ECC_GameTraceChannel5, true, MouseTaceHit);
	}
}

void ATowersDefencePlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetInputModeGameAndUI();
}

void ATowersDefencePlayerController::SetInputModeGameAndUI()
{
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
	InputMode.SetHideCursorDuringCapture(false);

	SetInputMode(InputMode);
}

void ATowersDefencePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("MouseWheelUp", IE_Pressed, this, &ATowersDefencePlayerController::MouseWheelUP);
	InputComponent->BindAction("MouseWheelDown", IE_Pressed, this, &ATowersDefencePlayerController::MouseWheelDown);

	InputComponent->BindAction("MouseMiddleButton", IE_Pressed, this, &ATowersDefencePlayerController::MouseMiddleButtonPressed);
	InputComponent->BindAction("MouseMiddleButton", IE_Released, this, &ATowersDefencePlayerController::MouseMiddleButtonReleased);
}

static float WheelValue = 15.f;
void ATowersDefencePlayerController::MouseWheelUP()
{
	ATowerDefenceGameCamera *ZoomCamera = Cast<ATowerDefenceGameCamera>(GetPawn());
	if (ZoomCamera)
	{
		ZoomCamera->Zoom(true, WheelValue);
	}
}

void ATowersDefencePlayerController::MouseWheelDown()
{
	ATowerDefenceGameCamera *ZoomCamera = Cast<ATowerDefenceGameCamera>(GetPawn());
	if (ZoomCamera)
	{
		ZoomCamera->Zoom(false, WheelValue);
	}
}

void ATowersDefencePlayerController::MouseMiddleButtonPressed()
{
	EventMouseMiddlePressed.ExecuteIfBound();
}

void ATowersDefencePlayerController::MouseMiddleButtonReleased()
{
	EventFMouseMiddleReleased.ExecuteIfBound();
}

const FHitResult & ATowersDefencePlayerController::GetHitResult()
{
	return MouseTaceHit;
}

AStoneDefenceGameMode * ATowersDefencePlayerController::GetGameMode()
{
	return GetWorld()->GetAuthGameMode<AStoneDefenceGameMode>();
}

ATowersDefenceGameState * ATowersDefencePlayerController::GetGameState()
{
	return GetWorld()->GetGameState<ATowersDefenceGameState>();
}

void ATowersDefencePlayerController::AddSkillSlot_Client(const FGuid & SlotID)
{
	AddSkillDelegate.ExecuteIfBound(SlotID);
}

void ATowersDefencePlayerController::SpawnBullet_Client(const FGuid &CharacterID, UClass *InClass)
{
	SpawnBulletDelegate.ExecuteIfBound(CharacterID, InClass);
}

ATowers * ATowersDefencePlayerController::SpawnTower(int32 CharacterID, int32 CharacterLevel, const FVector &Loction, const FRotator &Rotator)
{
	if (GetGameMode())
	{
		return GetGameMode()->SpawnTower(CharacterID, CharacterLevel, Loction, Rotator);
	}

	return nullptr;
}

AMonsters * ATowersDefencePlayerController::SpawnMonster(int32 CharacterID, int32 CharacterLevel, const FVector &Loction, const FRotator &Rotator)
{
	if (GetGameMode())
	{
		return GetGameMode()->SpawnMonster(CharacterID, CharacterLevel, Loction, Rotator);
	}

	return nullptr;
}