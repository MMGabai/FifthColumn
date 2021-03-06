//Copyright (c) 2016, Mordechai M. Gabai

#pragma once

#include "GameFramework/SaveGame.h"
#include "FCPersistentWorld.generated.h"

/**
 * 
 */
UCLASS(abstract)
class FIFTHCOLUMN_API UFCPersistentWorld : public USaveGame
{
	GENERATED_UCLASS_BODY()

	FString SaveGameName;
	int32 UserIndex;

	bool bFullScreen;
	bool bBorderless;
	bool InvertedXAxis;
	bool InvertedYAxis;

	float ViewDistance;
	float Gamma;
	float Brightness;
	float AspectRatio;

	int32 AA;
	int32 Shadows;

	int32 Effects;
	int32 PostProcessingQuality;

	void SaveGame();
	void LoadGame(FString GameNameInput, int32 UserIndexInput);
	
};
