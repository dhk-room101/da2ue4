// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "DA2UE4AreaTransition.generated.h"

UCLASS()
class DA2UE4_API ADA2UE4AreaTransition : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADA2UE4AreaTransition();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
		UStaticMeshComponent* AreaTransitionMesh = nullptr;

	UPROPERTY()
		UMaterial* MaterialTransparent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area Transition")
		FName tArea; //Transition to area

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area Transition")
		FName tWaypoint; //Transition to waypoint	


protected:
	UFUNCTION()
		void BeginCursorOver(UPrimitiveComponent* HitComp);

	UFUNCTION()
		void EndCursorOver(UPrimitiveComponent* HitComp);

	UFUNCTION()
		void OnClick(UPrimitiveComponent* HitComp, FKey ButtonClicked = EKeys::LeftMouseButton);

	UFUNCTION()
		void BeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& OverlapInfo);
};
