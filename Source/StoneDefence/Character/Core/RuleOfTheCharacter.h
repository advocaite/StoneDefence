// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../../Interface/Character/RuleCharacter.h"
#include "../../Core/GameCore/TowersDefencePlayerController.h"
#include "../../Core/GameCore/TowersDefenceGameState.h"
#include "../../Core/GameCore/TowersDefencePlayerState.h"
#include "../../StoneDefenceType.h"
#include "Engine/StaticMesh.h"
#include "RuleOfTheCharacter.generated.h"




class UBoxComponent;
class UArrowComponent;
class UWidgetComponent;
class USceneComponent;

UCLASS()
class STONEDEFENCE_API ARuleOfTheCharacter : public ACharacter, public IRuleCharacter
{
	GENERATED_BODY()

	//射线捕捉对象
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BaseAttrubute", meta = (AllowPrivateAccess = "true"))
		UBoxComponent *TraceShowCharacterInformation;

	//开火点
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BaseAttrubute", meta = (AllowPrivateAccess = "true"))
		UArrowComponent *OpenFriePoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BaseAttrubute", meta = (AllowPrivateAccess = "true"))
		UWidgetComponent *Widget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BaseAttrubute", meta = (AllowPrivateAccess = "true"))
		USceneComponent *HomePoint;

public:
	UPROPERTY(EditDefaultsOnly, Category = UI)
		TSubclassOf<class ADrawText> DrawTextClass;

	UPROPERTY(EditDefaultsOnly, Category = Death)
		float DelayDeath;

public:
	// Sets default values for this character's properties
	ARuleOfTheCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		virtual void OnClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);
public:
	void UpdateUI();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CharacterAttrubute")
		bool bAttack;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ID")
		FGuid GUID;

protected:
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

public:
	virtual EGameCharacterType::Type GetCharacterType();
	virtual bool IsDeath();
	virtual float GetHealth();
	virtual float GetMaxHealth();
	virtual bool IsTeam();

	virtual FCharacterData &GetCharacterData();

	UFUNCTION(Blueprintable, BlueprintPure, Category = "Towers|Test")
		UStaticMesh *GetDollMesh(FTransform &Transform, int32 MeshID);

public:
	FORCEINLINE ATowersDefencePlayerController* GetGameController() { return GetWorld() ? GetWorld()->GetFirstPlayerController<ATowersDefencePlayerController>() : nullptr; }
	FORCEINLINE ATowersDefenceGameState* GetGameState() { return GetWorld() ? GetWorld()->GetGameState<ATowersDefenceGameState>() : nullptr; }
	FORCEINLINE ATowersDefencePlayerState* GetPlayerState() { return GetGameController()->GetPlayerState<ATowersDefencePlayerState>(); }

	FORCEINLINE USceneComponent *GetHommingPoint() const { return HomePoint; }
	FORCEINLINE UArrowComponent *GetFirePoint() const { return OpenFriePoint; }

public:
	UFUNCTION(Blueprintable, BlueprintPure, Category = "Towers|Attrubute")
		bool IsActive() { return !IsDeath(); }

#if WITH_EDITOR
private:
	FCharacterData NULLData;
#endif
};
