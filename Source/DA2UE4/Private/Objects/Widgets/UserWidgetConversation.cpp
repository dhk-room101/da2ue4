// Fill out your copyright notice in the Description page of Project Settings.

#include "DA2UE4.h"
#include "UserWidgetConversation.h"

#include "DA2UE4GameInstance.h"
#include "DA2UE4PlayerController.h"
#include "DA2UE4Party.h"

#include "WidgetBlueprintLibrary.h"

#include "UMG.h"
#include "ldf.h"
#include "events_h.h"
#include "STypes.h"
#include "conversation_h.h"

void UUserWidgetConversation::NativeConstruct()
{
	Super::NativeConstruct();

#ifdef DEBUG
	//UE_LOG(LogTemp, Warning, TEXT("------ - Construct---- - "));

#endif // DEBUG

	BorderMood = (UBorder*)GetWidgetFromName(TEXT("BorderMood"));
	BorderMood->SetVisibility(ESlateVisibility::Hidden);

	TextNPC = (UTextBlock*)GetWidgetFromName(TEXT("TextNPC"));

	TextPlayer0 = (UTextBlock*)GetWidgetFromName(TEXT("TextPlayer0"));
	TextPlayer0->SetColorAndOpacity(FLinearColor(0.58, 0.58, 0.58, 1));
	TextPlayer1 = (UTextBlock*)GetWidgetFromName(TEXT("TextPlayer1"));
	TextPlayer1->SetColorAndOpacity(FLinearColor(0.58, 0.58, 0.58, 1));
	TextPlayer2 = (UTextBlock*)GetWidgetFromName(TEXT("TextPlayer2"));
	TextPlayer2->SetColorAndOpacity(FLinearColor(0.58, 0.58, 0.58, 1));
	TextPlayer3 = (UTextBlock*)GetWidgetFromName(TEXT("TextPlayer3"));
	TextPlayer3->SetColorAndOpacity(FLinearColor(0.58, 0.58, 0.58, 1));
	TextPlayer4 = (UTextBlock*)GetWidgetFromName(TEXT("TextPlayer4"));
	TextPlayer4->SetColorAndOpacity(FLinearColor(0.58, 0.58, 0.58, 1));
	TextPlayer5 = (UTextBlock*)GetWidgetFromName(TEXT("TextPlayer5"));
	TextPlayer5->SetColorAndOpacity(FLinearColor(0.58, 0.58, 0.58, 1));

	ButtonPlayer0 = (UButton*)GetWidgetFromName(TEXT("ButtonPlayer0"));
	ButtonPlayer0->OnHovered.AddDynamic(this, &UUserWidgetConversation::OnHovered0);
	ButtonPlayer0->OnUnhovered.AddDynamic(this, &UUserWidgetConversation::OnUnhovered0);
	ButtonPlayer0->OnClicked.AddDynamic(this, &UUserWidgetConversation::OnClicked0);

	ButtonPlayer1 = (UButton*)GetWidgetFromName(TEXT("ButtonPlayer1"));
	ButtonPlayer1->OnHovered.AddDynamic(this, &UUserWidgetConversation::OnHovered1);
	ButtonPlayer1->OnUnhovered.AddDynamic(this, &UUserWidgetConversation::OnUnhovered1);
	ButtonPlayer1->OnClicked.AddDynamic(this, &UUserWidgetConversation::OnClicked1);

	ButtonPlayer2 = (UButton*)GetWidgetFromName(TEXT("ButtonPlayer2"));
	ButtonPlayer2->OnHovered.AddDynamic(this, &UUserWidgetConversation::OnHovered2);
	ButtonPlayer2->OnUnhovered.AddDynamic(this, &UUserWidgetConversation::OnUnhovered2);
	ButtonPlayer2->OnClicked.AddDynamic(this, &UUserWidgetConversation::OnClicked2);

	ButtonPlayer3 = (UButton*)GetWidgetFromName(TEXT("ButtonPlayer3"));
	ButtonPlayer3->OnHovered.AddDynamic(this, &UUserWidgetConversation::OnHovered3);
	ButtonPlayer3->OnUnhovered.AddDynamic(this, &UUserWidgetConversation::OnUnhovered3);
	ButtonPlayer3->OnClicked.AddDynamic(this, &UUserWidgetConversation::OnClicked3);

	ButtonPlayer4 = (UButton*)GetWidgetFromName(TEXT("ButtonPlayer4"));
	ButtonPlayer4->OnHovered.AddDynamic(this, &UUserWidgetConversation::OnHovered4);
	ButtonPlayer4->OnUnhovered.AddDynamic(this, &UUserWidgetConversation::OnUnhovered4);
	ButtonPlayer4->OnClicked.AddDynamic(this, &UUserWidgetConversation::OnClicked4);

	ButtonPlayer5 = (UButton*)GetWidgetFromName(TEXT("ButtonPlayer5"));
	ButtonPlayer5->OnHovered.AddDynamic(this, &UUserWidgetConversation::OnHovered5);
	ButtonPlayer5->OnUnhovered.AddDynamic(this, &UUserWidgetConversation::OnUnhovered5);
	ButtonPlayer5->OnClicked.AddDynamic(this, &UUserWidgetConversation::OnClicked5);

	pLines.Add(ButtonPlayer0);
	pLines.Add(ButtonPlayer1);
	pLines.Add(ButtonPlayer2);
	pLines.Add(ButtonPlayer3);
	pLines.Add(ButtonPlayer4);
	pLines.Add(ButtonPlayer5);
}

void UUserWidgetConversation::OnHovered0()
{
	TextPlayer0->SetColorAndOpacity(FLinearColor(0.99, 0.99, 0.99, 1));
	FString s0 = wheelInstance.Find(0)->iconID;
	UTexture2D* t0 = *GetCurrentPlayerController()->moodTextures.Find(s0);
	FSlateBrush b0 = UWidgetBlueprintLibrary::MakeBrushFromTexture(t0);
	BorderMood->SetBrush(b0);
	BorderMood->SetVisibility(ESlateVisibility::Visible);
// 	NextLine(wheelInstance.Find(0)->lineIndex);
// 	UE_LOG(LogTemp, Warning, TEXT("------ - Clicked branch ---- - %i"), wheelInstance.Find(0)->lineIndex);
// 	NextLine(lineIndex);
}

void UUserWidgetConversation::OnHovered1()
{
	TextPlayer1->SetColorAndOpacity(FLinearColor(0.99, 0.99, 0.99, 1));
	FString s1 = wheelInstance.Find(1)->iconID;
	UTexture2D* t1 = *GetCurrentPlayerController()->moodTextures.Find(s1);
	FSlateBrush b1 = UWidgetBlueprintLibrary::MakeBrushFromTexture(t1);
	BorderMood->SetBrush(b1);
	BorderMood->SetVisibility(ESlateVisibility::Visible);
}

void UUserWidgetConversation::OnHovered2()
{
	TextPlayer2->SetColorAndOpacity(FLinearColor(0.99, 0.99, 0.99, 1));
	FString s2 = wheelInstance.Find(2)->iconID;
	UTexture2D* t2 = *GetCurrentPlayerController()->moodTextures.Find(s2);
	FSlateBrush b2 = UWidgetBlueprintLibrary::MakeBrushFromTexture(t2);
	BorderMood->SetBrush(b2);
	BorderMood->SetVisibility(ESlateVisibility::Visible);
}

void UUserWidgetConversation::OnHovered3()
{
	TextPlayer3->SetColorAndOpacity(FLinearColor(0.99, 0.99, 0.99, 1));
	FString s3 = wheelInstance.Find(3)->iconID;
	UTexture2D* t3 = *GetCurrentPlayerController()->moodTextures.Find(s3);
	FSlateBrush b3 = UWidgetBlueprintLibrary::MakeBrushFromTexture(t3);
	BorderMood->SetBrush(b3);
	BorderMood->SetVisibility(ESlateVisibility::Visible);
}

void UUserWidgetConversation::OnHovered4()
{
	TextPlayer4->SetColorAndOpacity(FLinearColor(0.99, 0.99, 0.99, 1));
	FString s4 = wheelInstance.Find(4)->iconID;
	UTexture2D* t4 = *GetCurrentPlayerController()->moodTextures.Find(s4);
	FSlateBrush b4 = UWidgetBlueprintLibrary::MakeBrushFromTexture(t4);
	BorderMood->SetBrush(b4);
	BorderMood->SetVisibility(ESlateVisibility::Visible);
}

void UUserWidgetConversation::OnHovered5()
{
	TextPlayer5->SetColorAndOpacity(FLinearColor(0.99, 0.99, 0.99, 1));
	FString s5 = wheelInstance.Find(5)->iconID;
	UTexture2D* t5 = *GetCurrentPlayerController()->moodTextures.Find(s5);
	FSlateBrush b5 = UWidgetBlueprintLibrary::MakeBrushFromTexture(t5);
	BorderMood->SetBrush(b5);
	BorderMood->SetVisibility(ESlateVisibility::Visible);
}

void UUserWidgetConversation::OnUnhovered0()
{
	TextPlayer0->SetColorAndOpacity(FLinearColor(0.58, 0.58, 0.58, 1));
	BorderMood->SetVisibility(ESlateVisibility::Hidden);
}

void UUserWidgetConversation::OnUnhovered1()
{
	TextPlayer1->SetColorAndOpacity(FLinearColor(0.58, 0.58, 0.58, 1));
	BorderMood->SetVisibility(ESlateVisibility::Hidden);
}

void UUserWidgetConversation::OnUnhovered2()
{
	TextPlayer2->SetColorAndOpacity(FLinearColor(0.58, 0.58, 0.58, 1));
	BorderMood->SetVisibility(ESlateVisibility::Hidden);
}

void UUserWidgetConversation::OnUnhovered3()
{
	TextPlayer3->SetColorAndOpacity(FLinearColor(0.58, 0.58, 0.58, 1));
	BorderMood->SetVisibility(ESlateVisibility::Hidden);
}

void UUserWidgetConversation::OnUnhovered4()
{
	TextPlayer4->SetColorAndOpacity(FLinearColor(0.58, 0.58, 0.58, 1));
	BorderMood->SetVisibility(ESlateVisibility::Hidden);
}

void UUserWidgetConversation::OnUnhovered5()
{
	TextPlayer5->SetColorAndOpacity(FLinearColor(0.58, 0.58, 0.58, 1));
	BorderMood->SetVisibility(ESlateVisibility::Hidden);
}

void UUserWidgetConversation::OnClicked0()
{
	HandleClick(0);
}

void UUserWidgetConversation::OnClicked1()
{
	HandleClick(1);
}

void UUserWidgetConversation::OnClicked2()
{
	HandleClick(2);
}

void UUserWidgetConversation::OnClicked3()
{
	HandleClick(3);
}

void UUserWidgetConversation::OnClicked4()
{
	HandleClick(4);
}

void UUserWidgetConversation::OnClicked5()
{
	HandleClick(5);
}

void UUserWidgetConversation::HandleClick(int32 n)
{
#ifdef DEBUG
	LogWarning("------ - Clicked " + IntToString(n) + " ---- - ");

#endif // DEBUG

	//reset audio comp
	USoundCue* empty = NewObject<USoundCue>();
	GetCurrentPlayerController()->ConversationAudioComp->SetSound(empty);

	GetWorld()->GetTimerManager().ClearTimer(GetCurrentPlayerController()->SoundCueTimerHandle);

	NextLine(wheelInstance.Find(n)->lineIndex);
}