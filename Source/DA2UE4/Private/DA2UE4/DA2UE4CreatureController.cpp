// Fill out your copyright notice in the Description page of Project Settings.

#include "DA2UE4.h"
#include "DA2UE4CreatureController.h"

#include "DA2UE4Creature.h"

/* AI Specific includes */
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "BehaviorTree/Composites/BTComposite_Selector.h"

//#include "Kismet/KismetSystemLibrary.h"

#include "STypes.h"
#include "ldf.h"
#include "ai_main_h.h"

ADA2UE4CreatureController::ADA2UE4CreatureController(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	BehaviorComp = ObjectInitializer.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorComp"));
	BlackboardComp = ObjectInitializer.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("BlackboardComp"));
}

void ADA2UE4CreatureController::Possess(class APawn* InPawn)
{
	Super::Possess(InPawn);

	bool result = SetupBehaviorTree();

	if (result)
	{
		ADA2UE4Creature* aCreature = Cast<ADA2UE4Creature>(InPawn);

		if (aCreature)
		{
			aCreature->CreatureSpawn();
			StartBehaviorTree(InPawn);
		}
	}
#ifdef DEBUG
	else LogWarning("SetupBehaviorTree return false!!");
#endif // DEBUG
}

void ADA2UE4CreatureController::UnPossess()
{
	Super::UnPossess();

	BehaviorComp->StopTree();
}

bool ADA2UE4CreatureController::SetupBehaviorTree()
{
#ifdef DEBUG
	LogWarning("Setting up Behavior Tree");
#endif // DEBUG

	//manual
	FName BTPath = TEXT("BehaviorTree'/Game/Data/Blueprints/BP2BT.BP2BT'");
	BehaviorTree = Cast<UBehaviorTree>(StaticLoadObject(UBehaviorTree::StaticClass(), NULL, *BTPath.ToString()));

	RunBehaviorTree(BehaviorTree);

	return true;
}

void ADA2UE4CreatureController::StartBehaviorTree(APawn* InPawn)
{
	ADA2UE4Creature* aCreature = Cast<ADA2UE4Creature>(InPawn);

	if (!aCreature->BehaviorTree)
		aCreature->BehaviorTree = BehaviorTree;

	if (aCreature->BehaviorTree->BlackboardAsset)
	{
		BlackboardComp->InitializeBlackboard(*aCreature->BehaviorTree->BlackboardAsset);

	}

	BehaviorComp->StartTree(*aCreature->BehaviorTree);
}

AActor* ADA2UE4CreatureController::GetActionTarget()
{
	if (BlackboardComp)
	{
		return Cast<ADA2UE4Creature>(BlackboardComp->GetValueAsObject(ActionTarget));
	}

	return nullptr;
}

void ADA2UE4CreatureController::SetActionTarget(APawn* NewTarget)
{
	if (BlackboardComp)	BlackboardComp->SetValueAsObject(ActionTarget, NewTarget);
}

ECombatResult ADA2UE4CreatureController::CombatTactics(AActor* aActor)
{
	//enable Combat State
	ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
	oCreature->COMBAT_STATE = TRUE_;

	//return EvaluateTactics(aActor);
	return AI_DetermineCombatRound(aActor);
	//return FALSE_;
}

