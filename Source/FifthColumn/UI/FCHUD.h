// Copyright © 2017, Mordechai Gabai

#pragma once

#include "GameFramework/HUD.h"
#include "FCHUD.generated.h"

/**
 * 
 */
UCLASS()
class FIFTHCOLUMN_API AFCHUD : public AHUD
{
	GENERATED_BODY()

	AFCHUD();

	virtual void BeginPlay() override;
	
};
