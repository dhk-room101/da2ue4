// Fill out your copyright notice in the Description page of Project Settings.

#include "DA2UE4.h"
#include "DA2UE4Trigger.h"

#include "ldf.h"

#include "trigger_h.h"
#include "trigger_constants_h.h"

#include <string>

// Sets default values
ADA2UE4Trigger::ADA2UE4Trigger()
{
	static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("Material'/Game/Data/Art/Textures/UTC/Transparent_Mat.Transparent_Mat'"));
	if (Material.Object != NULL)
	{
		MaterialTransparent = (UMaterial*)Material.Object;
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> TriggerAsset(TEXT("StaticMesh'/Game/Data/Art/Primitives/TriggerPlane.TriggerPlane'"));
	if (TriggerAsset.Succeeded())
	{
		TriggerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TriggerPlane"));
		TriggerMesh->SetStaticMesh(TriggerAsset.Object);
		TriggerMesh->Mobility = EComponentMobility::Static;
		RootComponent = TriggerMesh;
		SetActorEnableCollision(true);

		TriggerMesh->bGenerateOverlapEvents = true;

		TriggerMesh->SetCollisionProfileName(TEXT("Custom"));
		TriggerMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		TriggerMesh->SetCollisionObjectType(ECC_WorldStatic);
		TriggerMesh->SetCollisionResponseToChannels(ECR_Ignore); //TODO expensive if commented out ?
		TriggerMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		TriggerMesh->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Overlap);
		TriggerMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		TriggerMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);
		TriggerMesh->SetRelativeLocation(FVector(0.f));

		TriggerMesh->SetMaterial(0, MaterialTransparent);
	}
}

// Called when the game starts or when spawned
void ADA2UE4Trigger::BeginPlay()
{
	Super::BeginPlay();

	TriggerMesh->OnComponentBeginOverlap.AddDynamic(this, &ADA2UE4Trigger::BeginOverlap);
}

void ADA2UE4Trigger::BeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& OverlapInfo)
{
	FString _final = GetRealName(GetName());

	HandleTrigger(GetNewHash(_final), Other, this);

#ifdef DEBUG
	LogWarning("Stepped on " + _final + " with hash " + Int64ToString(GetNewHash(_final)));

#endif // DEBUG

}