// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "DA2UE4Trigger.generated.h"

UCLASS()
class DA2UE4_API ADA2UE4Trigger : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADA2UE4Trigger();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
		UStaticMeshComponent* TriggerMesh = nullptr;

	UPROPERTY()
		UMaterial* MaterialTransparent = nullptr;

protected:

	UFUNCTION()
		void BeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& OverlapInfo);

};
