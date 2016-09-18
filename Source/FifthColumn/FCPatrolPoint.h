//Copyright (c) 2016, Mordechai M. Gabai

#pragma once

#include "Engine/TargetPoint.h"
#include "FCPatrolPoint.generated.h"

/**
 * 
 */
UCLASS(abstract)
class FIFTHCOLUMN_API AFCPatrolPoint : public ATargetPoint
{
	GENERATED_BODY()

	virtual	void NotifyActorBeginOverlap(class AActor* Other) override;
	virtual void NotifyActorEndOverlap(class AActor* Other) override;

	bool bPatrolPointHit;
};
