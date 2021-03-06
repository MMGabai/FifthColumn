//Copyright (c) 2016, Mordechai M. Gabai

#pragma once

#include "Engine/TargetPoint.h"
#include "FCPatrolPoint.generated.h"

/**
 * 
 */
UCLASS()
class FIFTHCOLUMN_API AFCPatrolPoint : public ATargetPoint
{
	GENERATED_BODY()

	AFCPatrolPoint();

	virtual	void NotifyActorBeginOverlap(class AActor* Other) override;
	virtual void NotifyActorEndOverlap(class AActor* Other) override;

	//box a character will have to overlap in order to force the next checkpoint
	UPROPERTY(VisibleDefaultsOnly, Category = Gameplay)
		UBoxComponent* PatrolPointBox;

	bool bPatrolPointHit;
};
