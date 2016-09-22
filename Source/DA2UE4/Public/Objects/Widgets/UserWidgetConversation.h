// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "STypes.h"

#include "Blueprint/UserWidget.h"
#include "UserWidgetConversation.generated.h"

class UButton;
class UTextBlock;
class UBorder;

/**
 *
 */
UCLASS()
class DA2UE4_API UUserWidgetConversation : public UUserWidget
{
	GENERATED_BODY()

public:

	bool endConversation = false;

	FConversation conversation;
	AActor* CONVERSATION_DEFAULT_SPEAKER = nullptr;

	TArray<UButton*> pLines;
	TMap<int32, FConvLine> wheelInstance;//for player replies
	
	FConvLine ConvNextLineReply;//to queue up the next reply

	UBorder* BorderMood;

	UButton* ButtonPlayer0;
	UButton* ButtonPlayer1;
	UButton* ButtonPlayer2;
	UButton* ButtonPlayer3;
	UButton* ButtonPlayer4;
	UButton* ButtonPlayer5;

	UTextBlock* TextPlayer0;
	UTextBlock* TextPlayer1;
	UTextBlock* TextPlayer2;
	UTextBlock* TextPlayer3;
	UTextBlock* TextPlayer4;
	UTextBlock* TextPlayer5;

	UTextBlock* TextNPC;

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
		void OnHovered0();
	UFUNCTION()
		void OnHovered1();
	UFUNCTION()
		void OnHovered2();
	UFUNCTION()
		void OnHovered3();
	UFUNCTION()
		void OnHovered4();
	UFUNCTION()
		void OnHovered5();

	UFUNCTION()
		void OnUnhovered0();
	UFUNCTION()
		void OnUnhovered1();
	UFUNCTION()
		void OnUnhovered2();
	UFUNCTION()
		void OnUnhovered3();
	UFUNCTION()
		void OnUnhovered4();
	UFUNCTION()
		void OnUnhovered5();

	UFUNCTION()
		void OnClicked0();
	UFUNCTION()
		void OnClicked1();
	UFUNCTION()
		void OnClicked2();
	UFUNCTION()
		void OnClicked3();
	UFUNCTION()
		void OnClicked4();
	UFUNCTION()
		void OnClicked5();

	UFUNCTION()
		void HandleClick(int32 n);
};
