// Fill out your copyright notice in the Description page of Project Settings.

#include "DA2UE4.h"
#include "DA2UE4AreaTransition.h"

#include "DA2UE4PlayerController.h"
#include "DA2UE4Creature.h"
#include "DA2UE4GameInstance.h"
#include "DA2UE4Library.h"

#include "ldf.h"

// Sets default values
ADA2UE4AreaTransition::ADA2UE4AreaTransition()
{
	static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("Material'/Game/Data/Art/Textures/UTC/Transparent_Mat.Transparent_Mat'"));
	if (Material.Object != NULL)
	{
		MaterialTransparent = (UMaterial*)Material.Object;
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> AreaTransitionAsset(TEXT("StaticMesh'/Game/Data/Art/Primitives/AreaTransitionPlane.AreaTransitionPlane'"));
	if (AreaTransitionAsset.Succeeded())
	{
		AreaTransitionMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AreaTransitionPlane"));
		AreaTransitionMesh->SetStaticMesh(AreaTransitionAsset.Object);
		AreaTransitionMesh->Mobility = EComponentMobility::Static;
		RootComponent = AreaTransitionMesh;
		SetActorEnableCollision(true);

		AreaTransitionMesh->bGenerateOverlapEvents = true;

		AreaTransitionMesh->SetCollisionProfileName(TEXT("Custom"));
		AreaTransitionMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaTransitionMesh->SetCollisionObjectType(ECC_WorldStatic);
		AreaTransitionMesh->SetCollisionResponseToChannels(ECR_Ignore); //TODO expensive if commented out ?
		AreaTransitionMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		AreaTransitionMesh->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Overlap);
		AreaTransitionMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		AreaTransitionMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);
		AreaTransitionMesh->SetRelativeLocation(FVector(0.f));

		AreaTransitionMesh->SetMaterial(0, MaterialTransparent);
	}
}

// Called when the game starts or when spawned
void ADA2UE4AreaTransition::BeginPlay()
{
	Super::BeginPlay();

	AreaTransitionMesh->OnClicked.AddDynamic(this, &ADA2UE4AreaTransition::OnClick);
	AreaTransitionMesh->OnBeginCursorOver.AddDynamic(this, &ADA2UE4AreaTransition::BeginCursorOver);
	AreaTransitionMesh->OnEndCursorOver.AddDynamic(this, &ADA2UE4AreaTransition::EndCursorOver);
	AreaTransitionMesh->OnComponentBeginOverlap.AddDynamic(this, &ADA2UE4AreaTransition::BeginOverlap);
}

//clicks and mice player related
void ADA2UE4AreaTransition::OnClick(UPrimitiveComponent* HitComp, FKey ButtonClicked)
{
	LogWarning("AT Clicked!");
}

void ADA2UE4AreaTransition::BeginCursorOver(UPrimitiveComponent* HitComp)
{
	if (GetModule())
	{
		if (GetGameMode() == EGameMode::GM_EXPLORE)
			GetCurrentPlayerController()->HandleCursorUpdate("area_transition");
	}
}

void ADA2UE4AreaTransition::EndCursorOver(UPrimitiveComponent* HitComp)
{
	if (GetModule())
		GetCurrentPlayerController()->HandleCursorUpdate("standard");
}

void ADA2UE4AreaTransition::BeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& OverlapInfo)
{
	if (Cast<ADA2UE4Creature>(Other) == GetActorFromName(GetModule()->PLAYER_LEADER_STORE) && GetGameMode() == EGameMode::GM_EXPLORE)
	{
		LogWarning(OverlappedComp->GetName() + " Overlap with " + Other->GetName());
		LogWarning("Heading to area " + tArea.ToString() + " and waypoint " + tWaypoint.ToString()); //FName(*TheString)
		if (GetModule())
		{
			GetModule()->BeginLoadingScreen("Loading");
			GetModule()->WAYPOINT = tWaypoint;
			GetModule()->GAME_INIT_TYPE = 1; //LOAD
		}

		bool IsValid;
		UDA2UE4Library::GetDA2UE4Data(IsValid)->DA2UE4CurrentWorld = nullptr;
		UGameplayStatics::OpenLevel(GetWorld(), tArea);
	}
}