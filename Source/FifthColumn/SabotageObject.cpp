//Copyright (c) 2017, Mordechai M. Gabai

#include "FifthColumn.h"
#include "SabotageObject.h"

ASabotageObject::ASabotageObject()
{
	bCanBeDestroyed = true;
	bIsActive = true;

	Mesh3P->SetSimulatePhysics(false);
	Mesh3P->bReceivesDecals = false;
	Mesh3P->bChartDistanceFactor = false;
	Mesh3P->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	Mesh3P->SetCollisionObjectType(ECC_WorldDynamic);
	Mesh3P->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh3P->SetCollisionResponseToAllChannels(ECR_Block);
	Mesh3P->SetCollisionResponseToChannel(COLLISION_PICKUP, ECR_Ignore);
	Mesh3P->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

bool ASabotageObject::GetSabotaged()
{
	return (bIsActive == false);
}

//interaction with actors
void ASabotageObject::Activate(class AFCCharacter* Pawn, bool bForce = true)
{
	AFCPlayerCharacter* Target = Cast<AFCPlayerCharacter>(Pawn);

	if (Target)
	{
		AInventoryItem* KeyToUse = GetWorld()->SpawnActor<AInventoryItem>(Key, FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f));

		//do sabotage skill check
		if (Target->GetSkill(9) >= Level && Target->GetHasWeaponsHolstered() && bIsActive)
		{
			Target->AddXP(XPGained);
			FString XPGainedText = FString::FromInt(XPGained);
			Target->SendNotification(FText::FromString("You subtly molest this apparatus and gain " + XPGainedText + "XP"));
			bIsActive = false;

			if (FactionAffiliation > 0)
				Target->SetDispositionTowardsCharacter(FactionAffiliation, -FactionChange, true);

			OnActivatedEvent();
		}
		//sabotage skill check failed
		else if (bIsActive)
		{
			FString DoorLevelText = FString::FromInt(GetLevel());
			Target->SendNotification(FText::FromString("This level " + DoorLevelText + " apparatus proves too challeging for you to sabotage."));
			return;
		}
	}

	TArray<AActor*> AttachedActors;
	GetAttachedActors(AttachedActors);

	for (int32 i = 0; i < AttachedActors.Num(); i++)
	{
		if (Cast<ADoor>(AttachedActors[i]))
			Cast<ADoor>(AttachedActors[i])->Activate(Pawn, true);

		if (Cast<AActivatableObject>(AttachedActors[i]))
			Cast<AActivatableObject>(AttachedActors[i])->Activate(Pawn, true);
	}
}

int32 ASabotageObject::GetLevel() const
{
	return Level;
}

int32 ASabotageObject::GetXPGained() const
{
	return XPGained;
}

bool ASabotageObject::GetbCanBeDestroyed() const
{
	return bCanBeDestroyed;
}
