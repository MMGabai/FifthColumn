//Copyright (c) 2017, Mordechai M. Gabai

#include "FifthColumn.h"
#include "ActivatableObject.h"

AActivatableObject::AActivatableObject() 
{
	Mesh3P->SetSimulatePhysics(true);
	Mesh3P->bOnlyOwnerSee = false;
	Mesh3P->bOwnerNoSee = false;
	Mesh3P->bCastDynamicShadow = false;
	Mesh3P->bReceivesDecals = false;
	Mesh3P->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	Mesh3P->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	Mesh3P->SetCollisionObjectType(ECC_WorldStatic);
	Mesh3P->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh3P->SetCollisionResponseToAllChannels(ECR_Block);
	RootComponent = Mesh3P;

	PrimaryActorTick.bCanEverTick = false;
	bIsActive = false;
	PickedUpBy = NULL;

	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	FactionAffiliation = NULL;
	FactionChange = 5;
}

void AActivatableObject::BeginPlay() 
{
	Super::BeginPlay();
	bIsActive = true;
}

//To trigger activation events
void AActivatableObject::Activate(class AFCCharacter* Pawn, bool bForce) 
{
	PickedUpBy = Pawn;
	AFCPlayerCharacter* PC = Cast<AFCPlayerCharacter>(Pawn);

	if (bIsReadable)
	{
		PC->SetCurrentBook(this);
		PC->SetIsReading(true);
	}

	if (Pawn->IsAlive())
		OnActivatedEvent();

	if (FactionAffiliation > 0)
		Cast<AFCPlayerCharacter>(Pawn)->SetDispositionTowardsCharacter(FactionAffiliation, -FactionChange);

	bAutoDestroy ? Destroy() : NULL;
}

FText AActivatableObject::GetBookText() const
{
	return BookText;
}

//Replication
void AActivatableObject::Tick(float DeltaSeconds)
{
	//bIsActive ? CollisionComp->SetWorldLocation(Mesh3P->GetComponentLocation()) : NULL;
}

void AActivatableObject::OnRep_IsActive()
{
	!bIsActive ? bIsActive : NULL;
}