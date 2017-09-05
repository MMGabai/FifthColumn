//Copyright (c) 2017, Mordechai M. Gabai

#include "FifthColumn.h"
#include "FCPlayerCameraManager.h"

AFCPlayerCameraManager::AFCPlayerCameraManager(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NormalFOV = 90.0f;
	TargetingFOV = 45.0f;
	ViewPitchMin = -60.0f;
	ViewPitchMax = 60.0f;

	bAlwaysApplyModifiers = true;
}

void AFCPlayerCameraManager::UpdateCamera(float DeltaTime)
{
	//Widescreen adjustment
	int32 sizeX, sizeY, fAspectRatioAdjuster;
	PCOwner->GetViewportSize(sizeX, sizeY);
	FVector AdjustCameraLocation = GetCameraLocation();

	if (sizeX != 0 && sizeY != 0)
		fAspectRatioAdjuster = ((sizeX / 4) - (sizeY/3));

	//the rest
	AFCCharacter* MyPawn = PCOwner ? Cast<AFCCharacter>(PCOwner->GetPawn()) : NULL;

	if (MyPawn != NULL)
	{
		const float TargetFOV = MyPawn->IsTargeting() ? NormalFOV - 45.0f : NormalFOV;
		DefaultFOV = FMath::FInterpTo(DefaultFOV, TargetFOV, DeltaTime, 20.0f) ;
	}

	Super::UpdateCamera(DeltaTime);

	if (MyPawn != NULL)
		MyPawn->OnCameraUpdate(GetCameraLocation(), GetCameraRotation(), fAspectRatioAdjuster);
}