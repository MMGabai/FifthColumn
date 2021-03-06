//Copyright (c) 2017, Mordechai M. Gabai

// Disturbance source handles noises the player makes in the perception of enemy NPC's
// A Disturbance source will be spawned by anything that makes noise

#pragma once

#include "Engine/TargetPoint.h"
#include "FifthColumn.h"
#include "DisturbanceSource.generated.h"

UCLASS()
class FIFTHCOLUMN_API ADisturbanceSource : public ATargetPoint
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintCallable, Category = AI) int32 GetLoudness() const;

	UFUNCTION(BlueprintCallable, Category = AI) bool GetHeardStatus() const;

	UFUNCTION(BlueprintCallable, Category = AI) AActor* GetInstigator() const;

	UFUNCTION(BlueprintCallable, Category = AI) void SetHeardStatus();

protected: 

	UPROPERTY(EditAnywhere, Category = AI)
		int32 Loudness; //the amount of noise a sound makes - if it reaches 100, the enemy npcs know your position and will engage you

	bool bIsHeard; //if the thing is heard, don't do anything with it
	
};
