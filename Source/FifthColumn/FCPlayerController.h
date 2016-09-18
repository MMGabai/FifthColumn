//Copyright (c) 2016, Mordechai M. Gabai

#pragma once

#include "FifthColumn.h"
#include "GameFramework/PlayerController.h"
#include "FCPlayerController.generated.h"

UCLASS()
class FIFTHCOLUMN_API AFCPlayerController : public APlayerController
{
	GENERATED_UCLASS_BODY()

	//To-do: Send in notifications
    //To-do: Move faction system to Player Controller

private:

	//FACTIONS
	UFUNCTION(BlueprintCallable, Category = "World State")
		virtual void Redistribute(int32 choice, int32 change);

	UPROPERTY(EditAnywhere, Category = "World State")
		float ArmenianFactionDisposition;

	UPROPERTY(EditAnywhere, Category = "World State")
		float AzerbaijaniFactionDisposition;

	UPROPERTY(EditAnywhere, Category = "World State")
		float SovietFactionDisposition;

	UPROPERTY(EditAnywhere, Category = "World State")
		float MercenaryFactionDisposition;

	UPROPERTY(EditAnywhere, Category = "World State")
		float SSFactionDisposition;

public:

	//FACTIONS
	UFUNCTION(BlueprintCallable, Category = "World State")
		virtual void SetDispositionTowardsPlayer(int32 choice, float change, bool redistribute = false, bool notification = true);

	UFUNCTION(BlueprintCallable, Category = "World State")
		float GetDispositionTowardsPlayer(int32 choice) const;

	//GAMEPLAY
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void SendNotification(FText Message);

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		FText GetLatestMessage();

	//sound played on notify
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
		USoundCue* NotificationSound;

	//sound played on notify
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
		USoundCue* FactionsChangedSound;

	TArray<FText> MessageStack;
};
