//Copyright (c) 2016, Mordechai M. Gabai
#pragma once

#include "FifthColumn.h"
#include "NPC.generated.h"

UCLASS(abstract)
class FIFTHCOLUMN_API ANPC : public AFCCharacter
{
	GENERATED_UCLASS_BODY()

	virtual void PostInitializeComponents() override;

	//select an UBehaviorType
	UPROPERTY(EditAnywhere, Category = Behavior)
		UBehaviorTree* NPCBehavior;

	UPROPERTY(EditAnywhere, Category = Dialogue)
		TSubclassOf<class ADialogue> StartingDialogue;

public:
	UFUNCTION(BlueprintCallable, Category = Behavior)
		int32 GetFaction() const;

	UFUNCTION(BlueprintCallable, Category = Behavior)
		int32 GetDialogueLevel() const;

	UFUNCTION(BlueprintCallable, Category = Behavior)
		void SetStartingDialogue(ADialogue* NewStartingDialogue);

	UFUNCTION(BlueprintCallable, Category = Behavior)
		void SetHostileToPlayer(bool hostile = true);

	UFUNCTION(BlueprintCallable, Category = Behavior)
		bool GetHostileToPlayer() const;

	//Check if pawn is enemy if given controller
	bool IsEnemyFor(AController* TestPC) const;

	//notification when killed
	virtual void OnDeath(float KillingDamage, struct FDamageEvent const& DamageEvent, class APawn* InstigatingPawn, class AActor* DamageCauser) override;

protected:

	UPROPERTY(EditDefaultsOnly, Category = Behavior)
		int32 DialogueLevel; //the level in which the character converses from 1 to 3. Higher levels require more skill

	UPROPERTY(EditDefaultsOnly, Category = Behavior)
		bool bIsHostileToPlayer; //when this option is on, it will attack the player regardless of faction disposition

	UPROPERTY(EditDefaultsOnly, Category = Behavior)
		int32 FactionID;
};