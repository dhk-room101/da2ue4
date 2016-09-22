// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "DA2UE4.h"
#include "DA2UE4PlayerController.h"
#include "AI/Navigation/NavigationSystem.h"

#include "DA2UE4Library.h"
#include "DA2UE4GameInstance.h"
#include "DA2UE4Party.h"
#include "DA2UE4Creature.h"
#include "DA2UE4Item.h"
#include "DA2UE4TargetPoint.h"
#include "DA2UE4SaveGame.h"
#include "DA2UE4AnimInstance.h"

#include "ldf.h"
#include "events_h.h"
#include "utility_h.h"
#include "wrappers_h.h"
#include "STypes.h"

#include "UMG.h"
#include "WidgetBlueprintLibrary.h"

#include "Blueprint/UserWidget.h"
#include "WidgetTree.h"
#include "CanvasPanel.h"
#include "CanvasPanelSlot.h"
#include "TextWidgetTypes.h"
#include "TextBlock.h"
#include "UserWidgetMain.h"
#include "UserWidgetConversation.h"
#include "FloatyText.h"
#include "WidgetLayoutLibrary.h"

#include "conversation_h.h"

ADA2UE4PlayerController::ADA2UE4PlayerController()
{
	ConversationAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("ConversationAudioComp"));
	ConversationAudioComp->SetupAttachment(RootComponent);
	//ConversationAudioComp->bAutoActivate = false;
	//ConversationAudioComp->bAutoDestroy = false;

	bShowMouseCursor = true;
	bEnableMouseOverEvents = true;

	LoadConversationMoodTextures();
	LoadCursorTextures();
}

void ADA2UE4PlayerController::BeginPlay()
{
	Super::BeginPlay();

	LogWarning("Player Controller ON");

	//conv UI
	if (wConversationUI)
	{
		ConversationUI = CreateWidget<UUserWidgetConversation>(this, wConversationUI);

		if (ConversationUI)
		{
			ConversationUI->SetVisibility(ESlateVisibility::Hidden);
			ConversationUI->AddToViewport();
		}
	}

	if (ConversationAudioComp)
	{
		ConversationAudioComp->OnAudioFinished.AddDynamic(this, &ADA2UE4PlayerController::OnConvAudioFinished);
	}

	//prepare main game UI
	if (wGameUI)
	{
		GameUI = CreateWidget<UUserWidgetMain>(this, wGameUI);

		if (GameUI)
		{
			GameUI->AddToViewport();

			GameUI->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			GameUI->SetVisibility(ESlateVisibility::HitTestInvisible);

			UCanvasPanel* canvas = Cast<UCanvasPanel>(GameUI->WidgetTree->RootWidget);
			if (canvas)
			{
				canvas->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				canvas->SetVisibility(ESlateVisibility::HitTestInvisible);

				HandleCursorUpdate("standard", TRUE_);
			}
		}
	}

	//when spawn, update the singleton world to match current
	bool IsValid;

	if (GetCurrentWorld() != GetWorld())
	{
		UDA2UE4Library::GetDA2UE4Data(IsValid)->DA2UE4CurrentWorld = GetWorld();
#ifdef DEBUG
		LogWarning("updating world in singleton to " + GetWorld()->GetName());
#endif
	}

	UDA2UE4GameInstance* module = Cast<UDA2UE4GameInstance>(GetGameInstance());
	if (module->GAME_INIT_TYPE == 0 /*NEW*/)
	{
		LogWarning("New Game!");

		Cast<ADA2UE4Creature>(GetPawn())->CreatureSpawn();

		//when spawn set player character in global instance to party leader store if not already

		module->PLAYER_LEADER_STORE = FName(*GetPawn()->GetName());

		module->PARTY->Henchman0 = FName(*GetPawn()->GetName());

		//set group to PLAYER
		Cast<ADA2UE4Creature>(GetPawn())->GROUP = GROUP_PC;
	}
	else //1/Load
	{
		LogWarning("Load Game!");

// 		bool bSucceeded = false;
// 		FString sLoad = FPaths::GameSavedDir() + "autosave.sav";
// 		if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*sLoad))
// 		{
// 			bSucceeded = Cast<UDA2UE4GameInstance>(GetWorld()->GetGameInstance())->LoadGameDataFromFile(sLoad);
// 			if (!bSucceeded)
// 				LogError("Load Error: " + sLoad);
// 			else LogWarning("Load succeeded: " + sLoad);
// 		}
// 		else LogError("There is no load file in this location: " + sLoad);
	}

	//move player to WP
	for (TActorIterator<ADA2UE4TargetPoint> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ADA2UE4TargetPoint* oWaypoint = *ActorItr;
		if (oWaypoint->GetName().Contains(module->WAYPOINT.ToString()))
		{
			GetPawn()->SetActorLocationAndRotation(oWaypoint->GetActorLocation(), oWaypoint->GetActorRotation());
			break;
		}
	}
}

void ADA2UE4PlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if (GetPawn() == nullptr || GetModule() == nullptr) return;

	HandleCursorTick();

	//handle player only events and/or commands
	if (Cast<ADA2UE4Creature>(GetPawn())->qEvents.Num() > 0)
		HandlePlayerEvents();
	if (Cast<ADA2UE4Creature>(GetPawn())->currentCommand.nCommandType != COMMAND_TYPE_INVALID)
		HandlePlayerCommands();

	//handle module events as well here
	if (GetModule()->qEvents.Num() > 0)
		GetModule()->HandleModuleEvents();

	// keep updating the destination every tick while desired
	if (GetGameMode() == EGameMode::GM_EXPLORE || GetGameMode() == EGameMode::GM_COMBAT)
	{
		if (bMoveToMouseCursor)
		{
			(Cast<ADA2UE4Creature>(GetPawn()))->StopCustomMontages();
			
			MoveToMouseCursor();
		}
	}

	//floaty tick
	if (floaties.Num() > 0)
		HandleFloatiesTick();
}

//floaty
void ADA2UE4PlayerController::CreateWidgetFloaty(AActor* oCreature, FString sMessage, int32 nStyle, int32 nColor, float fDuration)
{
	//create Text
	UFloatyText* textWidget = NewObject<UFloatyText>(this, UFloatyText::StaticClass());
	//set text
	textWidget->SetText(FText::FromString(sMessage));
	textWidget->SetJustification(ETextJustify::Center);

	//set font
	FSlateFontInfo font = FSlateFontInfo(FPaths::GameDevelopersDir() / TEXT("Source/Fonts/Mikadan.ttf"), 13);
	textWidget->SetFont(font);
	//set color, convert from 0-255/0xFF to 0-1 range
	float r = ((nColor >> 16) & 255) / 255;
	float g = ((nColor >> 8) & 255) / 255;
	float b = (nColor & 255) / 255;
	float a = 255 / 255;
	textWidget->SetColorAndOpacity(FLinearColor(r, g, b, a));

	//set owner
	textWidget->oCreature = oCreature;
	//timestamp and duration
	textWidget->fTimestamp = GetWorld()->GetTimeSeconds();
	textWidget->fDuration = fDuration;
	//set initial position relative to owner
	float BoundingRadius = oCreature->GetRootComponent()->Bounds.SphereRadius;
	textWidget->vPosition = oCreature->GetActorLocation() + *(new FVector(10, -35, BoundingRadius));

	floaties.Add(textWidget);

	//get root
	UCanvasPanel* canvas = Cast<UCanvasPanel>(GameUI->WidgetTree->RootWidget);
	//add text to root
	canvas->AddChild(Cast<UWidget>(textWidget));

	int32 index = canvas->GetChildIndex(textWidget);
	UFloatyText* _text = Cast<UFloatyText>(canvas->GetChildAt(index));

	Cast<UCanvasPanelSlot>(_text->Slot)->SetSize(FVector2D(400, 40));
}

//inputs and move
void ADA2UE4PlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &ADA2UE4PlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &ADA2UE4PlayerController::OnSetDestinationReleased);

	// support touch devices 
	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ADA2UE4PlayerController::MoveToTouchLocation);
	InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &ADA2UE4PlayerController::MoveToTouchLocation);

	InputComponent->BindAction("TestStates", IE_Pressed, this, &ADA2UE4PlayerController::TestStates);
}

void ADA2UE4PlayerController::MoveToMouseCursor()
{
	// Trace to see what is under the mouse cursor
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (Hit.bBlockingHit)
	{
		// We hit something, move there
		SetNewMoveDestination(Hit.ImpactPoint);
	}
}

void ADA2UE4PlayerController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	FVector2D ScreenSpaceLocation(Location);

	// Trace to see what is under the touch location
	FHitResult HitResult;
	GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult);
	if (HitResult.bBlockingHit)
	{
		// We hit something, move there
		SetNewMoveDestination(HitResult.ImpactPoint);
	}
}

void ADA2UE4PlayerController::SetNewMoveDestination(const FVector DestLocation)
{
	if (GetPawn())
	{
		UNavigationSystem* const NavSys = GetWorld()->GetNavigationSystem();
		float const Distance = FVector::Dist(DestLocation, GetPawn()->GetActorLocation());

		// We need to issue move command only if far enough in order for walk animation to play correctly
		if (NavSys && (Distance > 120.0f))
		{
			NavSys->SimpleMoveToLocation(this, DestLocation);
		}
	}
}

void ADA2UE4PlayerController::OnSetDestinationPressed()
{
	// set flag to keep updating destination until released
	bMoveToMouseCursor = true;
}

void ADA2UE4PlayerController::OnSetDestinationReleased()
{
	// clear flag to indicate we should stop updating the destination
	bMoveToMouseCursor = false;
}

void ADA2UE4PlayerController::HandlePlayerEvents()
{
	FGameEvent ev = GetCurrentEvent(GetPawn());
	if (ev.nGameEventType == EVENT_TYPE_INVALID) return;

#ifdef DEBUG
	ev = SetEventString(ev, *(this->GetName()));
	Log_Events("", ev);
#endif

	int32 nEventType = GetEventType(ev);

	switch (nEventType)
	{
	case EVENT_TYPE_CONVERSATION:
	{
		AActor* oTarget = GetEventObject(ev, 0);
		UT_Talk(oTarget, Cast<ADA2UE4Creature>(oTarget)->CONVERSATION);

		break;
	}
	default:
	{
#ifdef DEBUG
		LogWarning("player event not found");
#endif
		break;
	}
	}
}

void ADA2UE4PlayerController::HandlePlayerCommands()
{

}

void ADA2UE4PlayerController::HandleFloatiesTick()
{
	for (int32 i = 0; i < floaties.Num(); i++)
	{
		//get current floaty
		//UFloatyText* floaty = Cast<UFloatyText>(root->GetChildAt(i));
		UFloatyText* floaty = floaties[i];
		if (GetWorld()->GetTimeSeconds() > floaty->fTimestamp + floaty->fDuration)
		{
			//remove
			if (floaty)
			{
				floaties.Remove(floaty);

				UCanvasPanel* canvas = Cast<UCanvasPanel>(GameUI->WidgetTree->RootWidget);
				canvas->RemoveChild(floaty);
			}
		}
		else //reposition
		{
			//prepare 2D position
			FVector2D v2d = FVector2D(0, 0);
			FVector2D& ScreenLocation = v2d;
			//project 3D to 2D
			float xx = floaty->vPosition.X + 0.5f;
			float yy = floaty->vPosition.Y;
			float zz = floaty->vPosition.Z + 0.5f;
			floaty->vPosition = FVector(xx, yy, zz);

			float Scale = UWidgetLayoutLibrary::GetViewportScale(GameUI);

			ProjectWorldLocationToScreen(floaty->vPosition, ScreenLocation);
			UCanvasPanel* canvas = Cast<UCanvasPanel>(GameUI->WidgetTree->RootWidget);

			int32 index = canvas->GetChildIndex(floaty);
			UFloatyText* _text = Cast<UFloatyText>(canvas->GetChildAt(index));

			Cast<UCanvasPanelSlot>(_text->Slot)->SetPosition(FVector2D(ScreenLocation.X / Scale, ScreenLocation.Y / Scale));
		}
	}
}

void ADA2UE4PlayerController::HandleCursorTick()
{
	UCanvasPanel* canvas = Cast<UCanvasPanel>(GameUI->WidgetTree->RootWidget);
	int32 index = canvas->GetChildIndex(gameCursor);
	UWidget* _cursor = canvas->GetChildAt(index);

	float _x = 0;
	float &x = _x;
	float _y = 0;
	float &y = _y;
	GetMousePosition(x, y);

	float Scale = UWidgetLayoutLibrary::GetViewportScale(GameUI);

	Cast<UCanvasPanelSlot>(_cursor->Slot)->SetPosition(FVector2D(x / Scale, y / Scale));

	//gameCursor->SetRenderTranslation(FVector2D(x / Scale, y / Scale));
}

void ADA2UE4PlayerController::HandleCursorUpdate(FString sName, int32 bInit)
{
	UCanvasPanel* canvas = Cast<UCanvasPanel>(GameUI->WidgetTree->RootWidget);

	if (bInit == TRUE_)
	{
		gameCursor = NewObject<UImage>();
		gameCursor->Rename(TEXT("cursor"));

		UTexture2D* t0 = *cursorTextures.Find(sName);
		FSlateBrush b0 = UWidgetBlueprintLibrary::MakeBrushFromTexture(t0);
		gameCursor->SetBrush(b0);
		gameCursor->SetVisibility(ESlateVisibility::Visible);

		canvas->AddChildToCanvas(gameCursor);

		int32 index = canvas->GetChildIndex(gameCursor);
		UWidget* _cursor = canvas->GetChildAt(index);
		Cast<UCanvasPanelSlot>(_cursor->Slot)->SetSize(FVector2D(32, 32));
	}
	else
	{
		int32 index = canvas->GetChildIndex(gameCursor);
		UImage* _cursor = Cast<UImage>(canvas->GetChildAt(index));
		UTexture2D* t0 = *cursorTextures.Find(sName);
		FSlateBrush b0 = UWidgetBlueprintLibrary::MakeBrushFromTexture(t0);
		gameCursor->SetBrush(b0);
		gameCursor->SetVisibility(ESlateVisibility::Visible);
	}
}

void ADA2UE4PlayerController::SetConversationUILocation()
{
	FVector pLoc = GetHero()->GetActorLocation();
	const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
	float Scale = UWidgetLayoutLibrary::GetViewportScale(ConversationUI);
	FVector2D canvasSize = FVector2D(800, 200);//manually?
	FVector2D v2d = FVector2D(0, 0);
	FVector2D& ScreenLocation = v2d;
	//project 3D to 2D
	ProjectWorldLocationToScreen(pLoc, ScreenLocation);
	ConversationUI->SetPositionInViewport(FVector2D(ScreenLocation.X - canvasSize.X / 4, ScreenLocation.Y + canvasSize.Y / 2 / Scale));
}

void ADA2UE4PlayerController::EndConversation()
{
	ConversationUI->conversation = FConversation();
	ConversationUI->CONVERSATION_DEFAULT_SPEAKER = nullptr;
	ConversationUI->endConversation = false;//reset for next use

	ResetLayout();
	ConversationUI->SetVisibility(ESlateVisibility::Hidden);

	// 	SetLocalInt(GetModule(), "CONVERSATION_IN_PROGRESS", EngineConstants.FALSE);

	WR_SetGameMode(static_cast<uint8>(EGameMode::GM_EXPLORE));
}

void ADA2UE4PlayerController::LoadConversationMoodTextures()
{
	static ConstructorHelpers::FObjectFinder<UTexture2D> arcaneTexture(TEXT("/Game/Data/Art/Textures/CNV/DW-arcane"));
	moodTextures.Add("arcane", arcaneTexture.Object);
	static ConstructorHelpers::FObjectFinder<UTexture2D> attackTexture(TEXT("/Game/Data/Art/Textures/CNV/DW-attack"));
	moodTextures.Add("attack", attackTexture.Object);
	static ConstructorHelpers::FObjectFinder<UTexture2D> confusedTexture(TEXT("/Game/Data/Art/Textures/CNV/DW-confused"));
	moodTextures.Add("confused", confusedTexture.Object);
	static ConstructorHelpers::FObjectFinder<UTexture2D> dwarfTexture(TEXT("/Game/Data/Art/Textures/CNV/DW-dwarf"));
	moodTextures.Add("dwarf", dwarfTexture.Object);
	static ConstructorHelpers::FObjectFinder<UTexture2D> elfTexture(TEXT("/Game/Data/Art/Textures/CNV/DW-elf"));
	moodTextures.Add("elf", elfTexture.Object);
	static ConstructorHelpers::FObjectFinder<UTexture2D> endromanceTexture(TEXT("/Game/Data/Art/Textures/CNV/DW-endromance"));
	moodTextures.Add("endromance", endromanceTexture.Object);
	static ConstructorHelpers::FObjectFinder<UTexture2D> generalTexture(TEXT("/Game/Data/Art/Textures/CNV/DW-general"));
	moodTextures.Add("general", generalTexture.Object);
	static ConstructorHelpers::FObjectFinder<UTexture2D> historyTexture(TEXT("/Game/Data/Art/Textures/CNV/DW-history"));
	moodTextures.Add("history", historyTexture.Object);
	static ConstructorHelpers::FObjectFinder<UTexture2D> humanTexture(TEXT("/Game/Data/Art/Textures/CNV/DW-human"));
	moodTextures.Add("human", humanTexture.Object);
	static ConstructorHelpers::FObjectFinder<UTexture2D> humanmageTexture(TEXT("/Game/Data/Art/Textures/CNV/DW-humanmage"));
	moodTextures.Add("humanmage", humanmageTexture.Object);
	static ConstructorHelpers::FObjectFinder<UTexture2D> investigateTexture(TEXT("/Game/Data/Art/Textures/CNV/DW-investigate"));
	moodTextures.Add("investigate", investigateTexture.Object);
	static ConstructorHelpers::FObjectFinder<UTexture2D> madTexture(TEXT("/Game/Data/Art/Textures/CNV/DW-mad"));
	moodTextures.Add("mad", madTexture.Object);
	static ConstructorHelpers::FObjectFinder<UTexture2D> mageTexture(TEXT("/Game/Data/Art/Textures/CNV/DW-mage"));
	moodTextures.Add("mage", mageTexture.Object);
	static ConstructorHelpers::FObjectFinder<UTexture2D> noTexture(TEXT("/Game/Data/Art/Textures/CNV/DW-no"));
	moodTextures.Add("no", noTexture.Object);
	static ConstructorHelpers::FObjectFinder<UTexture2D> paymentTexture(TEXT("/Game/Data/Art/Textures/CNV/DW-payment"));
	moodTextures.Add("payment", paymentTexture.Object);
	static ConstructorHelpers::FObjectFinder<UTexture2D> pleasedTexture(TEXT("/Game/Data/Art/Textures/CNV/DW-pleased"));
	moodTextures.Add("pleased", pleasedTexture.Object);
	static ConstructorHelpers::FObjectFinder<UTexture2D> politicsTexture(TEXT("/Game/Data/Art/Textures/CNV/DW-politics"));
	moodTextures.Add("politics", politicsTexture.Object);
	static ConstructorHelpers::FObjectFinder<UTexture2D> qunariTexture(TEXT("/Game/Data/Art/Textures/CNV/DW-qunari"));
	moodTextures.Add("qunari", qunariTexture.Object);
	static ConstructorHelpers::FObjectFinder<UTexture2D> rogueTexture(TEXT("/Game/Data/Art/Textures/CNV/DW-rogue"));
	moodTextures.Add("rogue", rogueTexture.Object);
	static ConstructorHelpers::FObjectFinder<UTexture2D> romanceTexture(TEXT("/Game/Data/Art/Textures/CNV/DW-romance"));
	moodTextures.Add("romance", romanceTexture.Object);
	static ConstructorHelpers::FObjectFinder<UTexture2D> sadTexture(TEXT("/Game/Data/Art/Textures/CNV/DW-sad"));
	moodTextures.Add("sad", sadTexture.Object);
	static ConstructorHelpers::FObjectFinder<UTexture2D> specialTexture(TEXT("/Game/Data/Art/Textures/CNV/DW-special"));
	moodTextures.Add("special", specialTexture.Object);
	static ConstructorHelpers::FObjectFinder<UTexture2D> stoicTexture(TEXT("/Game/Data/Art/Textures/CNV/DW-stoic"));
	moodTextures.Add("stoic", stoicTexture.Object);
	static ConstructorHelpers::FObjectFinder<UTexture2D> surprisedTexture(TEXT("/Game/Data/Art/Textures/CNV/DW-surprised"));
	moodTextures.Add("surprised", surprisedTexture.Object);
	static ConstructorHelpers::FObjectFinder<UTexture2D> underworldTexture(TEXT("/Game/Data/Art/Textures/CNV/DW-underworld"));
	moodTextures.Add("underworld", underworldTexture.Object);
	static ConstructorHelpers::FObjectFinder<UTexture2D> warriorTexture(TEXT("/Game/Data/Art/Textures/CNV/DW-warrior"));
	moodTextures.Add("warrior", warriorTexture.Object);
	static ConstructorHelpers::FObjectFinder<UTexture2D> yesTexture(TEXT("/Game/Data/Art/Textures/CNV/DW-yes"));
	moodTextures.Add("yes", yesTexture.Object);
}

void ADA2UE4PlayerController::LoadCursorTextures()
{
	static ConstructorHelpers::FObjectFinder<UTexture2D> area_transitionTexture(TEXT("/Game/Data/Art/Textures/CUR/area_transition"));
	cursorTextures.Add("area_transition", area_transitionTexture.Object);
	static ConstructorHelpers::FObjectFinder<UTexture2D> attackTexture(TEXT("/Game/Data/Art/Textures/CUR/attack"));
	cursorTextures.Add("attack", attackTexture.Object);
	static ConstructorHelpers::FObjectFinder<UTexture2D> conversationTexture(TEXT("/Game/Data/Art/Textures/CUR/conversation"));
	cursorTextures.Add("conversation", conversationTexture.Object);
	static ConstructorHelpers::FObjectFinder<UTexture2D> standardTexture(TEXT("/Game/Data/Art/Textures/CUR/standard"));
	cursorTextures.Add("standard", standardTexture.Object);
}

void ADA2UE4PlayerController::OnConvAudioFinished()
{
#ifdef DEBUG
	LogWarning("OnConvAudioFinished");

#endif // DEBUG
	bConvAudioFinished = true;

	//reset audio comp
	USoundCue* empty = NewObject<USoundCue>();
	ConversationAudioComp->SetSound(empty);
}

void ADA2UE4PlayerController::CheckIsNextLine()
{
	if (IsAudioFinished() && ConversationUI->endConversation)
		EndConversation();

	if (IsAudioFinished() && ConversationUI->ConvNextLineReply.lineIndex != -1)
	{
		PlayNextLine();
	}
}

void ADA2UE4PlayerController::PlayNextLine()
{
	FConvLine l = ConversationUI->ConvNextLineReply;
	ConversationUI->ConvNextLineReply = FConvLine();
	if (l.owner == "player")
	{
#ifdef DEBUG
		LogWarning("Player lines to " + IntToString(l.lineIndex));

#endif // DEBUG

		PlayerLines(l.lineIndex);
	}
	else if (l.owner == "npc")
	{
#ifdef DEBUG
		LogWarning("NPC lines to " + IntToString(l.lineIndex));

#endif // DEBUG
		NPCLines(l.lineIndex);
	}
	else
	{
#ifdef DEBUG
		LogError("Unknown conv line owner!!");
#endif // DEBUG
	}
}

bool ADA2UE4PlayerController::IsAudioFinished()
{
	return bConvAudioFinished;
}

void ADA2UE4PlayerController::PlaySoundCue(int32 nSoundID, FString textLine, FString owner)
{
#ifdef DEBUG

	LogWarning("Playing sound cue ID " + IntToString(nSoundID));

#endif // DEBUG

	bConvAudioFinished = false;

	ConversationUI->TextNPC->SetText(FText::FromString(owner + ": " + textLine));

	FString sSoundCue;
	sSoundCue += "/Game/Data/Art/Sounds/";
	sSoundCue += IntToString(nSoundID);
	sSoundCue += "_m_Cue.";//TODO sound gender based
	sSoundCue += IntToString(nSoundID);
	sSoundCue += "_m_Cue";
	USoundCue* ConversationCue = GetSoundCue(sSoundCue);

	//check if sound plays for conversations
	float deltaBetweenSounds = 0.5f;
	float timer = ConversationCue->GetDuration() + deltaBetweenSounds;
#ifdef DEBUG
	LogWarning("Current sound " + IntToString(nSoundID) + " has duration of " + FloatToString(timer - deltaBetweenSounds));
#endif // DEBUG

	GetWorldTimerManager().SetTimer(SoundCueTimerHandle, this, &ADA2UE4PlayerController::CheckIsNextLine, timer, false);

	ConversationAudioComp->Activate(true);
	ConversationAudioComp->SetSound(ConversationCue);
	ConversationAudioComp->Play(0.f);
}

USoundCue* ADA2UE4PlayerController::GetSoundCue(FString sSoundCue)
{
	ConversationCuePtr = FStringAssetReference(sSoundCue);
	bool IsValid;
	FStreamableManager& Streamable = UDA2UE4Library::GetDA2UE4Data(IsValid)->AssetLoader;
	if (ConversationCuePtr.IsPending())
	{
		const FStringAssetReference& AssetRef = ConversationCuePtr.ToStringReference();
		ConversationCuePtr = Cast<USoundCue>(Streamable.SynchronousLoad(AssetRef));
	}
	return ConversationCuePtr.Get();
}

//tests
void ADA2UE4PlayerController::TestStates()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("This is a message!"));

	UDA2UE4GameInstance* module = GetModule();

	for (TActorIterator<ADA2UE4Creature> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ADA2UE4Creature *oCreature = *ActorItr;
		if (oCreature->GetName().Contains("demo100cr_barkeep"))
		{
// 			FString r = GetAnimationResource("2A Misc 3");
// 			LogWarning("Anim Resource is " + r);

			Cast<UDA2UE4AnimInstance>(oCreature->MeshHEDComponent->GetAnimInstance())->AnimBlendLower = 0.f;
			Cast<UDA2UE4AnimInstance>(oCreature->MeshHEDComponent->GetAnimInstance())->AnimBlendUpperRight = 0.f;

// 			FString s = GetStringByStringId(396007);
// 			LogWarning("Talk String is " + s);

			//DisplayFloatyMessage(oCreature, IntToString(oCreature->INVENTORY_SLOTS.Num()) + " AaBbCc");
			//DisplayFloatyMessage(oCreature, "Hi!");

			//oCreature->GROUP = GROUP_HOSTILE;
			//oCreature->AddThreat(GetPawn(), 10.f);

// 			module->GAME_MODE = EGameMode::GM_COMBAT;
// #ifdef DEBUG
// 			LogWarning("setting game mode to combat");
// #endif

			//float f = GetDistanceBetween(GetControlledPlayer(), Creature);
			//LogError("Distance is " + FloatToString(f));
			break;
		}
	}
}
