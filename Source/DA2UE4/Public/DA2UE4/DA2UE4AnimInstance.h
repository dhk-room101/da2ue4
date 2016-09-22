// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "STypes.h"

#include "Animation/AnimInstance.h"
#include "DA2UE4AnimInstance.generated.h"

/**
 *
 */
UCLASS()
class DA2UE4_API UDA2UE4AnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "NativeUpdateAnimation")
		void NativeUpdateAnimation(float DeltaSeconds) override;

	void NativeInitializeAnimation() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool bIsConversation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Slots)
		float AnimBlendDefault;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Slots)
		float AnimBlendLower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Slots)
		float AnimBlendUpperRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Slots)
		float AnimBlendUpperLeft;

	UPROPERTY()
		float AnimBlendInDefault;

	UPROPERTY()
		float AnimBlendInLower;

	UPROPERTY()
		float AnimBlendInUpperRight;

	UPROPERTY()
		float AnimBlendInUpperLeft;

	UPROPERTY()
		FTimerHandle TimerHandleWeightshift;

	UPROPERTY()
		FTimerHandle TimerHandleDefault;

	UPROPERTY()
		FTimerHandle TimerHandleLower;

	UPROPERTY()
		FTimerHandle TimerHandleUpperRight;

	UPROPERTY()
		FTimerHandle TimerHandleUpperLeft;

	TArray<UAnimMontage*> montages;

	void StopCustomMontages();

	void HandleIdleWeightshift();
	FString GetIdleAnimation();

	void SetAnimationTimer(int32 nSlot, float fLength);

	void ResetDefaultSlot();
	void ResetLowerSlot();
	void ResetUpperRightSlot();
	void ResetUpperLeftSlot();

	void HandleMontageDefault();
	void HandleMontageLower();
	void HandleMontageUpperRight();
	void HandleMontageUpperLeft();

	void PlayMontage(FConvAnimationList animLine, int32 nType);
	void SetupCustomMontages();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float fMovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool bIsAttacking;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool bIsCombatState;
};
