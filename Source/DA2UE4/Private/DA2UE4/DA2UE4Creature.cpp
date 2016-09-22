// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "DA2UE4.h"
#include "DA2UE4Creature.h"
#include "DA2UE4CreatureController.h"
#include "DA2UE4MeshDataAsset.h"
#include "DA2UE4AnimInstance.h"
#include "DA2UE4PlayerController.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"

/* AI Include */
#include "Perception/PawnSensingComponent.h"

#include "DA2UE4Library.h"
#include "DA2UE4GameInstance.h"

#include "ldf.h"
#include "events_h.h"
#include "ai_constants_h.h"
#include "log_h.h"
#include "ai_main_h.h"

#include "sys_rubberband_h.h"
#include "sys_ambient_h.h"
#include "commands_h.h"
#include "wrappers_h.h"

FName ADA2UE4Creature::MeshHEDComponentName(TEXT("MeshHEDComponent"));
FName ADA2UE4Creature::MeshARMComponentName(TEXT("MeshARMComponent"));
FName ADA2UE4Creature::MeshBOOComponentName(TEXT("MeshBOOComponent"));
FName ADA2UE4Creature::MeshGLVComponentName(TEXT("MeshGLVComponent"));
FName ADA2UE4Creature::MeshHARComponentName(TEXT("MeshHARComponent"));
FName ADA2UE4Creature::MeshEYEComponentName(TEXT("MeshEYEComponent"));

ADA2UE4Creature::ADA2UE4Creature(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer
		.DoNotCreateDefaultSubobject(TEXT("CharacterMesh0")))
{
	MeshHEDComponent = CreateOptionalDefaultSubobject<USkeletalMeshComponent>(ADA2UE4Creature::MeshHEDComponentName);
	if (MeshHEDComponent != nullptr)
		InitSkeletalMeshComponent(MeshHEDComponent, false);

	MeshARMComponent = CreateOptionalDefaultSubobject<USkeletalMeshComponent>(ADA2UE4Creature::MeshARMComponentName);
	if (MeshARMComponent != nullptr)
		InitSkeletalMeshComponent(MeshARMComponent, true);

	MeshBOOComponent = CreateOptionalDefaultSubobject<USkeletalMeshComponent>(ADA2UE4Creature::MeshBOOComponentName);
	if (MeshBOOComponent != nullptr)
		InitSkeletalMeshComponent(MeshBOOComponent, true);

	MeshGLVComponent = CreateOptionalDefaultSubobject<USkeletalMeshComponent>(ADA2UE4Creature::MeshGLVComponentName);
	if (MeshGLVComponent != nullptr)
		InitSkeletalMeshComponent(MeshGLVComponent, true);

	MeshHARComponent = CreateOptionalDefaultSubobject<USkeletalMeshComponent>(ADA2UE4Creature::MeshHARComponentName);
	if (MeshHARComponent != nullptr)
		InitSkeletalMeshComponent(MeshHARComponent, true);

	MeshEYEComponent = CreateOptionalDefaultSubobject<USkeletalMeshComponent>(ADA2UE4Creature::MeshEYEComponentName);
	if (MeshEYEComponent != nullptr)
		InitSkeletalMeshComponent(MeshEYEComponent, true);

	/* Our sensing component to detect players by visibility and noise checks. */
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	PawnSensingComp->SetPeripheralVisionAngle(60.0f);
	PawnSensingComp->SightRadius = 2000;
	PawnSensingComp->HearingThreshold = 600;
	PawnSensingComp->LOSHearingThreshold = 1200;

	SenseTimeOut = 2.5f;

	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bAbsoluteRotation = true; // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 900.f;
	CameraBoom->RelativeRotation = FRotator(-45.f, 0.f, 0.f);
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create a decal in the world to show the cursor's location
	CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
	CursorToWorld->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/Data/Blueprints/M_Cursor_Decal.M_Cursor_Decal'"));
	if (DecalMaterialAsset.Succeeded())
	{
		CursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);
	}
	CursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
	CursorToWorld->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());

	static ConstructorHelpers::FObjectFinder<USkeleton> SkeletonAsset(TEXT("Skeleton'/Game/Data/Art/Anims/hm_Skeleton.hm_Skeleton'"));
	if (SkeletonAsset.Succeeded())
		Skeleton = SkeletonAsset.Object;

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ADA2UE4Creature::BeginPlay()
{
	Super::BeginPlay();

	//temp
	CursorToWorld->SetVisibility(false, true);
	CursorToWorld = nullptr;

	if (PawnSensingComp)
	{
		PawnSensingComp->OnSeePawn.AddDynamic(this, &ADA2UE4Creature::OnSeePlayer);
		PawnSensingComp->OnHearNoise.AddDynamic(this, &ADA2UE4Creature::OnHearNoise);
	}

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	//add over events
	GetCapsuleComponent()->OnClicked.AddDynamic(this, &ADA2UE4Creature::OnClick);
	GetCapsuleComponent()->OnBeginCursorOver.AddDynamic(this, &ADA2UE4Creature::BeginCursorOver);
	GetCapsuleComponent()->OnEndCursorOver.AddDynamic(this, &ADA2UE4Creature::EndCursorOver);

	PrimaryActorTick.AddPrerequisite(MeshHEDComponent, MeshHEDComponent->PrimaryComponentTick);

	//Animation = Cast<UDA2UE4AnimInstance>(MeshHEDComponent->GetAnimInstance());
	//Animation->Montage_Play(Montage);

	SetupCustomMontages();
}

void ADA2UE4Creature::Tick(float DeltaSeconds)
{
	if (CursorToWorld != nullptr)
	{
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			FHitResult TraceHitResult;
			PC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
			FVector CursorFV = TraceHitResult.ImpactNormal;
			FRotator CursorR = CursorFV.Rotation();
			CursorToWorld->SetWorldLocation(TraceHitResult.Location);
			CursorToWorld->SetWorldRotation(CursorR);
		}
	}

	UDA2UE4AnimInstance* AnimInstance = Cast<UDA2UE4AnimInstance>(MeshHEDComponent->GetAnimInstance());

	if (AnimInstance && AnimInstance->NotifyQueue.AnimNotifies.Num() > 0 
		&& GetGameMode() == EGameMode::GM_CONVERSATION && bBlend == FALSE_)
	{
		LogWarning("!");
	}
}

//clicks and mice player related
void ADA2UE4Creature::OnClick(UPrimitiveComponent* HitComp, FKey ButtonClicked)
{

	if (GetGameMode() == EGameMode::GM_COMBAT || IsObjectHostile(this, GetHero()))
	{
#ifdef DEBUG
		LogWarning("player attacks " + this->GetName());

#endif // DEBUG
	}
	else if (GetGameMode() == EGameMode::GM_EXPLORE && CONVERSATION != 0)
	{
#ifdef DEBUG
		LogWarning("clicked on " + this->GetName() + " with conv ID " + IntToString(CONVERSATION) + " | time for GM_CONVERSATION");

#endif // DEBUG
		FGameEvent ev = FGameEvent(EVENT_TYPE_CONVERSATION);
		ev = SetEventObject(ev, this);
		SignalEvent(GetHero(), ev);
	}
#ifdef DEBUG
	else LogError("unknown click action with game mode " + IntToString(static_cast<uint8>(GetGameMode())));

#endif // DEBUG
}

void ADA2UE4Creature::BeginCursorOver(UPrimitiveComponent* HitComp)
{
	if (GetCurrentWorld())
	{
		if (GROUP == GROUP_HOSTILE || GROUP == GROUP_HOSTILE_ON_GROUND)
		{
			SetOutline(STENCIL_ENEMY_OUTLINE, true);
			GetCurrentPlayerController()->HandleCursorUpdate("attack");
		}
		else if (GROUP == GROUP_FRIENDLY || GROUP == GROUP_PC)
		{
			if (GetGameMode() == EGameMode::GM_EXPLORE || GetGameMode() == EGameMode::GM_COMBAT)
			{
				SetOutline(STENCIL_FRIENDLY_OUTLINE, true);
			}
			if (GetGameMode() == EGameMode::GM_EXPLORE && CONVERSATION != 0)
			{
				GetCurrentPlayerController()->HandleCursorUpdate("conversation");
			}
		}
		else //if (GROUP == GROUP_NEUTRAL)
		{
			if (GetGameMode() == EGameMode::GM_EXPLORE || GetGameMode() == EGameMode::GM_COMBAT)
			{
				SetOutline(STENCIL_NEUTRAL_OUTLINE, true);
			}
			if (GetGameMode() == EGameMode::GM_EXPLORE && CONVERSATION != 0)
			{
				GetCurrentPlayerController()->HandleCursorUpdate("conversation");
			}
		}
	}
}

void ADA2UE4Creature::EndCursorOver(UPrimitiveComponent* HitComp)
{
	if (GetCurrentWorld())
	{
		SetOutline(0, false);
		GetCurrentPlayerController()->HandleCursorUpdate("standard");
	}
}

//perception and AI
void ADA2UE4Creature::OnSeePlayer(APawn* Pawn)
{
	ADA2UE4CreatureController* AIController = Cast<ADA2UE4CreatureController>(GetController());

	ADA2UE4Creature* SensedPawn = Cast<ADA2UE4Creature>(Pawn);
	if (AIController)// && SensedPawn->IsAlive())
	{
		//have I seen this pawn before?
		//if yes, did the hostility changed?

		Cast<ADA2UE4Creature>(SensedPawn)->LastSensedTime = GetWorld()->GetTimeSeconds();
		//TODO handle bStealth, and event integer 1 = ability
		int32 bStealth = TRUE_;
		if (!SensedPawns.Contains(SensedPawn))
		{
#ifdef DEBUG
			LogWarning(GetName() + " | OnSee " + SensedPawn->GetName() + " by " + GetName());
#endif // DEBUG
			int32 bHostile = IsObjectHostile(this, Cast<ADA2UE4Creature>(SensedPawn));
			if (bHostile) AddThreat(SensedPawn, 10.f); //default threat

			SensedPawns.Add(SensedPawn);
		}
		else if (bHostilityChanged)
		{
#ifdef DEBUG
			LogWarning(GetName() + " | OnSee Hostility changed " + SensedPawn->GetName() + " by " + GetName());
#endif // DEBUG
			bHostilityChanged = FALSE_;
			int32 bHostile = IsObjectHostile(this, Cast<ADA2UE4Creature>(SensedPawn));
			if (bHostile) AddThreat(SensedPawn, 10.f); //default threat
		}
	}
}

void ADA2UE4Creature::OnHearNoise(APawn* Pawn, const FVector& Location, float Volume)
{
	LastSensedTime = GetWorld()->GetTimeSeconds();

	ADA2UE4CreatureController* AIController = Cast<ADA2UE4CreatureController>(GetController());
	if (AIController)
	{
		AIController->SetActionTarget(Pawn);
	}
}

void ADA2UE4Creature::AddThreat(APawn* oThreat, float fThreat)
{
	ADA2UE4CreatureController* AIController = Cast<ADA2UE4CreatureController>(GetController());
	if (AIController)
	{
		AIController->SetActionTarget(oThreat);
	}

	FThreat threat = FThreat();
	threat.oThreat = oThreat;
	threat.fThreat = fThreat;
	Threats.Add(threat);
}

//meshes, materials and stencils
void ADA2UE4Creature::SetOutline(int32 nStencil, bool bEnable)
{
	MeshHEDComponent->SetRenderCustomDepth(bEnable);
	MeshHEDComponent->CustomDepthStencilValue = nStencil;
	MeshARMComponent->SetRenderCustomDepth(bEnable);
	MeshARMComponent->CustomDepthStencilValue = nStencil;
	MeshBOOComponent->SetRenderCustomDepth(bEnable);
	MeshBOOComponent->CustomDepthStencilValue = nStencil;
	MeshGLVComponent->SetRenderCustomDepth(bEnable);
	MeshGLVComponent->CustomDepthStencilValue = nStencil;
	MeshHARComponent->SetRenderCustomDepth(bEnable);
	MeshHARComponent->CustomDepthStencilValue = nStencil;
}

void ADA2UE4Creature::InitSkeletalMeshComponent(TWeakObjectPtr<class USkeletalMeshComponent> SMeshPointer, bool AttachToParent)
{
	SMeshPointer->AlwaysLoadOnClient = true;
	SMeshPointer->AlwaysLoadOnServer = true;
	SMeshPointer->bOwnerNoSee = false;
	SMeshPointer->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPose;
	SMeshPointer->bCastDynamicShadow = true;
	SMeshPointer->bAffectDynamicIndirectLighting = true;
	SMeshPointer->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	// force tick after movement component updates
	if (GetCharacterMovement())
	{
		SMeshPointer->PrimaryComponentTick.AddPrerequisite(this, GetCharacterMovement()->PrimaryComponentTick);
	}

	//SMeshPointer->AttachParent = GetCapsuleComponent();
	SMeshPointer->SetupAttachment(GetCapsuleComponent());
	static FName CollisionProfileName(TEXT("CharacterMesh"));
	SMeshPointer->SetCollisionObjectType(ECC_Pawn);
	SMeshPointer->SetCollisionProfileName(CollisionProfileName);
	SMeshPointer->bGenerateOverlapEvents = false;

	// Mesh acts as the head, as well as the parent for both animation and attachment.
	if (AttachToParent)
	{
		//SMeshPointer->AttachParent = MeshHEDComponent;
		SMeshPointer->SetupAttachment(MeshHEDComponent);
		SMeshPointer->SetMasterPoseComponent(MeshHEDComponent);
		SMeshPointer->bUseBoundsFromMasterPoseComponent = true;
	}
}

void ADA2UE4Creature::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Call to load library and setup meshes
	InitDefaultMeshes(0, 1, 2, 3, 4, 5);
}

void ADA2UE4Creature::InitDefaultMeshes(int32 HEDID, int32 ARMID, int32 BOOID, int32 GLVID, int32 HARID, int32 EYEID)
{
	LoadedMeshesDatabase = NewObject<UDA2UE4MeshDataAsset>();

	//head
	LoadedMeshesDatabase->MeshList.Add(FMeshInfo());
	LoadedMeshesDatabase->MeshList[0].MeshID = 0;
	LoadedMeshesDatabase->MeshList[0].MeshResource = FStringAssetReference("/Game/Data/Art/Meshes/UTC/hm_uhm_basa_0.hm_uhm_basa_0");
	LoadedMeshesDatabase->MeshList[0].MeshMat = FStringAssetReference("/Game/Data/Art/Textures/UTC/uh_hed_masa_Mat.uh_hed_masa_Mat");

	//arm
	LoadedMeshesDatabase->MeshList.Add(FMeshInfo());
	LoadedMeshesDatabase->MeshList[1].MeshID = 1;
	LoadedMeshesDatabase->MeshList[1].MeshResource = FStringAssetReference("/Game/Data/Art/Meshes/UTC/hm_arm_lgtd_0.hm_arm_lgtd_0");
	LoadedMeshesDatabase->MeshList[1].MeshMat = FStringAssetReference("/Game/Data/Art/Textures/UTC/pm_arm_lgtd_Mat.pm_arm_lgtd_Mat");

	//boots
	LoadedMeshesDatabase->MeshList.Add(FMeshInfo());
	LoadedMeshesDatabase->MeshList[2].MeshID = 2;
	LoadedMeshesDatabase->MeshList[2].MeshResource = FStringAssetReference("/Game/Data/Art/Meshes/UTC/hm_boo_lgtd_0.hm_boo_lgtd_0");
	LoadedMeshesDatabase->MeshList[2].MeshMat = FStringAssetReference("/Game/Data/Art/Textures/UTC/pn_boo_lgtd_Mat.pn_boo_lgtd_Mat");

	//gloves
	LoadedMeshesDatabase->MeshList.Add(FMeshInfo());
	LoadedMeshesDatabase->MeshList[3].MeshID = 3;
	LoadedMeshesDatabase->MeshList[3].MeshResource = FStringAssetReference("/Game/Data/Art/Meshes/UTC/hm_glv_lgtd_0.hm_glv_lgtd_0");
	LoadedMeshesDatabase->MeshList[3].MeshMat = FStringAssetReference("/Game/Data/Art/Textures/UTC/pn_glv_lgtd_Mat.pn_glv_lgtd_Mat");

	//hair 
	LoadedMeshesDatabase->MeshList.Add(FMeshInfo());
	LoadedMeshesDatabase->MeshList[4].MeshID = 4;
	LoadedMeshesDatabase->MeshList[4].MeshResource = FStringAssetReference("/Game/Data/Art/Meshes/UTC/hm_har_ha1a_0.hm_har_ha1a_0");
	LoadedMeshesDatabase->MeshList[4].MeshMat = FStringAssetReference("/Game/Data/Art/Textures/UTC/pn_har_blda_Mat.pn_har_blda_Mat");

	//eyes
	LoadedMeshesDatabase->MeshList.Add(FMeshInfo());
	LoadedMeshesDatabase->MeshList[5].MeshID = 5;
	LoadedMeshesDatabase->MeshList[5].MeshResource = FStringAssetReference("/Game/Data/Art/Meshes/UTC/hm_uem_basa_0.hm_uem_basa_0");
	LoadedMeshesDatabase->MeshList[5].MeshMat = FStringAssetReference("/Game/Data/Art/Textures/UTC/pn_eye_nrma_Mat.pn_eye_nrma_Mat");

	// Registers the loading requests
	ChangeHEDMeshByID(HEDID);
	ChangeARMMeshByID(ARMID);
	ChangeBOOMeshByID(BOOID);
	ChangeGLVMeshByID(GLVID);
	ChangeHARMeshByID(HARID);
	ChangeEYEMeshByID(EYEID);

	ChangeHEDMatByID(HEDID);
	ChangeARMMatByID(ARMID);
	ChangeBOOMatByID(BOOID);
	ChangeGLVMatByID(GLVID);
	ChangeHARMatByID(HARID);
	ChangeEYEMatByID(EYEID);
}

bool ADA2UE4Creature::ChangeHEDMeshByID(int32 IDCode)
{
	bool IsValid;

	if (LoadedMeshesDatabase != NULL && LoadedMeshesDatabase->MeshList.Num() >= IDCode)
	{
		TArray<FStringAssetReference> ObjToLoad;
		FStreamableManager& BaseLoader = UDA2UE4Library::GetDA2UE4Data(IsValid)->AssetLoader;
		HEDAssetToLoad = LoadedMeshesDatabase->MeshList[IDCode].MeshResource.ToStringReference();
		ObjToLoad.AddUnique(HEDAssetToLoad);
		BaseLoader.RequestAsyncLoad(ObjToLoad, FStreamableDelegate::CreateUObject(this, &ADA2UE4Creature::DoAsyncHEDMeshChange));
		return true;
	}

	return false;
}

bool ADA2UE4Creature::ChangeARMMeshByID(int32 IDCode)
{
	bool IsValid;

	// Prevents out of index access
	if (LoadedMeshesDatabase != NULL && LoadedMeshesDatabase->MeshList.Num() >= IDCode)
	{
		// Unfortunately, Asyncs just accepts arrays
		TArray<FStringAssetReference> ObjToLoad;
		// Gets our Asset Loader			
		FStreamableManager& BaseLoader = UDA2UE4Library::GetDA2UE4Data(IsValid)->AssetLoader;
		//Extracts the path/obj	
		ARMAssetToLoad = LoadedMeshesDatabase->MeshList[IDCode].MeshResource.ToStringReference();
		// Puts our loading task into array
		ObjToLoad.AddUnique(ARMAssetToLoad);
		//Assigns the delegate to the task end
		BaseLoader.RequestAsyncLoad(ObjToLoad, FStreamableDelegate::CreateUObject(this, &ADA2UE4Creature::DoAsyncARMMeshChange));
		return true;
	}

	return false;
}

bool ADA2UE4Creature::ChangeBOOMeshByID(int32 IDCode)
{
	bool IsValid;

	// Prevents out of index access
	if (LoadedMeshesDatabase != NULL && LoadedMeshesDatabase->MeshList.Num() >= IDCode)
	{
		// Unfortunately, Asyncs just accepts arrays
		TArray<FStringAssetReference> ObjToLoad;
		// Gets our Asset Loader			
		FStreamableManager& BaseLoader = UDA2UE4Library::GetDA2UE4Data(IsValid)->AssetLoader;
		//Extracts the path/obj	
		BOOAssetToLoad = LoadedMeshesDatabase->MeshList[IDCode].MeshResource.ToStringReference();
		// Puts our loading task into array
		ObjToLoad.AddUnique(BOOAssetToLoad);
		//Assigns the delegate to the task end
		BaseLoader.RequestAsyncLoad(ObjToLoad, FStreamableDelegate::CreateUObject(this, &ADA2UE4Creature::DoAsyncBOOMeshChange));
		return true;
	}

	return false;
}

bool ADA2UE4Creature::ChangeGLVMeshByID(int32 IDCode)
{
	bool IsValid;

	// Prevents out of index access
	if (LoadedMeshesDatabase != NULL && LoadedMeshesDatabase->MeshList.Num() >= IDCode)
	{
		// Unfortunately, Asyncs just accepts arrays
		TArray<FStringAssetReference> ObjToLoad;
		// Gets our Asset Loader			
		FStreamableManager& BaseLoader = UDA2UE4Library::GetDA2UE4Data(IsValid)->AssetLoader;
		//Extracts the path/obj	
		GLVAssetToLoad = LoadedMeshesDatabase->MeshList[IDCode].MeshResource.ToStringReference();
		// Puts our loading task into array
		ObjToLoad.AddUnique(GLVAssetToLoad);
		//Assigns the delegate to the task end
		BaseLoader.RequestAsyncLoad(ObjToLoad, FStreamableDelegate::CreateUObject(this, &ADA2UE4Creature::DoAsyncGLVMeshChange));
		return true;
	}

	return false;
}

bool ADA2UE4Creature::ChangeHARMeshByID(int32 IDCode)
{
	bool IsValid;

	// Prevents out of index access
	if (LoadedMeshesDatabase != NULL && LoadedMeshesDatabase->MeshList.Num() >= IDCode)
	{
		// Unfortunately, Asyncs just accepts arrays
		TArray<FStringAssetReference> ObjToLoad;
		// Gets our Asset Loader			
		FStreamableManager& BaseLoader = UDA2UE4Library::GetDA2UE4Data(IsValid)->AssetLoader;
		//Extracts the path/obj	
		HARAssetToLoad = LoadedMeshesDatabase->MeshList[IDCode].MeshResource.ToStringReference();
		// Puts our loading task into array
		ObjToLoad.AddUnique(HARAssetToLoad);
		//Assigns the delegate to the task end
		BaseLoader.RequestAsyncLoad(ObjToLoad, FStreamableDelegate::CreateUObject(this, &ADA2UE4Creature::DoAsyncHARMeshChange));
		return true;
	}

	return false;
}

bool ADA2UE4Creature::ChangeEYEMeshByID(int32 IDCode)
{
	bool IsValid;

	// Prevents out of index access
	if (LoadedMeshesDatabase != NULL && LoadedMeshesDatabase->MeshList.Num() >= IDCode)
	{
		// Unfortunately, Asyncs just accepts arrays
		TArray<FStringAssetReference> ObjToLoad;
		// Gets our Asset Loader			
		FStreamableManager& BaseLoader = UDA2UE4Library::GetDA2UE4Data(IsValid)->AssetLoader;
		//Extracts the path/obj	
		EYEAssetToLoad = LoadedMeshesDatabase->MeshList[IDCode].MeshResource.ToStringReference();
		// Puts our loading task into array
		ObjToLoad.AddUnique(EYEAssetToLoad);
		//Assigns the delegate to the task end
		BaseLoader.RequestAsyncLoad(ObjToLoad, FStreamableDelegate::CreateUObject(this, &ADA2UE4Creature::DoAsyncEYEMeshChange));
		return true;
	}

	return false;
}

bool ADA2UE4Creature::ChangeHEDMatByID(int32 IDCode)
{
	bool IsValid;

	if (LoadedMeshesDatabase != NULL && LoadedMeshesDatabase->MeshList.Num() >= IDCode)
	{
		TArray<FStringAssetReference> ObjToLoad;
		FStreamableManager& BaseLoader = UDA2UE4Library::GetDA2UE4Data(IsValid)->AssetLoader;
		HEDMatAssetToLoad = LoadedMeshesDatabase->MeshList[IDCode].MeshMat.ToStringReference();
		ObjToLoad.AddUnique(HEDMatAssetToLoad);
		BaseLoader.RequestAsyncLoad(ObjToLoad, FStreamableDelegate::CreateUObject(this, &ADA2UE4Creature::DoAsyncHEDMatChange));
		return true;
	}

	return false;
}

bool ADA2UE4Creature::ChangeARMMatByID(int32 IDCode)
{
	bool IsValid;

	if (LoadedMeshesDatabase != NULL && LoadedMeshesDatabase->MeshList.Num() >= IDCode)
	{
		TArray<FStringAssetReference> ObjToLoad;
		FStreamableManager& BaseLoader = UDA2UE4Library::GetDA2UE4Data(IsValid)->AssetLoader;
		ARMMatAssetToLoad = LoadedMeshesDatabase->MeshList[IDCode].MeshMat.ToStringReference();
		ObjToLoad.AddUnique(ARMMatAssetToLoad);
		BaseLoader.RequestAsyncLoad(ObjToLoad, FStreamableDelegate::CreateUObject(this, &ADA2UE4Creature::DoAsyncARMMatChange));
		return true;
	}

	return false;
}

bool ADA2UE4Creature::ChangeBOOMatByID(int32 IDCode)
{
	bool IsValid;

	if (LoadedMeshesDatabase != NULL && LoadedMeshesDatabase->MeshList.Num() >= IDCode)
	{
		TArray<FStringAssetReference> ObjToLoad;
		FStreamableManager& BaseLoader = UDA2UE4Library::GetDA2UE4Data(IsValid)->AssetLoader;
		BOOMatAssetToLoad = LoadedMeshesDatabase->MeshList[IDCode].MeshMat.ToStringReference();
		ObjToLoad.AddUnique(BOOMatAssetToLoad);
		BaseLoader.RequestAsyncLoad(ObjToLoad, FStreamableDelegate::CreateUObject(this, &ADA2UE4Creature::DoAsyncBOOMatChange));
		return true;
	}

	return false;
}

bool ADA2UE4Creature::ChangeGLVMatByID(int32 IDCode)
{
	bool IsValid;

	if (LoadedMeshesDatabase != NULL && LoadedMeshesDatabase->MeshList.Num() >= IDCode)
	{
		TArray<FStringAssetReference> ObjToLoad;
		FStreamableManager& BaseLoader = UDA2UE4Library::GetDA2UE4Data(IsValid)->AssetLoader;
		GLVMatAssetToLoad = LoadedMeshesDatabase->MeshList[IDCode].MeshMat.ToStringReference();
		ObjToLoad.AddUnique(GLVMatAssetToLoad);
		BaseLoader.RequestAsyncLoad(ObjToLoad, FStreamableDelegate::CreateUObject(this, &ADA2UE4Creature::DoAsyncGLVMatChange));
		return true;
	}

	return false;
}

bool ADA2UE4Creature::ChangeHARMatByID(int32 IDCode)
{
	bool IsValid;

	if (LoadedMeshesDatabase != NULL && LoadedMeshesDatabase->MeshList.Num() >= IDCode)
	{
		TArray<FStringAssetReference> ObjToLoad;
		FStreamableManager& BaseLoader = UDA2UE4Library::GetDA2UE4Data(IsValid)->AssetLoader;
		HARMatAssetToLoad = LoadedMeshesDatabase->MeshList[IDCode].MeshMat.ToStringReference();
		ObjToLoad.AddUnique(HARMatAssetToLoad);
		BaseLoader.RequestAsyncLoad(ObjToLoad, FStreamableDelegate::CreateUObject(this, &ADA2UE4Creature::DoAsyncHARMatChange));
		return true;
	}

	return false;
}

bool ADA2UE4Creature::ChangeEYEMatByID(int32 IDCode)
{
	bool IsValid;

	if (LoadedMeshesDatabase != NULL && LoadedMeshesDatabase->MeshList.Num() >= IDCode)
	{
		TArray<FStringAssetReference> ObjToLoad;
		FStreamableManager& BaseLoader = UDA2UE4Library::GetDA2UE4Data(IsValid)->AssetLoader;
		EYEMatAssetToLoad = LoadedMeshesDatabase->MeshList[IDCode].MeshMat.ToStringReference();
		ObjToLoad.AddUnique(EYEMatAssetToLoad);
		BaseLoader.RequestAsyncLoad(ObjToLoad, FStreamableDelegate::CreateUObject(this, &ADA2UE4Creature::DoAsyncEYEMatChange));
		return true;
	}

	return false;
}

// DELEGATE - Do the Async HED Change
void ADA2UE4Creature::DoAsyncHEDMeshChange()
{
	check(HEDAssetToLoad.ResolveObject() != nullptr)
	{
		// Creates a pointer to store the loaded object
		UObject* NewHEDMesh = HEDAssetToLoad.ResolveObject();
		// Casts and assigns
		MeshHEDComponent->SetSkeletalMesh(Cast<USkeletalMesh>(NewHEDMesh));

		MeshHEDComponent->SetRelativeLocationAndRotation(FVector(0, 0, -96), FRotator(0, 90, 0));

		// ******** UPDATE 4.11.2  Check Notes *****
		MeshARMComponent->SetMasterPoseComponent(MeshHEDComponent);
		MeshBOOComponent->SetMasterPoseComponent(MeshHEDComponent);
		MeshGLVComponent->SetMasterPoseComponent(MeshHEDComponent);
		MeshHARComponent->SetMasterPoseComponent(MeshHEDComponent);
		MeshEYEComponent->SetMasterPoseComponent(MeshHEDComponent);
	}
}

void ADA2UE4Creature::DoAsyncARMMeshChange()
{
	check(ARMAssetToLoad.ResolveObject() != nullptr)
	{
		UObject* NewARMMesh = ARMAssetToLoad.ResolveObject();
		MeshARMComponent->SetSkeletalMesh(Cast<USkeletalMesh>(NewARMMesh));
	}
}

void ADA2UE4Creature::DoAsyncBOOMeshChange()
{
	check(BOOAssetToLoad.ResolveObject() != nullptr)
	{
		UObject* NewBOOMesh = BOOAssetToLoad.ResolveObject();
		MeshBOOComponent->SetSkeletalMesh(Cast<USkeletalMesh>(NewBOOMesh));
	}
}

void ADA2UE4Creature::DoAsyncGLVMeshChange()
{
	check(GLVAssetToLoad.ResolveObject() != nullptr)
	{
		UObject* NewGLVMesh = GLVAssetToLoad.ResolveObject();
		MeshGLVComponent->SetSkeletalMesh(Cast<USkeletalMesh>(NewGLVMesh));
	}
}

void ADA2UE4Creature::DoAsyncHARMeshChange()
{
	check(HARAssetToLoad.ResolveObject() != nullptr)
	{
		UObject* NewHARMesh = HARAssetToLoad.ResolveObject();
		MeshHARComponent->SetSkeletalMesh(Cast<USkeletalMesh>(NewHARMesh));
	}
}

void ADA2UE4Creature::DoAsyncEYEMeshChange()
{
	check(EYEAssetToLoad.ResolveObject() != nullptr)
	{
		UObject* NewEYEMesh = EYEAssetToLoad.ResolveObject();
		MeshEYEComponent->SetSkeletalMesh(Cast<USkeletalMesh>(NewEYEMesh));
	}
}

void ADA2UE4Creature::DoAsyncHEDMatChange()
{
	check(HEDMatAssetToLoad.ResolveObject() != nullptr)
	{
		UObject* NewHEDMat = HEDMatAssetToLoad.ResolveObject();
		UMaterialInstanceDynamic* NewHEDMatDynamic = UMaterialInstanceDynamic::Create(Cast<UMaterial>(NewHEDMat), MeshHEDComponent);//;
		MeshHEDComponent->SetMaterial(0, NewHEDMatDynamic);
	}
}

void ADA2UE4Creature::DoAsyncARMMatChange()
{
	check(ARMMatAssetToLoad.ResolveObject() != nullptr)
	{
		UObject* NewARMMat = ARMMatAssetToLoad.ResolveObject();
		UMaterialInstanceDynamic* NewARMMatDynamic = UMaterialInstanceDynamic::Create(Cast<UMaterial>(NewARMMat), MeshARMComponent);//;
		MeshARMComponent->SetMaterial(0, NewARMMatDynamic);
	}
}

void ADA2UE4Creature::DoAsyncBOOMatChange()
{
	check(BOOMatAssetToLoad.ResolveObject() != nullptr)
	{
		UObject* NewBOOMat = BOOMatAssetToLoad.ResolveObject();
		UMaterialInstanceDynamic* NewBOOMatDynamic = UMaterialInstanceDynamic::Create(Cast<UMaterial>(NewBOOMat), MeshBOOComponent);//;
		MeshBOOComponent->SetMaterial(0, NewBOOMatDynamic);
	}
}

void ADA2UE4Creature::DoAsyncGLVMatChange()
{
	check(GLVMatAssetToLoad.ResolveObject() != nullptr)
	{
		UObject* NewGLVMat = GLVMatAssetToLoad.ResolveObject();
		UMaterialInstanceDynamic* NewGLVMatDynamic = UMaterialInstanceDynamic::Create(Cast<UMaterial>(NewGLVMat), MeshGLVComponent);//;
		MeshGLVComponent->SetMaterial(0, NewGLVMatDynamic);
	}
}

void ADA2UE4Creature::DoAsyncHARMatChange()
{
	check(HARMatAssetToLoad.ResolveObject() != nullptr)
	{
		UObject* NewHARMat = HARMatAssetToLoad.ResolveObject();
		UMaterialInstanceDynamic* NewHARMatDynamic = UMaterialInstanceDynamic::Create(Cast<UMaterial>(NewHARMat), MeshHARComponent);//;
		MeshHARComponent->SetMaterial(0, NewHARMatDynamic);
	}
}

void ADA2UE4Creature::DoAsyncEYEMatChange()
{
	check(EYEMatAssetToLoad.ResolveObject() != nullptr)
	{
		UObject* NewEYEMat = EYEMatAssetToLoad.ResolveObject();
		UMaterialInstanceDynamic* NewEYEMatDynamic = UMaterialInstanceDynamic::Create(Cast<UMaterial>(NewEYEMat), MeshEYEComponent);//;
		MeshEYEComponent->SetMaterial(0, NewEYEMatDynamic);
	}
}

//default spawn
void ADA2UE4Creature::CreatureSpawn()
{
	//DEBUG
	Abilities.Add(FAbility(617));

	// Creatures with a ranged weapon in their default slot, prefer ranged weapon
	if (_AI_GetWeaponSetEquipped(this) == AI_WEAPON_SET_RANGED)
		_AI_SetFlag(this, AI_FLAG_PREFERS_RANGED, TRUE_);

	// -----------------------------------------------------------------
	// Force default item equip from the 2da
	// -----------------------------------------------------------------

	int32 nAppType = GetAppearanceType(this);
	EquipDefaultItem(this, nAppType, INVENTORY_SLOT_MAIN, "DefaultWeapon");
	EquipDefaultItem(this, nAppType, INVENTORY_SLOT_CHEST, "DefaultArmor");

	//give it a shield for tests //DHK
	Log_Trace(LOG_CHANNEL_EVENTS, "CreatureSpawn", "Default creature item for this appearance: [gen_im_arm_shd_kit_met] for inventory slot: " + IntToString(INVENTORY_SLOT_OFFHAND));
	AActor* shield = CreateItemOnObject("gen_im_arm_shd_kit_met", this);
	EquipItem(this, shield, INVENTORY_SLOT_OFFHAND);

	// Store start location for homing "rubberband" system
	Rubber_SetHome(this);

	// Check whether ambient behavior should start on spawn.
	Ambient_SpawnStart(this);

	// If has stealth and in combat and hostile to the player then trigger stealth
	if (HasAbility(this, ABILITY_TALENT_STEALTH) && GetCombatState(this) == FALSE_
		&& GetGroupHostility(GROUP_PC, GetGroupId(this)) == TRUE_)
	{
#ifdef DEBUG
		Log_Trace_AI(this, "creature_core", "TRIGGERING STEALTH");
#endif
		Ambient_Stop(this);
		FCommand cStealth = CommandUseAbility(ABILITY_TALENT_STEALTH, this);
		WR_AddCommand(this, cStealth);
	}

	// -----------------------------------------------------------------
	// Creature with roaming enabled.
	// -----------------------------------------------------------------           //
	float fRoamDistance = ROAM_DISTANCE;
	if (fRoamDistance > 25.0f * SCALE_MULTIPLIER)
	{
		SetRoamRadius(this, fRoamDistance * SCALE_MULTIPLIER);
	}

}

void ADA2UE4Creature::EquipDefaultItem(AActor* aActor, int32 nAppType, int32 nTargetSlot, FString s2daColumnName)
{
	// Equip default creature item based on appearance (only if none equipped yet)
	AActor* oItem = GetItemInEquipSlot(nTargetSlot, aActor);
	FString rDefaultCreatureItem;
	AActor* oDefaultCreatureoItem;
	FString sItem;
	if (!IsObjectValid(oItem))
	{
		rDefaultCreatureItem = GetM2DAString(TABLE_APPEARANCE, s2daColumnName, nAppType);
		if (rDefaultCreatureItem == "****")
			rDefaultCreatureItem = sItem = s2daColumnName;
		else sItem = rDefaultCreatureItem;
#ifdef DEBUG
		Log_Trace(LOG_CHANNEL_EVENTS, "CreatureSpawn", "Default creature item for this appearance: [" + sItem + "] for inventory slot: " +
			IntToString(nTargetSlot));
#endif
		if (sItem != "")
		{
			oDefaultCreatureoItem = CreateItemOnObject(rDefaultCreatureItem, aActor);
			SetItemDroppable(oDefaultCreatureoItem, FALSE_);
			if (IsObjectValid(oDefaultCreatureoItem))
			{
#ifdef DEBUG
				Log_Trace(LOG_CHANNEL_EVENTS, "CreatureSpawn", "Creating default item on creature");
#endif
				EquipItem(aActor, oDefaultCreatureoItem, nTargetSlot);
			}
		}
	}
}

void ADA2UE4Creature::StopCustomMontages()
{
	UDA2UE4AnimInstance* AnimInstance = Cast<UDA2UE4AnimInstance>(MeshHEDComponent->GetAnimInstance());
	AnimInstance->StopCustomMontages();
}

void ADA2UE4Creature::SetupCustomMontages()
{
	UDA2UE4AnimInstance* AnimInstance = Cast<UDA2UE4AnimInstance>(MeshHEDComponent->GetAnimInstance());
	AnimInstance->SetupCustomMontages();
}