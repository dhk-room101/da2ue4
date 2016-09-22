// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "STypes.h"

#include "AIController.h"
#include "DA2UE4CreatureController.generated.h"


/**
 * 
 */
UCLASS()
class DA2UE4_API ADA2UE4CreatureController : public AAIController
{
	GENERATED_BODY()

	ADA2UE4CreatureController(const class FObjectInitializer& ObjectInitializer);

	/* Called whenever the controller possesses a character bot */
	virtual void Possess(class APawn* InPawn) override;
	virtual void UnPossess() override;

	bool SetupBehaviorTree();
	void StartBehaviorTree(APawn* InPawn);

	UPROPERTY()
		UBehaviorTree* BehaviorTree = nullptr;
	UPROPERTY()
		UBehaviorTreeComponent* BehaviorComp = nullptr;
	UPROPERTY()
		UBlackboardComponent* BlackboardComp = nullptr;

public:

	void SetActionTarget(APawn* Pawn);
	AActor* GetActionTarget();
	ECombatResult CombatTactics(AActor* aActor);

	/** Returns BehaviorComp subobject **/
	FORCEINLINE UBehaviorTreeComponent* GetBehaviorComp() const { return BehaviorComp; }
	FORCEINLINE UBlackboardComponent* GetBlackboardComp() const { return BlackboardComp; }
};
