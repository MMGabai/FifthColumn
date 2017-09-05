//Copyright (c) 2017, Mordechai M. Gabai

#pragma once

#include "FifthColumn.h"
#include "Particles/ParticleSystemComponent.h"

AFCWeapon::AFCWeapon() 
{
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh1P"));
	Mesh1P->SetSimulatePhysics(false);
	Mesh1P->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	Mesh1P->bChartDistanceFactor = false;
	Mesh1P->bReceivesDecals = false;
	Mesh1P->CastShadow = false;
	Mesh1P->bOnlyOwnerSee = true;
	Mesh1P->SetCollisionObjectType(ECC_Pawn);
	Mesh1P->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	Mesh1P->SetCollisionResponseToAllChannels(ECR_Block);
	Mesh1P->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	Mesh1P->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	Mesh1P->AttachParent = Mesh3P;

	//Mesh3P->AttachParent = Mesh1P;

	WeaponCameraSettings = FVector(0.0f, 0.0f, 9.0f);

	bLoopedMuzzleFX = false;
	bIsAutomatic = false;
	bPlayingFireAnim = false;
	bIsEquipped = false;
	bWantsToFire = false;
	bPendingReload = false;
	bPendingEquip = false;
	CurrentState = EWeaponState::Idle;

	bIsSilenced = false;

	CurrentAmmo = 0;
	CurrentAmmoInClip = 0;
	LastFireTime = 0.0f;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	bNetUseOwnerRelevancy = true;
}

void AFCWeapon::BeginPlay() 
{
	Super::BeginPlay();
	SpawnPickup();
}

void AFCWeapon::PostInitializeComponents() 
{
	Super::PostInitializeComponents();

	if (WeaponConfig.InitialClips > 0)
	{
		CurrentAmmoInClip = WeaponConfig.AmmoPerClip;
		CurrentAmmo = WeaponConfig.AmmoPerClip * WeaponConfig.InitialClips;
	}
}

void AFCWeapon::Destroyed() 
{
	Super::Destroyed();
	StopSimulatingWeaponFire();
}

void AFCWeapon::SetLocation(const FVector LocationReceived)
{
	//CollisionComp->SetWorldLocation(LocationReceived);
	Mesh1P->SetWorldLocation(LocationReceived);
	Mesh3P->SetWorldLocation(LocationReceived);
	Super::SetActorLocation(LocationReceived);
}

void AFCWeapon::SpawnPickup()
{
	bIsActive = true;
	TArray<AActor*> OverlappingPawns;
}

void AFCWeapon::PickupOnTouch(class AFCPlayerCharacter* Pawn)
{
	if (bIsActive == true)
	{
		if (PickupSound)
			UGameplayStatics::SpawnSoundAttached(PickupSound, GetRootComponent());

		if (Pawn->AddWeapon(this))
		{
			SetOwner(Pawn);
			OnPickedUp();
			PickedUpBy = Pawn;
			bIsActive = false;
		}
	}
}

void AFCWeapon::OnPickedUp()
{
	//CollisionComp->Deactivate();
	Mesh3P->Deactivate();
	DetachMeshFromPawn();

	OnPickedUpEvent();
}

int32 AFCWeapon::GetWeaponType() const 
{
	return iWeaponType;
}

void AFCWeapon::OnEquip() 
{
	if (MyPawn->GetHasWeaponsHolstered() == false)
	{
		AttachMeshToPawn();
		bPendingEquip = true;
		float Duration = 0.5f;

		if (EquipAnim)
			Duration = EquipAnim->SequenceLength;

		GetWorldTimerManager().SetTimer(EquipFinishedHandle, this, &AFCWeapon::OnEquipFinished, Duration, false);
		DetermineWeaponState();

		if (MyPawn->GetHasWeaponsHolstered() == false && iWeaponType == 4)
			Cast<AFCPlayerCharacter>(MyPawn)->ToggleHolster();

		if (MyPawn && MyPawn->IsLocallyControlled())
			PlayWeaponSound(EquipSound);
	}
}

void AFCWeapon::OnEquipFinished()
{
	AttachMeshToPawn();

	bIsEquipped = true;
	bPendingEquip = false;

	DetermineWeaponState();
}

void AFCWeapon::OnUnEquip() 
{
	bIsEquipped = false;
	GetWorldTimerManager().SetTimer(EquipFinishedHandle, this, &AFCWeapon::DetachMeshFromPawn, 0.5f, false);
	StopFire();

	if (bPendingReload) 
	{
		bPendingReload = false;
		GetWorldTimerManager().SetTimer(EquipFinishedHandle, this, &AFCWeapon::StopReload, -1.f, false);
		GetWorldTimerManager().SetTimer(EquipFinishedHandle, this, &AFCWeapon::ReloadWeapon, -1.f, false);
	}

	if (bPendingEquip) 
	{
		bPendingEquip = false;
		GetWorldTimerManager().SetTimer(EquipFinishedHandle, this, &AFCWeapon::OnEquipFinished, -1.f, false);
	}

	DetermineWeaponState();
}

void AFCWeapon::OnEnterInventory(AFCCharacter* NewOwner) 
{
	SetOwningPawn(NewOwner);

	if (Mesh3P)
	{
		Mesh3P->SetSimulatePhysics(false);
		Mesh3P->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		Mesh3P->SetRelativeRotation(FRotator(0.0f, 0.0f, -90.0f));
	}
}

void AFCWeapon::OnLeaveInventory()
{
	if (Role == ROLE_Authority)
		SetOwningPawn(NULL);

	if (IsAttachedToPawn())
		OnUnEquip();

	bIsActive = true;
}

void AFCWeapon::AttachMeshToPawn() 
{
	if (MyPawn) 
	{
		// Remove and hide both first and third person meshes
		DetachMeshFromPawn();

		Mesh3P ? Mesh3P->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f)) : NULL;

		// For locally controller players we attach both weapons and let the bOnlyOwnerSee, bOwnerNoSee flags deal with visibility.
		FName AttachPoint = MyPawn->GetWeaponAttachPoint();
		if (MyPawn->IsLocallyControlled() == true)
		{
			USkeletalMeshComponent* PawnMesh1p;
			AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(MyPawn);

			if (Player)
			{
				PawnMesh1p = Player->Mesh1P;
				Mesh1P->SetHiddenInGame(false);
				Mesh1P->AttachTo(PawnMesh1p, AttachPoint);
			}

			USkeletalMeshComponent* PawnMesh3p = MyPawn->GetMesh();

			if (Mesh3P)
			{
				Mesh3P->AttachTo(PawnMesh3p, AttachPoint);
				Mesh3P->SetHiddenInGame(false);
			}
		}
		else
		{
			USkeletalMeshComponent* UseWeaponMesh = GetWeaponMesh();
			USkeletalMeshComponent* UsePawnMesh = MyPawn->GetMesh();
			UseWeaponMesh->AttachTo(UsePawnMesh, AttachPoint);
			UseWeaponMesh->SetHiddenInGame(false);
		}
	}
}

void AFCWeapon::DetachMeshFromPawn()
{
	Mesh1P ? Mesh1P->DetachFromParent() : NULL;
	Super::DetachMeshFromPawn();
}

//INPUT
void AFCWeapon::StartFire()
{
	if (!(GetPawnOwner()->IsRunning()) && !bWantsToFire)
	{
		bWantsToFire = true;
		DetermineWeaponState();
	}
}

void AFCWeapon::StopFire()
{
	if (bWantsToFire)
	{
		bWantsToFire = false;
		DetermineWeaponState();
	}
}

void AFCWeapon::StartReload(bool bFromReplication)
{
	GetWorldTimerManager().SetTimer(ReloadWeaponHandle, this, &AFCWeapon::StopFire, 0.1f, false);

	if (bFromReplication || CanReload())
	{
		bPendingReload = true;
		DetermineWeaponState();
		if (ReloadAnim)
		{
			float AnimDuration = ReloadAnim->SequenceLength;

			if (AnimDuration <= 0.0f)
				AnimDuration = WeaponConfig.NoAnimReloadDuration;

			GetWorldTimerManager().SetTimer(StopReloadHandle, this, &AFCWeapon::StopReload, AnimDuration, false);
			if (Role == ROLE_Authority)
				GetWorldTimerManager().SetTimer(ReloadWeaponHandle, this, &AFCWeapon::ReloadWeapon, FMath::Max(0.1f, AnimDuration - 0.1f), false);
		}
		else
			StopReload();
		
		if (MyPawn && MyPawn->IsLocallyControlled())
			PlayWeaponSound(ReloadSound);
	}
}

void AFCWeapon::StartReloadNoPar()
{
	StartReload(false);
}

void AFCWeapon::StopReload()
{
	if (CurrentState == EWeaponState::Reloading)
	{
		bPendingReload = false;
		DetermineWeaponState();
	}
}

//CONTROL
bool AFCWeapon::CanFire() const
{
	bool bCanFire = MyPawn && MyPawn->CanFire();
	bool bStateOKToFire = ((CurrentState == EWeaponState::Idle) || (CurrentState == EWeaponState::Firing));
	return ((bCanFire == true) && (bStateOKToFire == true) && (bPendingReload == false));
}

bool AFCWeapon::CanReload() const
{
	bool bCanReload = (!MyPawn || MyPawn->CanReload());
	bool bGotAmmo = MyPawn->GetUnlimitedClips() || ( CurrentAmmoInClip < WeaponConfig.AmmoPerClip) && (CurrentAmmo - CurrentAmmoInClip > 0);
	bool bStateOKToReload = ((CurrentState ==  EWeaponState::Idle ) || ( CurrentState == EWeaponState::Firing));
	return (bCanReload == true && bGotAmmo == true && bStateOKToReload == true);	
}

//WEAPONS
void AFCWeapon::GiveAmmo(int AddAmount)
{
	int32 MissingAmmo = FMath::Max(0, WeaponConfig.MaxAmmo - CurrentAmmo);

	if (MyPawn->GetUnlimitedClips())
		MissingAmmo = WeaponConfig.AmmoPerClip;

	AddAmount = FMath::Min(AddAmount, MissingAmmo);
	CurrentAmmo += AddAmount;
	
	// start reload if clip was empty
	if (GetCurrentAmmoInClip() <= 0 &&
		CanReload() &&
		MyPawn->GetWeapon() == this)
		GetWorldTimerManager().SetTimer(ReloadWeaponHandle, this, &AFCWeapon::StartReloadNoPar, 0.5f, false);
}

void AFCWeapon::UseAmmo()
{
	if (MyPawn->GetUnlimitedClips() == false)
	{
		CurrentAmmoInClip--;
		CurrentAmmo--;
	}

	APlayerController* PlayerController = MyPawn ? Cast<APlayerController>(MyPawn->GetController()) : NULL;
}

void AFCWeapon::HandleFiring()
{
	const bool bShouldUpdateAmmo = (CurrentAmmoInClip > 0 && CanFire());

	if (bShouldUpdateAmmo)
	{
		if (Role == ROLE_Authority && CurrentState != EWeaponState::Firing)
			return;

		if (!bIsSilenced)
			GetWorld()->SpawnActor<ADisturbanceSource>(LoudnessModel, GetPawnOwner()->GetActorLocation(), FRotator::ZeroRotator);

		PlayWeaponSound(FireSound);

		if (MuzzleFX)
		{
			USkeletalMeshComponent* UseWeaponMesh = GetWeaponMesh();
			if (!bLoopedMuzzleFX || MuzzlePSC == NULL)
				MuzzlePSC = UGameplayStatics::SpawnEmitterAttached(MuzzleFX, UseWeaponMesh, MuzzleAttachPoint);
		}

		if (EjectionFX)
		{
			USkeletalMeshComponent* UseWeaponMesh = GetWeaponMesh();
			EjectionPSC == NULL ? EjectionPSC = UGameplayStatics::SpawnEmitterAttached(EjectionFX, UseWeaponMesh, EjectAttachPoint) : NULL;
		}

		APlayerController* PC = (MyPawn != NULL) ? Cast<APlayerController>(MyPawn->Controller) : NULL;
		if (PC != NULL && PC->IsLocalController())
		{
			FireCameraShake != NULL ? PC->ClientPlayCameraShake(FireCameraShake, 10) : NULL;
			FireForceFeedback != NULL ? PC->ClientPlayForceFeedback(FireForceFeedback, false, "Weapon") : NULL;
		}

		!bIsAutomatic ? GetWorldTimerManager().SetTimer(FiringHandle, this, &AFCWeapon::StopSimulatingWeaponFire, WeaponConfig.TimeBetweenShots, false) : NULL;

		if (MyPawn && MyPawn->IsLocallyControlled())
		{
			FireWeapon();
			UseAmmo();
		}
	}
	else if (CanReload())
		GetWorldTimerManager().SetTimer(ReloadWeaponHandle, this, &AFCWeapon::StartReloadNoPar, 0.5f, false);
	else if (MyPawn && MyPawn->IsLocallyControlled() && GetCurrentAmmo() == 0 && !bRefiring)
	{
		PlayWeaponSound(OutOfAmmoSound);
		APlayerController* MyPC = Cast<APlayerController>(MyPawn->Controller);
	}

	if (MyPawn && MyPawn->IsLocallyControlled())
	{
		// reload after firing last round
		CurrentAmmoInClip <= 0 && CanReload() ? GetWorldTimerManager().SetTimer(ReloadWeaponHandle, this, &AFCWeapon::StartReloadNoPar, 0.5f, false) : NULL;

		// setup refire timer
		bRefiring = (CurrentState == EWeaponState::Firing && WeaponConfig.TimeBetweenShots > 0.0f);
		bRefiring ? GetWorldTimerManager().SetTimer(FiringHandle, this, &AFCWeapon::HandleFiring, WeaponConfig.TimeBetweenShots, false) : NULL;
	}

	LastFireTime = GetWorld()->GetTimeSeconds();
}

void AFCWeapon::ReloadWeapon()
{
	const int32 ClipDelta = FMath::Min(WeaponConfig.AmmoPerClip - CurrentAmmoInClip, CurrentAmmo - CurrentAmmoInClip);

	if (ClipDelta > 0)
		CurrentAmmoInClip += ClipDelta;

	CurrentAmmo = FMath::Max(CurrentAmmoInClip, CurrentAmmo);
}

void AFCWeapon::SetWeaponState(EWeaponState::Type NewState)
{
	const EWeaponState::Type PrevState = CurrentState;

	if (PrevState == EWeaponState::Firing && NewState != EWeaponState::Firing)
		OnFireFinished();

	CurrentState = NewState;

	if (PrevState != EWeaponState::Firing && NewState == EWeaponState::Firing)
		OnFireStarted();
}

void AFCWeapon::DetermineWeaponState()
{
	EWeaponState::Type NewState = EWeaponState::Idle;

	if (bIsEquipped && bPendingReload)
	{
		if (CanReload() == false)
			NewState = CurrentState;
		else
			NewState = EWeaponState::Reloading;
	}
	else if (bWantsToFire == true && CanFire() == true)
		NewState = EWeaponState::Firing;
	else if (bPendingEquip)
		NewState = EWeaponState::Equipping;

	SetWeaponState(NewState);
}

void AFCWeapon::OnFireStarted() 
{
	GetWorldTimerManager().ClearTimer(FiringHandle);
	const float GameTime = GetWorld()->GetTimeSeconds();
	if (LastFireTime > 0 && WeaponConfig.TimeBetweenShots > 0.0f &&
		LastFireTime + WeaponConfig.TimeBetweenShots > GameTime)
		GetWorldTimerManager().SetTimer(FiringHandle, this, &AFCWeapon::HandleFiring, LastFireTime + WeaponConfig.TimeBetweenShots - GameTime, false);
	else
		HandleFiring();
}

void AFCWeapon::OnFireFinished()
{
	if (bIsAutomatic && FiringHandle.IsValid())
		GetWorldTimerManager().SetTimer(FiringHandle, this, &AFCWeapon::StopSimulatingWeaponFire, 0.05f, false);
	else
		GetWorldTimerManager().SetTimer(FiringHandle, this, &AFCWeapon::StopSimulatingWeaponFire, 0.1f, false);

	//HandleFiring();
	//GetWorldTimerManager().SetTimer(FiringHandle, this, &AFCWeapon::HandleFiring, -1.f, false);
	bRefiring = false;
}

UAudioComponent* AFCWeapon::PlayWeaponSound(USoundCue* Sound) 
{
	UAudioComponent* AC = NULL;
	if (Sound && MyPawn)
		AC = UGameplayStatics::SpawnSoundAttached(Sound, MyPawn->GetRootComponent());

	return AC;
}

FVector AFCWeapon::GetWeaponCameraSettings() const
{
	return WeaponCameraSettings;
}

FVector AFCWeapon::GetCameraAim() const 
{
	APlayerController* const PlayerController = Instigator ? Cast<APlayerController>(Instigator->Controller) : NULL;
	FVector FinalAim = FVector::ZeroVector;

	if (PlayerController) 
	{
		FVector CamLoc;
		FRotator CamRot;
		PlayerController->GetPlayerViewPoint(CamLoc, CamRot);
		FinalAim = CamRot.Vector();
	}
	else if (Instigator) 
		FinalAim = Instigator->GetBaseAimRotation().Vector();

	return FinalAim;
}

FVector AFCWeapon::GetAdjustedAim() const 
{
	APlayerController* const PlayerController = Instigator ? Cast<APlayerController>(Instigator->Controller) : NULL;
	FVector FinalAim = FVector::ZeroVector;
	if (PlayerController) 
	{
		FVector CamLoc;
		FRotator CamRot;
		PlayerController->GetPlayerViewPoint(CamLoc, CamRot);
		FinalAim = CamRot.Vector();
	}
	else if (Instigator) 
	{
		AAIController* AIController = MyPawn ? Cast<AAIController>(MyPawn->Controller) : NULL;
		if(AIController != NULL )
			FinalAim = AIController->GetControlRotation().Vector();
		else
			FinalAim = Instigator->GetBaseAimRotation().Vector();
	}

	return FinalAim;
}

FVector AFCWeapon::GetCameraDamageStartLocation(const FVector& AimDir) const 
{
	APlayerController* PC = MyPawn ? Cast<APlayerController>(MyPawn->Controller) : NULL;
	FVector OutStartTrace = FVector::ZeroVector;

	OutStartTrace = GetMuzzleLocation();

	return OutStartTrace;
}

FVector AFCWeapon::GetMuzzleLocation() const 
{
	USkeletalMeshComponent* UseMesh = GetWeaponMesh();
	return UseMesh->GetSocketLocation(MuzzleAttachPoint);
}

FVector AFCWeapon::GetMuzzleDirection() const 
{
	USkeletalMeshComponent* UseMesh = GetWeaponMesh();
	return UseMesh->GetSocketRotation(MuzzleAttachPoint).Vector();
}

FHitResult AFCWeapon::GetWeaponTrace(const FVector& StartTrace, const FVector& EndTrace) const 
{
	return WeaponTrace(StartTrace, EndTrace);
}

FHitResult AFCWeapon::WeaponTrace(const FVector& StartTrace, const FVector& EndTrace) const 
{
	static FName WeaponFireTag = FName(TEXT("WeaponTrace"));

	// Perform trace to retrieve hit info
	FCollisionQueryParams TraceParams(WeaponFireTag, true, Instigator);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, COLLISION_WEAPON, TraceParams);

	return Hit;
}

void AFCWeapon::SetOwningPawn(AFCCharacter* NewOwner) 
{
	if (MyPawn != NewOwner) 
	{
		Instigator = NewOwner;
		MyPawn = NewOwner;
		SetOwner(NewOwner);
	}	
}

//REPLICATION & EFFECTS
void AFCWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AFCWeapon::OnRep_IsActive() 
{
	if (!bIsActive)
		OnPickedUp();
}

void AFCWeapon::OnRep_MyPawn()
{
	if (MyPawn)
		OnEnterInventory(MyPawn);
	else
		OnLeaveInventory();
}

void AFCWeapon::OnRep_Reload()
{
	if (bPendingReload)
		StartReload(true);
	else
		StopReload();
}

void AFCWeapon::StopSimulatingWeaponFire()
{
	if (bLoopedMuzzleFX)
	{
		if (MuzzlePSC != NULL)
		{
			MuzzlePSC->DeactivateSystem();
			MuzzlePSC = NULL;
		}

		if (MuzzlePSCSecondary != NULL)
		{
			MuzzlePSCSecondary->DeactivateSystem();
			MuzzlePSCSecondary = NULL;
		}

		if (EjectionPSC != NULL)
		{
			EjectionPSC->DeactivateSystem();
			EjectionPSC = NULL;
		}
	}
}

USkeletalMeshComponent* AFCWeapon::GetWeaponMesh() const
{
	return (MyPawn != NULL && MyPawn->IsFirstPerson()) ? Mesh1P : Mesh3P;
}

class AFCCharacter* AFCWeapon::GetPawnOwner() const
{
	return MyPawn;
}

UMaterialInstance* AFCWeapon::GetIcon() const
{
	return Icon;
}

bool AFCWeapon::IsEquipped() const 
{
	return bIsEquipped;
}

bool AFCWeapon::IsAttachedToPawn() const 
{
	return bIsEquipped || bPendingEquip;
}

EWeaponState::Type AFCWeapon::GetCurrentState() const 
{
	return CurrentState;
}

bool AFCWeapon::GetIsReloading() const 
{
	if (CurrentState == EWeaponState::Reloading)
		return true;
	else
		return false;
}

bool AFCWeapon::GetIsFiring() const 
{
	if (CurrentState == EWeaponState::Firing)
		return true;
	else
		return false;
}

bool AFCWeapon::GetIsEquipping() const
{
	if (CurrentState == EWeaponState::Equipping)
		return true;
	else
		return false;
}

int32 AFCWeapon::GetCurrentAmmo() const 
{
	return CurrentAmmo;
}

int32 AFCWeapon::GetCurrentAmmoInClip() const
{
	return CurrentAmmoInClip;
}

int32 AFCWeapon::GetAmmoPerClip() const 
{
	return WeaponConfig.AmmoPerClip;
}

int32 AFCWeapon::GetMaxAmmo() const 
{
	return WeaponConfig.MaxAmmo;
}

float AFCWeapon::GetEquipStartedTime() const 
{
	return EquipStartedTime;
}

float AFCWeapon::GetEquipDuration() const
{
	return EquipDuration;
}

void AFCWeapon::RagdollPhysics() 
{
	Mesh3P->SetSimulatePhysics(true);
}

//ANIMATIONS
UAnimSequence* AFCWeapon::GetIdleAnim() const 
{
	return IdleAnim;
}

//reload animations
UAnimSequence* AFCWeapon::GetReloadAnim() const
{
	return ReloadAnim;
}

//equip animations
UAnimSequence* AFCWeapon::GetEquipAnim() const 
{
	return EquipAnim;
}

//fire animations
UAnimSequence* AFCWeapon::GetFireAnim() const 
{
	return FireAnim;
}

UAnimSequence* AFCWeapon::GetRunAnim() const
{
	return RunAnim;
}

UAnimSequence* AFCWeapon::GetHardAimAnim() const 
{
	return HardAimAnim;
}

void AFCWeapon::Tick(float DeltaSeconds)
{
	RecoilDeltaSeconds = DeltaSeconds;

	if (!MyPawn) 
	{
		//CollisionComp->SetWorldLocation(Mesh3P->GetComponentLocation());
		Mesh3P->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	else if (Mesh3P)
		Mesh3P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}