//Copyright (c) 2016, Mordechai M. Gabai

#pragma once

#include "FifthColumn.h"
#include "GameFramework/Actor.h"
#include "Quest.generated.h"

UCLASS(abstract)
class FIFTHCOLUMN_API AQuest : public AActor
{
	GENERATED_UCLASS_BODY()
	
	UFUNCTION(BlueprintCallable, Category = Quest)
		bool GetIsComplete() const;

	UFUNCTION(BlueprintCallable, Category = Quest)
		FString GetAbstractName() const;

	UFUNCTION(BlueprintCallable, Category = Quest)
		FText GetName() const;

	UFUNCTION(BlueprintCallable, Category = Quest)
		FText GetQuestDesc() const;

	//quest started sound
	UPROPERTY(EditDefaultsOnly, Category = Effects)
		USoundCue* StartSound;

	//quest finished sound
	UPROPERTY(EditDefaultsOnly, Category = Effects)
		USoundCue* FinishSound;

	//quest failed sound
	UPROPERTY(EditDefaultsOnly, Category = Effects)
		USoundCue* FailSound;

	UFUNCTION(BlueprintCallable, Category = "Quest")
		virtual void QuestStarted(AFCPlayerCharacter* Player);

	UFUNCTION(BlueprintCallable, Category = "Quest")
		virtual void QuestFinished(AFCPlayerCharacter* Player);
	
	UFUNCTION(BlueprintCallable, Category = "Quest")
		virtual void QuestFailed(AFCPlayerCharacter* Player);

	protected:

	bool bQuestComplete;

	//XP one will get by finishing the quest
	UPROPERTY(EditDefaultsOnly, Category = Config)
		int32 XPtoAddOnCompletion;

	//the name by which to identify the quest
	UPROPERTY(EditDefaultsOnly, Category = Config)
		FString AbstractName;

	//the name by which to identify the quest for users
	UPROPERTY(EditDefaultsOnly, Category = Localisation)
		FText QuestName;

	UPROPERTY(EditDefaultsOnly, Category = Localisation)
		FText QuestDesc;
};
