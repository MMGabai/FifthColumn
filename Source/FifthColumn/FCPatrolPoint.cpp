//Copyright (c) 2016, Mordechai M. Gabai

#include "FifthColumn.h"
#include "FCPatrolPoint.h"

AFCPatrolPoint::AFCPatrolPoint()
{
	PatrolPointBox = CreateDefaultSubobject<UBoxComponent>(TEXT("PatrolPointBox"));
	//PatrolPointBox->AttachParent = GetSpriteComponent();
	PatrolPointBox->SetBoxExtent(FVector(64.0f, 64.0f, 64.0f), false);
	PatrolPointBox->bVisible = false;
	PatrolPointBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PatrolPointBox->SetCollisionResponseToAllChannels(ECR_Overlap);
}

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


