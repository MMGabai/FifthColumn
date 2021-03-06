//Copyright (c) 2017, Mordechai M. Gabai

#include "FifthColumn.h"
#include "DisturbanceSource.h"

ADisturbanceSource::ADisturbanceSource(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Loudness = 25;
	InitialLifeSpan = 0.5f;
	bIsHeard = false;
}

int32 ADisturbanceSource::GetLoudness() const
{
	return Loudness;
}

bool ADisturbanceSource::GetHeardStatus() const
{
	return bIsHeard;
}

AActor* ADisturbanceSource::GetInstigator() const
{
	return Instigator;
}

void ADisturbanceSource::SetHeardStatus()
{
	bIsHeard = true;
}
