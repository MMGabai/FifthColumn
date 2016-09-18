//Copyright (c) 2016, Mordechai M. Gabai

#pragma once

#include "FifthColumn.h"

AFCPlayerController::AFCPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) 
{
	PlayerCameraManagerClass = AFCPlayerCameraManager::StaticClass();
}

//FACTIONS
float AFCPlayerController::GetDispositionTowardsPlayer(int32 choice) const
{
	switch (choice)
	{
	case 1:
		return ArmenianFactionDisposition;
	case 2:
		return AzerbaijaniFactionDisposition;
	case 3:
		return SovietFactionDisposition;
	case 4:
		return MercenaryFactionDisposition;
	case 5:
		return SSFactionDisposition;
	}

	return 0;
}

void AFCPlayerController::SetDispositionTowardsPlayer(int32 choice, float change, bool redistribute, bool notification)
{
	if (notification)
		SendNotification(FText::FromString("Your actions have been noted."));

	UGameplayStatics::PlaySoundAttached(FactionsChangedSound, GetRootComponent());

	switch (choice)
	{
	case 1:

		if (!(ArmenianFactionDisposition <= 0))
		{
			ArmenianFactionDisposition += change;

			if (ArmenianFactionDisposition >= 100)
				ArmenianFactionDisposition = 100;
		}
		else
			ArmenianFactionDisposition = 0;

		if (redistribute)
			Redistribute(choice, change);

		return;
	case 2:

		if (!(AzerbaijaniFactionDisposition <= 0))
		{
			AzerbaijaniFactionDisposition += change;

			if (AzerbaijaniFactionDisposition >= 100)
				AzerbaijaniFactionDisposition = 100;
		}
		else
			AzerbaijaniFactionDisposition = 0;

		if (redistribute)
			Redistribute(choice, change);

		return;
	case 3:
		if (!(SovietFactionDisposition <= 0))
		{
			SovietFactionDisposition += change;

			if (SovietFactionDisposition >= 100)
				SovietFactionDisposition = 100;
		}
		else
			SovietFactionDisposition = 0;

		if (redistribute)
			Redistribute(choice, change);

		return;
	case 4:
		if (!(MercenaryFactionDisposition <= 0))
		{
			MercenaryFactionDisposition += change;

			if (MercenaryFactionDisposition >= 100)
				MercenaryFactionDisposition = 100;
		}
		else
			MercenaryFactionDisposition = 0;

		if (redistribute)
			Redistribute(choice, change);

		return;
	case 5:
		if (!(SSFactionDisposition <= 0))
		{
			SSFactionDisposition += change;

			if (SSFactionDisposition >= 100)
				SSFactionDisposition = 100;
		}
		else
			SSFactionDisposition = 0;

		if (redistribute)
			Redistribute(choice, change);

		return;
	}
}

void AFCPlayerController::Redistribute(int32 choice, int32 change)
{
	change = -change;

	for (int32 i = 1; i <= 5; i++)
		if (i != choice)
			SetDispositionTowardsPlayer(i, change / 2, false, false);
}

void AFCPlayerController::SendNotification(FText Message)
{
	//make sure there is a controlled pawn
	if (GetControlledPawn())
		Cast<AFCCharacter>(GetControlledPawn())->HudRunoffTime = 6.0f;

	MessageStack.Push(Message);

	if (MessageStack.Num() > 5)
		MessageStack.RemoveAt(0);
}

FText AFCPlayerController::GetLatestMessage()
{
	FText Message = FText::GetEmpty();
	int32 MessageStackCount = MessageStack.Num();

	if (MessageStackCount > 0)
	{
		FText LastMessage = MessageStack.Last();

		for (int32 i = MessageStackCount - 1; i >= 0; i--)
		{
			FText MessageIteration = MessageStack[i];

			Message = FText::FromString(MessageIteration.ToString() + "\n" + Message.ToString());
		}

		return Message;
	}

	SendNotification(FText::FromString("Notifications will be shown here."));
	return Message;
}