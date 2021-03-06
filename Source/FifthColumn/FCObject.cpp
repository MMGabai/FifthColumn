//Copyright (c) 2017, Mordechai M. Gabai

#include "FifthColumn.h"
#include "FCObject.h"


// Sets default values
AFCObject::AFCObject(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
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

	PrimaryActorTick.bCanEverTick = true;

}

//REPLICATION & EFFECTS
void AFCObject::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

FString AFCObject::GetAbstractName() const
{
	return AbstractName;
}

//INVENTORY
FText AFCObject::GetName() const
{
	return ObjectName;
}

bool AFCObject::GetIsActive() const
{
	return bIsActive;
}

AActor* AFCObject::GetCharacter() const
{
	return PickedUpBy;
}

void AFCObject::AttachMeshToPawn()
{
	DetachMeshFromPawn();
}

void AFCObject::DetachMeshFromPawn()
{
	Mesh3P ? Mesh3P->DetachFromParent() : NULL;
}

//REPLICATION
void AFCObject::OnRep_IsActive()
{
	return;
}

void AFCObject::OnRep_MyPawn()
{
	return;
}

// Called when the game starts or when spawned
void AFCObject::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AFCObject::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

