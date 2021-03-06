//Copyright (c) 2017, Mordechai M. Gabai

#include "FifthColumn.h"
#include "Door.h"

ADoor::ADoor() 
{
	DoorOpeningSpeed = 2.5f;
	OpeningAngle = 80.0f;
	bDoorOpened = false;
	bLocked = false;
	bCanBeDestroyed = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DoorAngle"));
	SceneComponent->bAutoActivate = true;
	RootComponent = SceneComponent;
	
	Mesh3P->AttachParent = SceneComponent;
	Mesh3P->SetSimulatePhysics(false);
	Mesh3P->bReceivesDecals = false;
	Mesh3P->bChartDistanceFactor = false;
	Mesh3P->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	Mesh3P->SetCollisionObjectType(ECC_WorldDynamic);
	Mesh3P->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh3P->SetCollisionResponseToAllChannels(ECR_Block);
	Mesh3P->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	PrimaryActorTick.bCanEverTick = true;
}

void ADoor::PostInitializeComponents() 
{
	Super::PostInitializeComponents();
	Yaw = RootComponent->GetComponentRotation().Yaw;
}

//interaction with actors
void ADoor::Activate(class AFCCharacter* Pawn, bool bForce)
{
	AFCPlayerCharacter* Target = Cast<AFCPlayerCharacter>(Pawn);

	if (bLocked && Target)
	{
		if (!bDoorOpened)
		{
			//FString* KeyToUse;

			//if player can sabotage the lock
			if (bForce)
				bDoorOpened = true;

			//if player has key, open it
			/*else if (KeyToUse == Key)
			{
				Target->SendNotification(FText::FromString("You have unlocked the door with the key."));
				bLocked = false;
				bDoorOpened = true;
			}*/
			//do lockpickskill check
			else if (Target->GetSkill(7) >= DoorLevel)
			{
				Target->AddXP(XPGained);
				FString XPGainedText = FString::FromInt(XPGained);
				Target->SendNotification(FText::FromString("You deftly pick the lock of this door and gain " + XPGainedText + "XP"));
				bLocked = false;
				bDoorOpened = true;
			}
			//lockpick skill check failed and keep door shut
			else
			{
				FString DoorLevelText = FString::FromInt(GetDoorLevel());
				UGameplayStatics::PlaySoundAtLocation(this, LockedSound, GetActorLocation());
				Target->SendNotification(FText::FromString("This level " + DoorLevelText + " lock proves too challeging for you to pick."));
				return;
			}
		}
		else
			bDoorOpened = false;
	}
	else if(!bDoorOpened)
		bDoorOpened = true;
	else
		bDoorOpened = false;

	bDoorOpened ? UGameplayStatics::SpawnSoundAttached(OpeningSound, GetRootComponent()) : UGameplayStatics::SpawnSoundAttached(LockedSound, GetRootComponent());

}

void ADoor::OpenDoor(float DeltaSeconds) 
{
	const FRotator DoorOpeningAngle = FMath::RInterpTo(GetActorRotation(), 
		FRotator(GetActorRotation().Pitch, Yaw + OpeningAngle, GetActorRotation().Roll), DeltaSeconds, DoorOpeningSpeed);
	SetActorRotation(DoorOpeningAngle);
}

void ADoor::CloseDoor(float DeltaSeconds) 
{
	const FRotator DoorClosingAngle = FMath::RInterpTo(GetActorRotation(), 
		FRotator(GetActorRotation().Pitch, Yaw, GetActorRotation().Roll), DeltaSeconds, DoorOpeningSpeed);
	SetActorRotation(DoorClosingAngle);
}

int32 ADoor::GetDoorLevel() const
{
	return DoorLevel;
}

int32 ADoor::GetXPGained() const
{
	return XPGained;
}

bool ADoor::GetbLocked() const
{
	return bLocked;
}

bool ADoor::GetbCanBeDestroyed() const
{
	return bCanBeDestroyed;
}

void ADoor::Tick(float DeltaSeconds) 
{
	AActor::Tick(DeltaSeconds);

	if (bDoorOpened == true)
		OpenDoor(DeltaSeconds);
	else if (GetActorRotation().Yaw != Yaw)
		CloseDoor(DeltaSeconds);
}