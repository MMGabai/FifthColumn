///Copyright (c) 2017, Mordechai M. Gabai

#pragma once

#include "Camera/PlayerCameraManager.h"
#include "FCPlayerCameraManager.generated.h"

UCLASS()
class FIFTHCOLUMN_API AFCPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_UCLASS_BODY()
public:
	void UpdateCamera(float DeltaTime);

private:
	float TargetingFOV, NormalFOV;
};
