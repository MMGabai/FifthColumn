//Copyright (c) 2016, Mordechai M. Gabai

#pragma once

#include "FCGameMode.h"
#include "GameFramework/Actor.h"
#include "Dialogue.generated.h"

UCLASS()
class FIFTHCOLUMN_API ADialogue : public AActor
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
		FText GetDialogueName() const;
	
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
		FString GetAbstractDialogueName() const;
	
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
		FText GetDialogueText() const;

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
		bool GetAvailable() const;
	
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
		void ToggleAvailability();

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
		bool GetSpeechCheck() const;

	//Get Dialogue Options
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
		void LoadDialogueOptions();

	//if true, opening this piece of dialogue will load new dialogue options
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
		bool GetDialogueTreeOption() const;

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
		TArray<ADialogue*> GetLoadedDialogueOptions() const;

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
		ADialogue* GetDialogueOption(int32 choice) const;

	//dialogue related events
	UFUNCTION(BlueprintImplementableEvent)
		void DialogueEvent();

	UFUNCTION(BlueprintImplementableEvent)
		void DialogueFailEvent();

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
		void NotifyDialogueFailure();

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
		void NotifyDialogueSuccess();

	//clean ARRAY
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
		void UnloadDialogueOptions(bool EndDialogue = false);

	//check whether the conditions of the dialogue are fulfilled
	//UFUNCTION(BlueprintCallable, Category = Dialogue)
	//	bool Success(AFCCharacter* OpeningCharacter, AFCCharacter* DialoguePartner);

protected:

	//DialogueOptions
	UPROPERTY(Transient)
		TArray<ADialogue*> LoadedDialogueOptions;

	UPROPERTY(EditDefaultsOnly, Category = "Dialogue")
		TArray<TSubclassOf<ADialogue>> DialogueOptions;

	//determines whether the dialogue standard can be reached as option
	UPROPERTY(EditAnywhere, Category = "Dialogue")
		bool bAvailable;

	//if true, opening this piece of dialogue will load new dialogue options
	UPROPERTY(EditDefaultsOnly, Category = "Dialogue")
		bool bDialogueTreeOption;

	//fires only one time
	UPROPERTY(EditDefaultsOnly, Category = "Dialogue")
		bool bOneTimeOnly;

	//if true, one time only fails are valid throughout the session rather than just applicable to character
	UPROPERTY(EditDefaultsOnly, Category = "Dialogue")
		bool bGlobalFail;

	//speech check
	UPROPERTY(EditDefaultsOnly, Category = "Dialogue")
		bool bIsSpeechCheck;

	//new starting dialogue
	UPROPERTY(EditDefaultsOnly, Category = "Dialogue")
		bool bNewStartingDialogue;

	//The name of the dialogue (will be shown if the dialogue can be selected)
	//the name by which to identify the quest
	UPROPERTY(EditDefaultsOnly, Category = "Dialogue")
		FString AbstractDialogueName;

	//the name by which to identify the dialogue for end-users
	UPROPERTY(EditDefaultsOnly, Category = "Dialogue")
		FText DialogueOptionName;

	//THE DIALOGUE ITSELF
	//the name by which to identify the dialogue for end-users
	UPROPERTY(EditDefaultsOnly, Category = "Dialogue")
		FText DialogueOptionText;

	//the name by which to identify the dialogue for end-users
	UPROPERTY(EditDefaultsOnly, Category = "Dialogue")
		FText DialogueFailText;
};
