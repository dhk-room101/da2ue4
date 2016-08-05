// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ldf.h"
#include "GameFramework/PlayerController.h"
#include "DA2UE4PlayerController.generated.h"

//forward declaration
class ADA2UE4Creature;
class UWidget;

/**
 *
 */
UCLASS()
class DA2UE4_API ADA2UE4PlayerController : public APlayerController
{
	GENERATED_BODY()
	
	ADA2UE4PlayerController(const class FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	ADA2UE4Creature* GetControlledPlayer() const;

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
		TSubclassOf<class UUserWidgetFloaty> wGameUI;

	//UUserWidgetFloaty* GameUI;
	TArray<UUserWidgetFloaty*> floaties;

	void CreateWidgetFloaty(AActor* oCreature, FString sMessage, int32 nStyle = FLOATY_MESSAGE, int32 nColor = 16777215, float fDuration = 1.f);

protected:
	//DHK
	void TestStates();

	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;

	virtual void SetupInputComponent() override;
	// End PlayerController interface

	/** Navigate player to the current mouse cursor location. */
	void MoveToMouseCursor();

	/** Navigate player to the current touch location. */
	void MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location);

	/** Navigate player to the given world location. */
	void SetNewMoveDestination(const FVector DestLocation);

	/** Input handlers for SetDestination action. */
	void OnSetDestinationPressed();
	void OnSetDestinationReleased();
};
