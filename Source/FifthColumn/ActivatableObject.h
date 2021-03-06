//Copyright (c) 2017, Mordechai M. Gabai

#pragma once

#include "FifthColumn.h"
#include "ActivatableObject.generated.h"

/**
 * Activatables are similar to inventory items, though unlike inventory items, 
 they remain in the game world and cannot be stored in the inventory.

 However, occasionally, they can be reused more then ones. Can be used for in-game
 readables, ammo crates and dispensers
 */
UCLASS(abstract)
class FIFTHCOLUMN_API AActivatableObject : public AFCObject
{
	GENERATED_BODY()
public:
	AActivatableObject();

	virtual void BeginPlay() override;

	virtual void Activate(class AFCCharacter* Pawn, bool bForce = false);

	UFUNCTION(BlueprintCallable, Category = Text)
		FText GetBookText() const;

	//activate item, weapons should be automatically activated
	UFUNCTION(BlueprintImplementableEvent)
		void OnActivatedEvent();

	virtual void Tick(float DeltaSeconds) override;
protected:

	//Will it automatically activate, or does it need to be activated?
	UPROPERTY(EditDefaultsOnly, Category = Item)
		bool bAutoActivate;

	UPROPERTY(EditDefaultsOnly, Category = Item)
		bool bAutoDestroy;

	UPROPERTY(EditDefaultsOnly, Category = Item)
		bool bIsReadable;

	UFUNCTION()
		void OnRep_IsActive();

	UPROPERTY(EditDefaultsOnly, Category = Localisation)
		FText BookText;

	UPROPERTY(EditAnywhere, Category = "Faction")
		int32 FactionAffiliation;

	UPROPERTY(EditAnywhere, Category = "Faction")
		float FactionChange;
};
