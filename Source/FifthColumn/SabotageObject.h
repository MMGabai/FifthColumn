//Copyright (c) 2017, Mordechai M. Gabai

#pragma once

#include "ActivatableObject.h"
#include "SabotageObject.generated.h"

/**
 * 
 */
UCLASS()
class FIFTHCOLUMN_API ASabotageObject : public AActivatableObject
{
	GENERATED_BODY()

public:
	ASabotageObject();

	//Action which happens once object is succesfully sabotaged.
	UFUNCTION(BlueprintImplementableEvent, Category = Game)
		void OnActivatedEvent();

	//is the object sabotaged?
	UFUNCTION(BlueprintCallable, Category = Game)
		bool GetSabotaged();

	virtual void Activate(class AFCCharacter* Pawn, bool bForce) override;

	int32 GetLevel() const;

	int32 GetXPGained() const;

	bool GetbCanBeDestroyed() const;

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
