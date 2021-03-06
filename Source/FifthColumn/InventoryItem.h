//Copyright (c) 2017, Mordechai M. Gabai

#pragma once

#include "FifthColumn.h"
#include "FCWeapon.h"
#include "InventoryItem.generated.h"

UCLASS(abstract)
class FIFTHCOLUMN_API AInventoryItem : public AFCWeapon
{
	GENERATED_BODY()

public:
	AInventoryItem();

	virtual void GiveAmmo(int AddAmount) override;

	virtual bool CanBePickedUp(class AFCCharacter* TestPawn) const;

	virtual void StartFire() override;

	virtual void Activate();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		bool GetNoQuantity() const;

	//activate item, weapons should be automatically activated
	UFUNCTION(BlueprintImplementableEvent)
		void OnActivatedEvent();

protected:

	//Will it automatically activate, or does it need to be activated?
	UPROPERTY(EditDefaultsOnly, Category = Item)
		bool bAutoActivate;

	UPROPERTY(EditDefaultsOnly, Category = Config)
		bool NoQuantity;

	bool GivePickupTo(class AFCCharacter* Pawn);
};