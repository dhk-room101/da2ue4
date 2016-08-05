// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "STypes.h"
#include "DA2UE4Item.generated.h"

UCLASS()
class DA2UE4_API ADA2UE4Item : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADA2UE4Item(const class FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	//from item xml
	//ID in the BaseItem json file
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
		int32 BaseItemType; 

	//GetItemAbilityId ?? //ABILITY_INVALID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
		int32 ABILITY; 
	//linked to ItemProperties json
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
		TArray<FItemProperty> ItemProperties;
	
	//from Creature Inventory list
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
		int32 ItemURI;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
		int32 StackSize;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
		int32 Stackable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
		int32 Stealable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
		int32 Droppable;
	//INVENTORY_SLOT_INVALID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
		int32 Slot = 255; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
		int32 Infinite;
	//0/1/2?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
		int32 SetNumber; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesCombat)
		int32 PROJECTILE_OVERRIDE;
};
