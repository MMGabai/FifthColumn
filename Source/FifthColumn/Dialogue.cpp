//Copyright (c) 2016, Mordechai Gabai

#pragma once

#include "FifthColumn.h"
#include "FCGameMode.h"
#include "Dialogue.h"


ADialogue::ADialogue(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bDialogueTreeOption = false;
	bOneTimeOnly = false;
	bGlobalFail = false;
	bIsSpeechCheck = false;
	bNewStartingDialogue = false;
	DialogueOptions.AddZeroed(5);
}

FText ADialogue::GetDialogueName() const 
{
	return DialogueOptionName;
}

FString ADialogue::GetAbstractDialogueName() const 
{
	return AbstractDialogueName;
}

FText ADialogue::GetDialogueText() const
{
	return DialogueOptionText;
}

bool ADialogue::GetAvailable() const
{
	return bAvailable;
}

void ADialogue::ToggleAvailability()
{
	bOneTimeOnly && bAvailable ? bAvailable = false : NULL;
}

bool ADialogue::GetSpeechCheck() const
{
	return bIsSpeechCheck;
}

//get dialogue options
//they have to be unloaded, as they are still in the world
void ADialogue::UnloadDialogueOptions(bool EndDialogue)
{
	int j;

	//if the dialogue options must be stopped due to end of dialogue, use different starting value
	if (EndDialogue)
		j = 0;
	else
		j = 1;

	for (int32 i = j; i < LoadedDialogueOptions.Num(); i++)
		if (LoadedDialogueOptions[i] != NULL)
			LoadedDialogueOptions[i]->Destroy();

	LoadedDialogueOptions.SetNum(0);
}

void ADialogue::LoadDialogueOptions()
{
	for (int32 i = 0; i < DialogueOptions.Num(); i++)
	{
		ADialogue* DialogueOption = GetWorld()->SpawnActor<ADialogue>(DialogueOptions[i]);

		//filter out dialogue placed in the blocking box;
		if (DialogueOption && Cast<AFCGameMode>(GetWorld()->GetAuthGameMode())->
			CheckForDialogueName(DialogueOption->GetAbstractDialogueName()) == false)
			LoadedDialogueOptions.Add(DialogueOption);
		else
			LoadedDialogueOptions.Add(NULL);
	}
}

bool ADialogue::GetDialogueTreeOption() const
{
	return bDialogueTreeOption;
}

void ADialogue::NotifyDialogueFailure()
{
	DialogueFailEvent();
	DialogueOptionText = DialogueFailText;

	bOneTimeOnly ? (Cast<AFCGameMode>(GetWorld()->GetAuthGameMode()))->AddIntoBlockedDialogueName(AbstractDialogueName) : NULL;
}

void ADialogue::NotifyDialogueSuccess()
{
	DialogueEvent();
	bOneTimeOnly || bIsSpeechCheck ? (Cast<AFCGameMode>(GetWorld()->GetAuthGameMode()))->AddIntoBlockedDialogueName(AbstractDialogueName) : NULL;
}

//memory issues - not sure whether it is the engine or myself
TArray<ADialogue*> ADialogue::GetLoadedDialogueOptions() const
{
	return LoadedDialogueOptions;
}

ADialogue* ADialogue::GetDialogueOption(int32 choice) const
{
	if (LoadedDialogueOptions.Num() > 0)
		return LoadedDialogueOptions[choice];
	else
		return 0;
}