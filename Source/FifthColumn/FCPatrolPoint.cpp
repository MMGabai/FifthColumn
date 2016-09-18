//Copyright (c) 2016, Mordechai M. Gabai

#include "FifthColumn.h"
#include "FCPatrolPoint.h"

void AFCPatrolPoint::NotifyActorBeginOverlap(class AActor* Other)
{
	if (!bPatrolPointHit && GetAttachParentActor() == Other)
	{
		Cast<AFCAIController>(Other->GetInstigatorController())->NextPatrolPoint();
		bPatrolPointHit = true;
	}
}

void AFCPatrolPoint::NotifyActorEndOverlap(class AActor* Other)
{
	//it should only be the attached actor, so other actors cannot ruin the patrol route
	if (GetAttachParentActor() == Other)
		bPatrolPointHit = false;
}


