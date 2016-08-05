// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
//TODO remove DEBUG includes
#include "DA2UE4.h"
#include "DA2UE4Module.h"
#include "DA2UE4Library.h"
#include "DA2UE4Creature.h"
#include "DA2UE4CreatureController.h"
#include "DA2UE4CreatureComponent.h"
#include "DA2UE4PlayerComponent.h"
#include "ldf.h"

/* AI Include */
#include "Perception/PawnSensingComponent.h"  

//////////////////////////////////////////////////////////////////////////
// ADA2UE4Creature

ADA2UE4Creature::ADA2UE4Creature(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DA2UE4Creature = CreateDefaultSubobject<UDA2UE4CreatureComponent>(TEXT("DA2UE4CreatureComponent"));
	DA2UE4Player = CreateDefaultSubobject<UDA2UE4PlayerComponent>(TEXT("DA2UE4PlayerComponent"));

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
	CameraBoom->TargetArmLength = 1200.f;
	CameraBoom->RelativeRotation = FRotator(-45.f, 0.f, 0.f);
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level
										  // Create a camera...
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	/* Our sensing component to detect players by visibility and noise checks. */
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	PawnSensingComp->SetPeripheralVisionAngle(60.0f);
	PawnSensingComp->SightRadius = 2000;

	MeleeRingComp = CreateDefaultSubobject<USphereComponent>(TEXT("MeleeRing"));
	MeleeRingComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MeleeRingComp->SetCollisionObjectType(ECC_WorldDynamic);
	//MeleeRing->SetCollisionResponseToChannels(ECR_Ignore); //TODO expensive if commented out?
	MeleeRingComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	FVector Scale = FVector(5.0f);
	MeleeRingComp->SetWorldScale3D(Scale);

	SensedTimeOut = 2.5f;
}

void ADA2UE4Creature::BeginPlay()
{
	PrimaryActorTick.AddPrerequisite(GetMesh(), GetMesh()->PrimaryComponentTick);//bug notify queue
	
	Super::BeginPlay();

	/* This is the earliest moment we can bind our delegates to the component */
	if (PawnSensingComp)
	{
		PawnSensingComp->OnSeePawn.AddDynamic(this, &ADA2UE4Creature::OnSeePlayer);
	}

	if (MeleeRingComp)
	{
		MeleeRingComp->OnComponentBeginOverlap.AddDynamic(this, &ADA2UE4Creature::TriggerEnter);
		MeleeRingComp->OnComponentEndOverlap.AddDynamic(this, &ADA2UE4Creature::TriggerExit);
	}

	//HOME_LOCATION = this->GetActorLocation(); set it in event spawn
}

void ADA2UE4Creature::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//is the last seen time bigger than X, if yes, send Disappear event
	ADA2UE4CreatureController* AIController = Cast<ADA2UE4CreatureController>(GetController());
	if (AIController)
	{
		for (APawn* pawn : SensedPawns)
		{
			if (GetWorld()->GetTimeSeconds() - Cast<ADA2UE4Creature>(pawn)->LastSeenTime > SensedTimeOut)
			{
				LogWarning(GetName() + " stopped seeing " + pawn->GetName());
				AIController->TriggerEventPerceptionAppear(pawn, FALSE_, FALSE_, FALSE_, FALSE_);//disappear
				if (IsObjectHostile(this, Cast<ADA2UE4Creature>(pawn)))
				{
					for (FThreat threat : Threats)
					{
						if (threat.oThreat == pawn)
							Threats.Remove(threat);
					}
				}
				if (Threats.Num() == 0)
					AI_THREAT_TARGET = nullptr;//TODO last threat resets AI_THREAT_TARGET;
				SensedPawns.Remove(pawn);
			}
		}
	}
}

void ADA2UE4Creature::TriggerEnter(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	MeleeRingPawns.Add(Cast<APawn>(OtherActor));
}

void ADA2UE4Creature::TriggerExit(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	MeleeRingPawns.Remove(Cast<APawn>(OtherActor));
}

void ADA2UE4Creature::OnSeePlayer(APawn* Pawn)
{
	ADA2UE4CreatureController* AIController = Cast<ADA2UE4CreatureController>(GetController());

	/* Keep track of the time the player was last sensed in order to clear the target */
	//LastSeenTime = GetWorld()->GetTimeSeconds();

	ADA2UE4Creature* SensedPawn = Cast<ADA2UE4Creature>(Pawn);
	if (AIController)// && SensedPawn->IsAlive())
	{
		//have I seen this pawn before?
		//if yes, did the hostility changed?

		//AIController->SetTargetEnemy(SensedPawn);
		Cast<ADA2UE4Creature>(SensedPawn)->LastSeenTime = GetWorld()->GetTimeSeconds();
		//TODO handle bStealth, and event integer 1 = ability
		int32 bStealth = TRUE_;
		if (!SensedPawns.Contains(SensedPawn))
		{
#ifdef DEBUG
			LogWarning("OnSee " + SensedPawn->GetName() + " by " + this->GetName());
#endif // DEBUG
			int32 bHostile = IsObjectHostile(this, Cast<ADA2UE4Creature>(SensedPawn));
			if (bHostile) AddThreat(SensedPawn, 10.f); //default threat

			SensedPawns.Add(SensedPawn);
			AIController->TriggerEventPerceptionAppear(SensedPawn, TRUE_, bHostile, bStealth, bHostilityChanged);
		}
		else if (bHostilityChanged)
		{
#ifdef DEBUG
			LogWarning("OnSee Hostility changed " + SensedPawn->GetName() + " by " + this->GetName());
#endif // DEBUG
			bHostilityChanged = FALSE_;
			int32 bHostile = IsObjectHostile(this, Cast<ADA2UE4Creature>(SensedPawn));
			if (bHostile) AddThreat(SensedPawn, 10.f); //default threat

			AIController->TriggerEventPerceptionAppear(SensedPawn, TRUE_, bHostile, bStealth, bHostilityChanged);
		}
	}
}

void ADA2UE4Creature::AddThreat(APawn* oThreat, float fThreat)
{
	if (AI_THREAT_TARGET == nullptr)
		AI_THREAT_TARGET = oThreat;//if first threat add to AI_THREAT_TARGET;
	
	FThreat threat = FThreat();
	threat.oThreat = oThreat;
	threat.fThreat = fThreat;
	Threats.Add(threat);
}

void ADA2UE4Creature::JoinParty()
{
	DefaultRank = RANK;
	RANK = CREATURE_RANK_PLAYER;
}

void ADA2UE4Creature::LeaveParty()
{
	if (DefaultRank != -1)
		RANK = DefaultRank;
}

void ADA2UE4Creature::SetDefaultHostilities()
{
	if (GROUP == GROUP_HOSTILE || GROUP == GROUP_HOSTILE_ON_GROUND)
	{
		HostilityTargetGroup.Add(0); //GROUP_PC
		HostilityTargetGroup.Add(2); //GROUP_FRIENDLY
	}
	if (GROUP == GROUP_PC || GROUP == GROUP_FRIENDLY)
	{
		HostilityTargetGroup.Add(1); //GROUP_HOSTILE
		HostilityTargetGroup.Add(36); //GROUP_HOSTILE_ON_GROUND
	}
}
