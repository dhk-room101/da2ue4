// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/PlayerController.h"
#include "DA2UE4PlayerController.generated.h"

class UFloatyText;
class UImage;

UCLASS()
class ADA2UE4PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ADA2UE4PlayerController();

	UPROPERTY()
	FTimerHandle SoundCueTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
		TSubclassOf<class UUserWidgetMain> wGameUI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
		TSubclassOf<class UUserWidgetConversation> wConversationUI;

	UPROPERTY(VisibleAnywhere, Category = "Audio")
		class UAudioComponent* ConversationAudioComp;
	
	UPROPERTY(VisibleAnywhere, Category = "Audio")
		TAssetPtr<USoundCue> ConversationCuePtr;

	void PlaySoundCue(int32 nSoundID, FString textLine, FString owner);
	USoundCue* GetSoundCue(FString sSoundCue);
	void PlayNextLine();
	void EndConversation();
	void HandleCursorUpdate(FString sName, int32 bInit = 0 /*FALSE_*/);

	UPROPERTY()
		UUserWidgetMain* GameUI = nullptr;
	UPROPERTY()
		TArray<UFloatyText*> floaties;

	UPROPERTY()
		UUserWidgetConversation* ConversationUI = nullptr;
	
	void CreateWidgetFloaty(AActor* oCreature, FString sMessage, int32 nStyle = 0 /*FLOATY_MESSAGE*/, int32 nColor = 16777215, float fDuration = 1.f);

	void SetConversationUILocation();

	TMap<FString, UTexture2D*> moodTextures;
	TMap<FString, UTexture2D*> cursorTextures;
	
	UPROPERTY() 
		UImage* gameCursor = nullptr;
	
protected:

	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	virtual void BeginPlay() override;

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

	//DHK
	void HandlePlayerEvents();
	void HandlePlayerCommands();
	void HandleFloatiesTick();
	void HandleCursorTick();
	
	bool bConvAudioFinished = false;
	void CheckIsNextLine();
	bool IsAudioFinished();

	UFUNCTION() //.AddDynamic
	void OnConvAudioFinished();

	void LoadConversationMoodTextures();
	void LoadCursorTextures();

	void TestStates();
};


