//Copyright (c) 2016, Mordechai M. Gabai

#pragma once

#include "FifthColumn.h"

AQuest::AQuest(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bOnlyRelevantToOwner = true;
}

//Quest Name
FText AQuest::GetName() const 
{
	return QuestName;
}

FText AQuest::GetQuestDesc() const 
{
	return QuestDesc;
}

FString AQuest::GetAbstractName() const
{
	return AbstractName;
}

bool AQuest::GetIsComplete() const 
{
	return bQuestComplete;
}

//TODO: Quest events, etc.
void AQuest::QuestStarted(AFCPlayerCharacter* Player) 
{
	SetOwner(Player);

	if (Player)
	{
		Player->ActiveQuests.Add(this);
		Player->SelectNextQuest();
	}

	if (StartSound)
		UGameplayStatics::PlaySoundAttached(StartSound, GetRootComponent());
}

void AQuest::QuestFinished(AFCPlayerCharacter* Player) 
{
	if (!bQuestComplete)
	{
		if (FinishSound)
			UGameplayStatics::PlaySoundAttached(FinishSound, GetRootComponent());

		Player->AddXP(XPtoAddOnCompletion);
		Player->ActiveQuests.Remove(this);
	}

	Player->SelectNextQuest();
}

void AQuest::QuestFailed(AFCPlayerCharacter* Player) 
{
	//It never happened
	Destroy();
	Player->SelectNextQuest();

	if (FailSound)
		UGameplayStatics::PlaySoundAttached(FailSound, GetRootComponent());
}