//Copyright (c) 2016, Mordechai M. Gabai

#include "FifthColumn.h"
#include "FCObject.h"


// Sets default values
AFCObject::AFCObject(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	CollisionComp = ObjectInitializer.CreateDefaultSubobject<UCapsuleComponent>(this, TEXT("CollisionComp"));
	CollisionComp->SetSimulatePhysics(false);
	CollisionComp->InitCapsuleSize(60.0f, 60.0f);
	CollisionComp->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	RootComponent = CollisionComp;

	Mesh3P = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("Mesh"));
	Mesh3P->SetSimulatePhysics(true);
	Mesh3P->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	Mesh3P->bChartDistanceFactor = true;
	Mesh3P->bReceivesDecals = false;
	Mesh3P->CastShadow = true;
	Mesh3P->bOwnerNoSee = true;
	Mesh3P->SetCollisionObjectType(ECC_WorldDynamic);
	Mesh3P->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh3P->SetCollisionResponseToAllChannels(ECR_Block);
	Mesh3P->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore); //this one is important, removing this line may cause it to crash
	Mesh3P->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore); //this one is important, removing this line may cause it to crash

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
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

