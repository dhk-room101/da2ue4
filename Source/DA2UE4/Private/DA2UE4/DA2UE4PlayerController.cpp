// Fill out your copyright notice in the Description page of Project Settings.

#include "DA2UE4.h"
#include "DA2UE4Module.h"
#include "DA2UE4Creature.h"
#include "DA2UE4PlayerController.h"
#include "DA2UE4Library.h"
#include "DA2UE4Module.h"
#include "DA2UE4AnimInstance.h"

#include "AI/Navigation/NavigationSystem.h"

#include "Blueprint/UserWidget.h"
#include "WidgetTree.h"
#include "CanvasPanel.h"
#include "CanvasPanelSlot.h"
#include "TextWidgetTypes.h"
#include "TextBlock.h"
#include "UserWidgetFloaty.h"

//DEBUG
#include "utility_h.h"
#include "events_h.h"

ADA2UE4PlayerController::ADA2UE4PlayerController(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

// Called when the game starts
void ADA2UE4PlayerController::BeginPlay()
{
	Super::BeginPlay();

	GetControlledPlayer()->PlayerState->PlayerName = "Player";

#ifdef DEBUG
	UE_LOG(LogTemp, Warning, TEXT("Player Controller %s"), *(GetControlledPlayer()->PlayerState->PlayerName));
#endif

	// TODO convert to EVENT

	//when spawn, update the singleton world to match current
	bool IsValid;

	if (GetWorld() != GetCurrentWorld())
	{
		UDA2UE4Library::GetDA2UE4Data(IsValid)->DA2UE4CurrentWorld = GetWorld();
#ifdef DEBUG
		UE_LOG(LogTemp, Warning, TEXT("no world in singleton, setting in %s"), *(GetWorld()->GetName()));
#endif
	}

	//initialize module
	if (!UDA2UE4Library::GetDA2UE4Data(IsValid)->DA2UE4CurrentModule)
	{
		UDA2UE4Library::GetDA2UE4Data(IsValid)->DA2UE4CurrentModule = NewObject<ADA2UE4Module>();
	}

	//when spawn set player character in global instance to party leader store if not already
	ADA2UE4Module* module = UDA2UE4Library::GetDA2UE4Data(IsValid)->DA2UE4CurrentModule;
	if (!(module->PLAYER_LEADER_STORE))
	{
		module->PLAYER_LEADER_STORE = GetControlledPlayer();
#ifdef DEBUG
		UE_LOG(LogTemp, Warning, TEXT("no player leader, setting in %s"), *(GetWorld()->GetName()));
#endif
	}

	//set game mode explore if currently invalid
	if (module->GAME_MODE == GM_INVALID)
		module->GAME_MODE = GM_EXPLORE;

	//add player to party
	module->Party.Add(GetControlledPlayer());

	//set PC to GROUP_PC/0
	SetGroupId(GetControlledPlayer(), 0);
#ifdef DEBUG
	UE_LOG(LogTemp, Warning, TEXT("Set PC to GROUP_PC/0"));
#endif

	//add hostility defaults
	GetControlledPlayer()->HostilityTargetGroup.Add(1); //GROUP_HOSTILE
	GetControlledPlayer()->HostilityTargetGroup.Add(36); //GROUP_HOSTILE_ON_GROUND


	GetControlledPlayer()->JoinParty();

	//trigger EVENT_TYPE_SPAWN
	FGameEvent ev = Event(EVENT_TYPE_SPAWN);
	ev = SetEventObject(ev, 0, this);
	ev = SetEventObject(ev, 0, GetControlledPlayer());
	SignalEvent(GetControlledPlayer(), ev);

	//TODO Initialize player properties here
	LogWarning("Player: Initialize player properties, such as RANK etc!!");
}

// Called every frame
void ADA2UE4PlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (int32 i = 0; i < floaties.Num(); i++)
	{
		//get current floaty
		//UTextBlock* floaty = Cast<UTextBlock>(root->GetChildAt(i));
		UUserWidgetFloaty* floaty = floaties[i];
		if (GetWorld()->GetTimeSeconds() > floaty->fTimestamp + floaty->fDuration)
		{
			//remove
			floaty->RemoveFromViewport();
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

			ProjectWorldLocationToScreen(floaty->vPosition, ScreenLocation);
			floaty->SetPositionInViewport(ScreenLocation);
		}
	}
}

ADA2UE4Creature* ADA2UE4PlayerController::GetControlledPlayer() const
{
	return Cast<ADA2UE4Creature>(GetPawn());
}

// void ADA2UE4PlayerController::CreateWidgetFloaty(AActor* oCreature, FString sMessage, int32 nStyle, int32 nColor, float fDuration)
// {
// 	UTextRenderComponent* textRender = oCreature->FindComponentByClass<class UTextRenderComponent>();
// 	if(textRender)
// 		textRender->SetText(FText::FromString(sMessage));
// }

void ADA2UE4PlayerController::CreateWidgetFloaty(AActor* oCreature, FString sMessage, int32 nStyle, int32 nColor, float fDuration)
{
	UUserWidgetFloaty* FloatyParent = nullptr;
	if (wGameUI)
	{
		FloatyParent = CreateWidget<UUserWidgetFloaty>(this, wGameUI);

		if (FloatyParent)
		{
			FloatyParent->AddToViewport();
			floaties.Add(FloatyParent);
		}
	}

	//create Text
	UTextBlock* textWidget = NewObject<UTextBlock>(this, UTextBlock::StaticClass());
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
	FloatyParent->oCreature = oCreature;
	//timestamp and duration
	FloatyParent->fTimestamp = GetWorld()->GetTimeSeconds();
	FloatyParent->fDuration = fDuration;
	//set initial position relative to owner
	float BoundingRadius = oCreature->GetRootComponent()->Bounds.SphereRadius;
	FloatyParent->vPosition = oCreature->GetActorLocation() + *(new FVector(10, -35, BoundingRadius));
	//get root
	UCanvasPanel* canvas = Cast<UCanvasPanel>(FloatyParent->WidgetTree->RootWidget);
	//add text to root
	canvas->AddChild(Cast<UWidget>(textWidget));
}

//navigation
void ADA2UE4PlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// keep updating the destination every tick while desired
	if (bMoveToMouseCursor)
	{
		MoveToMouseCursor();
	}
}

void ADA2UE4PlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	//DHK
	InputComponent->BindAction("TestStates", IE_Pressed, this, &ADA2UE4PlayerController::TestStates);

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &ADA2UE4PlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &ADA2UE4PlayerController::OnSetDestinationReleased);

	// support touch devices 
	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ADA2UE4PlayerController::MoveToTouchLocation);
	InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &ADA2UE4PlayerController::MoveToTouchLocation);
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
	APawn* const Pawn = GetPawn();
	if (Pawn)
	{
		UNavigationSystem* const NavSys = GetWorld()->GetNavigationSystem();
		float const Distance = FVector::Dist(DestLocation, Pawn->GetActorLocation());

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

void ADA2UE4PlayerController::TestStates()
{
	//goes hostile
#ifdef DEBUG
	UE_LOG(LogTemp, Warning, TEXT("Goes hostile"));
#endif
	UT_TeamGoesHostile(3);

	for (TActorIterator<ADA2UE4Creature> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ADA2UE4Creature *Creature = *ActorItr;
		if (Creature->GetName().Contains("Bandit1"))
		{
			DisplayFloatyMessage(Creature, "Hello");
			float f = GetDistanceBetween(GetControlledPlayer(), Creature);
			LogError("Distance is " + FloatToString(f));
		}
	}

	//Mesh->GetAnimInstance()->GetCurrentStateName(0);
	USkeletalMeshComponent* mesh = GetControlledPlayer()->GetMesh();
	UAnimInstance* ai = mesh->GetAnimInstance();

	UDA2UE4AnimInstance* AnimInstanceRef = Cast<UDA2UE4AnimInstance>(mesh->GetAnimInstance());
	//FAnimNode_AssetPlayerBase* fff;
	//fff = AnimInstanceRef->GetAssetFromState();
	//UAnimInstance* aaa = fff->GetAnimAsset();
	
	LogError("State: " + ai->GetCurrentStateName(0).ToString());
	for (int32 i = 0; i < ai->NotifyQueue.AnimNotifies.Num(); i++)
	{
		FAnimNotifyEvent a = *ai->NotifyQueue.AnimNotifies[i];
		LogError("what what!!");
	}

	//FAnimNode_AssetPlayerBase* AssetPlayer = ai->GetRelevantAssetPlayerFromState(0, 0);
	//AssetPlayer->GetAnimAsset()->GetAllAnimationSequencesReferred();


	//ai->ActiveAnimNotifyState;

	/*//current world
	#ifdef DEBUG
	UE_LOG(LogTemp, Warning, TEXT("Current world %s"), *(GetCurrentWorld()->GetName()))
	#endif*/
}