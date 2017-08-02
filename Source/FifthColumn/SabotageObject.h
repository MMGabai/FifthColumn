//Copyright (c) 2016, Mordechai M. Gabai

#pragma once

#include "ActivatableObject.h"
#include "SabotageObject.generated.h"

/**
 * 
 */
UCLASS()
class FIFTHCOLUMN_API ASabotageObject : public AActivatableObject
{
	GENERATED_UCLASS_BODY()

	virtual void Activate(class AFCCharacter* Pawn, bool bForce) override;

	int32 GetLevel() const;
	int32 GetXPGained() const;

	bool GetbCanBeDestroyed() const;

	//Action which happens once object is succesfully sabotaged.
	UFUNCTION(BlueprintImplementableEvent)
		virtual void OnActivatedEvent();

protected:

	UPROPERTY(EditAnywhere, Category = Config)
		int32 Level; //the amount of skill with the lockpick needed to bypass the lock

	UPROPERTY(EditAnywhere, Category = Config)
		int32 XPGained; //XP gained by breaking/bypassing the lock

	UPROPERTY(EditDefaultsOnly, Category = Config)
		TSubclassOf<AInventoryItem> Key; //determines which inventory item can be used as key

	UPROPERTY(EditDefaultsOnly, Category = Config)
		bool bCanBeDestroyed; //can be destroyed using weapons
	
};
