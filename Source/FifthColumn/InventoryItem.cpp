//Copyright (c) 2016, Mordechai M. Gabai

#pragma once

#include "FifthColumn.h"

AInventoryItem::AInventoryItem(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) 
{
	bIsActive = false;
	bAutoActivate = false;
	PickedUpBy = NULL;
	NoQuantity = true;
	iWeaponType = 4;
}

void AInventoryItem::StartFire() 
{
	if (!(GetPawnOwner()->IsRunning()))
		Activate();
}

void AInventoryItem::GiveAmmo(int AddAmount)
{
	const int32 MissingAmmo = FMath::Max(0, WeaponConfig.MaxAmmo - CurrentAmmoInClip);
	AddAmount = FMath::Min(AddAmount, MissingAmmo);
	CurrentAmmoInClip += AddAmount;
}

//To trigger activation events
void AInventoryItem::Activate()
{
	if (!NoQuantity && CurrentAmmoInClip > 0)
		CurrentAmmoInClip--;

	OnActivatedEvent();
}

bool AInventoryItem::GetNoQuantity() const 
{
	return NoQuantity;
}

bool AInventoryItem::CanBePickedUp(class AFCCharacter* TestPawn) const 
{
	return TestPawn && TestPawn->IsAlive();
}