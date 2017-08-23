//Copyright (c) 2016, Mordechai M. Gabai

#pragma once

#include "FifthColumn.h"

AFCPlayerController::AFCPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) 
{
	PlayerCameraManagerClass = AFCPlayerCameraManager::StaticClass();
	InputYawScale = 2.0f;
	InputPitchScale = -1.0f;
}