// Fill out your copyright notice in the Description page of Project Settings.

#include "DA2UE4.h"
#include "DA2UE4AnimInstance.h"

#include "DA2UE4Creature.h"
#include "ldf.h"

#include "Animation/AnimMontage.h"

void UDA2UE4AnimInstance::NativeInitializeAnimation()
{
	int32 nRandom = FMath::RandRange(6, 11);
	GetSkelMeshComponent()->GetOwner()->GetWorldTimerManager().SetTimer(TimerHandleWeightshift, this, &UDA2UE4AnimInstance::HandleIdleWeightshift, nRandom, false);
}

void UDA2UE4AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(TryGetPawnOwner());

	if (oCreature)
	{
		//bIsFalling = Pawn->GetMovementComponent()->IsFalling();
		int32 i = oCreature->COMBAT_STATE;
		bIsCombatState = (i == 0 ? false : true);

		fMovementSpeed = oCreature->GetVelocity().Size();

		if (Montage_IsPlaying(oCreature->AnimMontageDefault))
			HandleMontageDefault();
		if (Montage_IsPlaying(oCreature->AnimMontageLower))
			HandleMontageLower();
		if (Montage_IsPlaying(oCreature->AnimMontageUpperRight))
			HandleMontageUpperRight();
		if (Montage_IsPlaying(oCreature->AnimMontageUpperLeft))
			HandleMontageUpperLeft();
	}
}

void UDA2UE4AnimInstance::SetAnimationTimer(int32 nSlot, float fLength)
{
	switch (nSlot)
	{
	case SLOT_DEFAULT:
	{
		GetSkelMeshComponent()->GetOwner()->GetWorldTimerManager().SetTimer(TimerHandleDefault, this, &UDA2UE4AnimInstance::ResetDefaultSlot, fLength, false);
		break;
	}
	case SLOT_LOWER:
	{
		GetSkelMeshComponent()->GetOwner()->GetWorldTimerManager().SetTimer(TimerHandleLower, this, &UDA2UE4AnimInstance::ResetLowerSlot, fLength, false);
		break;
	}
	case SLOT_UPPER_RIGHT:
	{
		GetSkelMeshComponent()->GetOwner()->GetWorldTimerManager().SetTimer(TimerHandleUpperRight, this, &UDA2UE4AnimInstance::ResetUpperRightSlot, fLength, false);
		break;
	}
	case SLOT_UPPER_LEFT:
	{
		GetSkelMeshComponent()->GetOwner()->GetWorldTimerManager().SetTimer(TimerHandleUpperLeft, this, &UDA2UE4AnimInstance::ResetUpperLeftSlot, fLength, false);
		break;
	}
	default:
#ifdef DEBUG
		LogError("Unknown animation slot");
#endif // DEBUG
		break;
	}
}

void UDA2UE4AnimInstance::ResetDefaultSlot()
{
	AnimBlendDefault = 0;
	Cast<ADA2UE4Creature>(GetOwningActor())->AnimMontageDefault->SlotAnimTracks[0].AnimTrack.AnimSegments.Empty();
}

void UDA2UE4AnimInstance::ResetLowerSlot()
{
	AnimBlendLower = 0;
	ADA2UE4Creature* creature = Cast<ADA2UE4Creature>(GetOwningActor());
	creature->AnimMontageLower->SlotAnimTracks[0].AnimTrack.AnimSegments.Empty();
}

void UDA2UE4AnimInstance::ResetUpperRightSlot()
{
	AnimBlendUpperRight = 0;
	Cast<ADA2UE4Creature>(GetOwningActor())->AnimMontageUpperRight->SlotAnimTracks[0].AnimTrack.AnimSegments.Empty();
}

void UDA2UE4AnimInstance::ResetUpperLeftSlot()
{
	AnimBlendUpperLeft = 0;
	Cast<ADA2UE4Creature>(GetOwningActor())->AnimMontageUpperLeft->SlotAnimTracks[0].AnimTrack.AnimSegments.Empty();
}

void UDA2UE4AnimInstance::HandleMontageDefault()
{
	ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(TryGetPawnOwner());

	float LenghtDefault = oCreature->AnimMontageDefault->SequenceLength;

	if (Montage_GetPosition(oCreature->AnimMontageDefault) < LenghtDefault / 3 &&
		AnimBlendDefault < AnimBlendInDefault)
	{
		AnimBlendDefault += 0.005;
	}

	if (Montage_IsPlaying(oCreature->AnimMontageDefault))
	{
		float length = Montage_GetPosition(oCreature->AnimMontageDefault);
		if (length > LenghtDefault - LenghtDefault / 3)
		{
			if (AnimBlendDefault > 0)
			{
				AnimBlendDefault -= 0.005;
			}
		}
	}
}

void UDA2UE4AnimInstance::HandleMontageLower()
{
	ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(TryGetPawnOwner());

	float LenghtLower = oCreature->AnimMontageLower->SequenceLength;

	if (Montage_GetPosition(oCreature->AnimMontageLower) < LenghtLower / 3 &&
		AnimBlendLower < AnimBlendInLower)
	{
		AnimBlendLower += 0.005;
	}

	if (Montage_IsPlaying(oCreature->AnimMontageLower))
	{
		float length = Montage_GetPosition(oCreature->AnimMontageLower);
		if (length > LenghtLower - LenghtLower / 3)
		{
			if (AnimBlendLower > 0)
			{
				AnimBlendLower -= 0.005;
			}
		}
	}
}

void UDA2UE4AnimInstance::HandleMontageUpperRight()
{
	ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(TryGetPawnOwner());

	float LenghtUpperRight = oCreature->AnimMontageUpperRight->SequenceLength;

	if (Montage_GetPosition(oCreature->AnimMontageUpperRight) < LenghtUpperRight / 3 &&
		AnimBlendUpperRight < AnimBlendInUpperRight)
	{
		AnimBlendUpperRight += 0.005;
	}

	if (Montage_IsPlaying(oCreature->AnimMontageUpperRight))
	{
		float length = Montage_GetPosition(oCreature->AnimMontageUpperRight);
		if (length > LenghtUpperRight - LenghtUpperRight / 3)
		{
			if (AnimBlendUpperRight > 0)
			{
				AnimBlendUpperRight -= 0.005;
			}
		}
	}
}

void UDA2UE4AnimInstance::HandleMontageUpperLeft()
{
	ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(TryGetPawnOwner());

	float LenghtUpperLeft = oCreature->AnimMontageUpperLeft->SequenceLength;

	if (Montage_GetPosition(oCreature->AnimMontageUpperLeft) < LenghtUpperLeft / 3 &&
		AnimBlendUpperLeft < AnimBlendInUpperLeft)
	{
		AnimBlendUpperLeft += 0.005;
	}

	if (Montage_IsPlaying(oCreature->AnimMontageUpperLeft))
	{
		float length = Montage_GetPosition(oCreature->AnimMontageUpperLeft);
		if (length > LenghtUpperLeft - LenghtUpperLeft / 3)
		{
			if (AnimBlendUpperLeft > 0)
			{
				AnimBlendUpperLeft -= 0.005;
			}
		}
	}
}

void UDA2UE4AnimInstance::HandleIdleWeightshift()
{
	//TODO idle combat anim
	ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(TryGetPawnOwner());

	fMovementSpeed = oCreature->GetVelocity().Size();
	if (fMovementSpeed == 0 /*idle*/ &&
		!Montage_IsPlaying(oCreature->AnimMontageLower))
	{

		oCreature->AnimSequenceIdle = GetConvAnimation(GetIdleAnimation());
		if (oCreature->AnimSequenceIdle && oCreature->AnimMontageLower)
		{
			FSlotAnimationTrack slot = oCreature->AnimMontageLower->SlotAnimTracks[0];

			FAnimSegment Segment;
			Segment.AnimReference = oCreature->AnimSequenceIdle;
			Segment.AnimStartTime = 0;
			Segment.StartPos = 0;
			Segment.AnimEndTime = oCreature->AnimSequenceIdle->SequenceLength;
			Segment.AnimPlayRate = 1;
			Segment.LoopingCount = 1;
			AnimBlendInLower = 1;
			SetAnimationTimer(SLOT_LOWER, Segment.AnimEndTime / Segment.AnimPlayRate);

			slot.AnimTrack.AnimSegments.Add(Segment);
#ifdef DEBUG
			LogWarning(oCreature->GetName() + ": Current slot is " + slot.SlotName.ToString() + " with animation " + oCreature->AnimSequenceIdle->GetName());
			LogWarning("Current anim segments num is " + IntToString(slot.AnimTrack.AnimSegments.Num()));

#endif // DEBUG

			oCreature->AnimMontageLower->SlotAnimTracks[0] = slot;

			//Montage_SetPlayRate(oCreature->AnimMontageLower, animLine.Speed);
			oCreature->AnimMontageLower->SequenceLength = Segment.AnimEndTime / Segment.AnimPlayRate;// +0.33f;
			Montage_Play(oCreature->AnimMontageLower);
			//montages.Add(oCreature->AnimMontageLower);
		}
	}

	//reset
	int32 nRandom = FMath::RandRange(6, 11);
	GetSkelMeshComponent()->GetOwner()->GetWorldTimerManager().SetTimer(TimerHandleWeightshift, this, &UDA2UE4AnimInstance::HandleIdleWeightshift, nRandom, false);
}

FString UDA2UE4AnimInstance::GetIdleAnimation()
{
	int32 nWeightshiftID = FMath::RandRange(1, 6); //_X
	int32 nWeightshiftType = FMath::RandRange(1, 3); //a,b,c
	FString sWeightshiftType;
	FString sAnimation;

	switch (nWeightshiftType)
	{
	case 1: sWeightshiftType = "a"; break;
	case 2: sWeightshiftType = "b"; break;
	case 3: sWeightshiftType = "c"; break;
	default:
		break;
	}

#ifdef DEBUG
	LogWarning("idle animation to be played is mh_po_tw_" + sWeightshiftType + IntToString(nWeightshiftID));

#endif // DEBUG

	return "mh_po_tw_" + sWeightshiftType + IntToString(nWeightshiftID);
}

void UDA2UE4AnimInstance::StopCustomMontages()
{
	ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(TryGetPawnOwner());

	if (oCreature)
	{
		if (Montage_IsPlaying(oCreature->AnimMontageDefault))
		{
			Montage_Stop(0.f, oCreature->AnimMontageDefault);
			ResetDefaultSlot();
		}
		if (Montage_IsPlaying(oCreature->AnimMontageLower))
		{
			Montage_Stop(0.f, oCreature->AnimMontageLower);
			ResetLowerSlot();
		}
		if (Montage_IsPlaying(oCreature->AnimMontageUpperRight))
		{
			Montage_Stop(0.f, oCreature->AnimMontageUpperRight);
			ResetUpperRightSlot();
		}
		if (Montage_IsPlaying(oCreature->AnimMontageUpperLeft))
		{
			Montage_Stop(0.f, oCreature->AnimMontageUpperLeft);
			ResetUpperLeftSlot();
		}
	}
}

void UDA2UE4AnimInstance::PlayMontage(FConvAnimationList animLine, int32 nType)
{
	ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(TryGetPawnOwner());

	switch (nType)
	{
	case SLOT_UPPER_RIGHT:
	{
#ifdef DEBUG
		LogWarning(oCreature->GetName() + " UPPER RIGHT: Attempting to play " + oCreature->AnimSequence->GetName());

#endif // DEBUG

		FSlotAnimationTrack slot = oCreature->AnimMontageUpperRight->SlotAnimTracks[0];

		FAnimSegment Segment;
		Segment.AnimReference = oCreature->AnimSequence;
		Segment.AnimStartTime = 0;// animLine.StartOffset;
		Segment.StartPos = 0;// animLine.StartTime;
		Segment.AnimEndTime = oCreature->AnimSequence->SequenceLength + animLine.StartOffset;
		Segment.AnimPlayRate = animLine.Speed;
		Segment.LoopingCount = 1;
		AnimBlendInUpperRight = animLine.BlendIn;
		SetAnimationTimer(SLOT_UPPER_RIGHT, Segment.AnimEndTime / Segment.AnimPlayRate);

		slot.AnimTrack.AnimSegments.Add(Segment);
#ifdef DEBUG
		LogWarning(oCreature->GetName() + ": Current slot is " + slot.SlotName.ToString() + " with animation " + oCreature->AnimSequence->GetName());
		LogWarning("Current anim segments num is " + IntToString(slot.AnimTrack.AnimSegments.Num()));

#endif // DEBUG

		oCreature->AnimMontageUpperRight->SlotAnimTracks[0] = slot;

		//Montage_SetPlayRate(oCreature->AnimMontageUpperRight, animLine.Speed);
		oCreature->AnimMontageUpperRight->SequenceLength = Segment.AnimEndTime / Segment.AnimPlayRate;// +0.33f;
		Montage_Play(oCreature->AnimMontageUpperRight);
		//montages.Add(oCreature->AnimMontageUpperRight);

		break;
	}
	case SLOT_UPPER_LEFT:
	{
#ifdef DEBUG
		LogWarning(oCreature->GetName() + " UPPER LEFT: Attempting to play " + oCreature->AnimSequence->GetName());

#endif // DEBUG

		FSlotAnimationTrack slot = oCreature->AnimMontageUpperLeft->SlotAnimTracks[0];

		FAnimSegment Segment;
		Segment.AnimReference = oCreature->AnimSequence;
		Segment.AnimStartTime = 0;// animLine.StartOffset;
		Segment.StartPos = 0;// animLine.StartTime;
		Segment.AnimEndTime = oCreature->AnimSequence->SequenceLength + animLine.StartOffset;
		Segment.AnimPlayRate = animLine.Speed;
		Segment.LoopingCount = 1;
		AnimBlendInUpperLeft = animLine.BlendIn;
		SetAnimationTimer(SLOT_UPPER_LEFT, Segment.AnimEndTime / Segment.AnimPlayRate);

		slot.AnimTrack.AnimSegments.Add(Segment);
#ifdef DEBUG
		LogWarning(oCreature->GetName() + ": Current slot is " + slot.SlotName.ToString() + " with animation " + oCreature->AnimSequence->GetName());
		LogWarning("Current anim segments num is " + IntToString(slot.AnimTrack.AnimSegments.Num()));

#endif // DEBUG

		oCreature->AnimMontageUpperLeft->SlotAnimTracks[0] = slot;

		//Montage_SetPlayRate(oCreature->AnimMontageUpperLeft, animLine.Speed);
		oCreature->AnimMontageUpperLeft->SequenceLength = Segment.AnimEndTime / Segment.AnimPlayRate;// +0.33f;
		Montage_Play(oCreature->AnimMontageUpperLeft);
		//montages.Add(oCreature->AnimMontageUpperLeft);

		break;
	}
	default:
		break;
	}
}

void UDA2UE4AnimInstance::SetupCustomMontages()
{
	ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(TryGetPawnOwner());

	//Default
	oCreature->AnimMontageDefault = NewObject<UAnimMontage>();
	oCreature->AnimMontageDefault->SetSkeleton(oCreature->Skeleton);

	FSlotAnimationTrack slotTrackDefault;
	slotTrackDefault.SlotName = "DefaultSlot";

	FAnimTrack animTrackDefault;
	slotTrackDefault.AnimTrack = animTrackDefault;

	FCompositeSection SectionDefault;
	SectionDefault.SectionName = TEXT("Default");
	SectionDefault.SetTime(0.0f);

	oCreature->AnimMontageDefault->CompositeSections.Add(SectionDefault);
	oCreature->AnimMontageDefault->BlendIn.SetBlendTime(0.f);
	oCreature->AnimMontageDefault->BlendOut.SetBlendTime(0.f);
	oCreature->AnimMontageDefault->BlendOutTriggerTime = -1.f;

	oCreature->AnimMontageDefault->SlotAnimTracks.Add(slotTrackDefault);

	//Lower
	oCreature->AnimMontageLower = NewObject<UAnimMontage>();
	oCreature->AnimMontageLower->SetSkeleton(oCreature->Skeleton);

	FSlotAnimationTrack slotTrackLower;
	slotTrackLower.SlotName = "LowerSlot";

	FAnimTrack animTrackLower;
	slotTrackLower.AnimTrack = animTrackLower;

	FCompositeSection SectionLower;
	SectionLower.SectionName = TEXT("Default");
	SectionLower.SetTime(0.0f);

	oCreature->AnimMontageLower->CompositeSections.Add(SectionLower);
	oCreature->AnimMontageLower->BlendIn.SetBlendTime(0.f);
	oCreature->AnimMontageLower->BlendOut.SetBlendTime(0.f);
	oCreature->AnimMontageLower->BlendOutTriggerTime = -1.f;

	oCreature->AnimMontageLower->SlotAnimTracks.Add(slotTrackLower);

	//UpperRight
	oCreature->AnimMontageUpperRight = NewObject<UAnimMontage>();
	oCreature->AnimMontageUpperRight->SetSkeleton(oCreature->Skeleton);

	FSlotAnimationTrack slotTrackUpperRight;
	slotTrackUpperRight.SlotName = "UpperRightSlot";

	FAnimTrack animTrackUpperRight;
	slotTrackUpperRight.AnimTrack = animTrackUpperRight;

	FCompositeSection SectionUpperRight;
	SectionUpperRight.SectionName = TEXT("Default");
	SectionUpperRight.SetTime(0.0f);

	oCreature->AnimMontageUpperRight->CompositeSections.Add(SectionUpperRight);
	oCreature->AnimMontageUpperRight->BlendIn.SetBlendTime(0.f);
	oCreature->AnimMontageUpperRight->BlendOut.SetBlendTime(0.f);
	oCreature->AnimMontageUpperRight->BlendOutTriggerTime = -1.f;

	oCreature->AnimMontageUpperRight->SlotAnimTracks.Add(slotTrackUpperRight);

	//UpperLeft
	oCreature->AnimMontageUpperLeft = NewObject<UAnimMontage>();
	oCreature->AnimMontageUpperLeft->SetSkeleton(oCreature->Skeleton);

	FSlotAnimationTrack slotTrackUpperLeft;
	slotTrackUpperLeft.SlotName = "UpperLeftSlot";

	FAnimTrack animTrackUpperLeft;
	slotTrackUpperLeft.AnimTrack = animTrackUpperLeft;

	FCompositeSection SectionUpperLeft;
	SectionUpperLeft.SectionName = TEXT("Default");
	SectionUpperLeft.SetTime(0.0f);

	oCreature->AnimMontageUpperLeft->CompositeSections.Add(SectionUpperLeft);
	oCreature->AnimMontageUpperLeft->BlendIn.SetBlendTime(0.f);
	oCreature->AnimMontageUpperLeft->BlendOut.SetBlendTime(0.f);
	oCreature->AnimMontageUpperLeft->BlendOutTriggerTime = -1.f;

	oCreature->AnimMontageUpperLeft->SlotAnimTracks.Add(slotTrackUpperLeft);
}