//Copyright (c) 2016, Mordechai M. Gabai

#include "FifthColumn.h"
#include "Door.h"

ADoor::ADoor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) 
{
	CollisionComp->SetSimulatePhysics(false);
	CollisionComp->InitCapsuleSize(100.0f, 150.0f);
	CollisionComp->SetCollisionObjectType(COLLISION_PICKUP);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RootComponent = CollisionComp;

	DoorOpeningSpeed = 2.5f;
	OpeningAngle = 80.0f;
	bDoorOpened = false;
	bLocked = false;
	bCanBeDestroyed = true;
	
	Mesh3P->SetSimulatePhysics(false);
	Mesh3P->bReceivesDecals = false;
	Mesh3P->bChartDistanceFactor = false;
	Mesh3P->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	Mesh3P->SetCollisionObjectType(ECC_WorldDynamic);
	Mesh3P->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh3P->SetCollisionResponseToAllChannels(ECR_Block);
	Mesh3P->SetCollisionResponseToChannel(COLLISION_PICKUP, ECR_Ignore);
	Mesh3P->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	DoorFeelingCapsule = ObjectInitializer.CreateDefaultSubobject<UCapsuleComponent>(this, TEXT("DoorFeelingCapsule"));
	DoorFeelingCapsule->AttachParent = Mesh3P;
	DoorFeelingCapsule->SetCapsuleHalfHeight(135.0f);
	DoorFeelingCapsule->SetCapsuleRadius(100.0f);
	DoorFeelingCapsule->SetRelativeLocation(FVector(50.0f, 0.0f, 20.0f));
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
	AFCPlayerController* TargetController = Cast<AFCPlayerController>(Pawn->GetController());

	if (!bDoorOpened)
	{
		if (bLocked && Target)
		{
			AInventoryItem* KeyToUse = GetWorld()->SpawnActor<AInventoryItem>(Key, FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f));

			//if player can sabotage the lock
			if (bForce)
				bDoorOpened = true;

			//if player has key, open it
			else if (Target->HasItem(KeyToUse))
			{
				TargetController->SendNotification(FText::FromString("You have unlocked the door with the key."));
				bLocked = false;
				bDoorOpened = true;
			}
			//do lockpickskill check
			else if (Target->GetSkill(7) >= DoorLevel)
			{
				Target->AddXP(XPGained);
				FString XPGainedText = FString::FromInt(XPGained);
				TargetController->SendNotification(FText::FromString("You deftly pick the lock of this door and gain " + XPGainedText + "XP"));
				bLocked = false;
				bDoorOpened = true;
			}
			//lockpick skill check failed and keep door shut
			else
			{
				Target->HudRunoffTime = 6.0f;
				FString DoorLevelText = FString::FromInt(GetDoorLevel());
				UGameplayStatics::PlaySoundAtLocation(this, LockedSound, GetActorLocation());
				TargetController->SendNotification(FText::FromString("This level " + DoorLevelText + " lock proves too challeging for you to pick."));
				return;
			}
		}
		else
			bDoorOpened = true;

		bDoorOpened ? UGameplayStatics::PlaySoundAttached(OpeningSound, GetRootComponent()) : UGameplayStatics::PlaySoundAttached(LockedSound, GetRootComponent());
	}
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

TSubclassOf<AInventoryItem> ADoor::GetKey() const
{
	return Key;
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