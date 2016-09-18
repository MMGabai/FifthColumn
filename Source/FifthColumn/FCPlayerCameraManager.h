///Copyright (c) 2016, Mordechai M. Gabai

#pragma once

#include "Camera/PlayerCameraManager.h"
#include "FCPlayerCameraManager.generated.h"

UCLASS()
class FIFTHCOLUMN_API AFCPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_UCLASS_BODY()

	float TargetingFOV, NormalFOV;

	void UpdateCamera(float DeltaTime) override;
};
