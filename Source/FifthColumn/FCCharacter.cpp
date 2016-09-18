//Copyright (c) 2016, Mordechai M. Gabai

#pragma once
#include "FifthColumn.h"

AFCCharacter::AFCCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.SetDefaultSubobjectClass<UFCCharacterMovement>(ACharacter::CharacterMovementComponentName)) 
{	
	Level = 1;
	XP = 0;
	SkillPointsToGive = 0;
	IncreasedRunningPace = 1;
	XPRequiredForLevelling = 1000;
	WithdrawalEffect = false;
	BaseTimeUntilWithdrawal = 100000.0f;
	BaseStamina = 900.0f;
	TimeUntilWithdrawal = BaseTimeUntilWithdrawal;
	Health = MaxHealth;
	MaxHealth = 100;
	bIsInDialogue = false;
	CameraSettings = FVector(12.0f, 6.0f, 0.0f);

	Mesh1P = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("PawnMesh1P"));
	Mesh1P->AttachParent = GetCapsuleComponent();
	Mesh1P->bOnlyOwnerSee = true;
	Mesh1P->bTreatAsBackgroundForOcclusion = true;
	Mesh1P->bOwnerNoSee = false;
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->bReceivesDecals = true;
	Mesh1P->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	Mesh1P->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	Mesh1P->bChartDistanceFactor = false;
	Mesh1P->SetCollisionObjectType(ECC_Pawn);
	Mesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh1P->SetCollisionResponseToAllChannels(ECR_Block);

	GetMesh()->AttachParent = Mesh1P;
	GetMesh()->bOnlyOwnerSee = false;
	GetMesh()->bOwnerNoSee = true;
	GetMesh()->bReceivesDecals = false;
	GetMesh()->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	GetMesh()->SetCollisionObjectType(ECC_Pawn);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionResponseToAllChannels(ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(COLLISION_PROJECTILE, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(COLLISION_PICKUP, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	GetCapsuleComponent()->SetCollisionObjectType(ECC_Pawn);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_PROJECTILE, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	HeadshotBox = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("InstantKillBox"));
	HeadshotBox->AttachParent = Mesh1P;
	HeadshotBox->SetBoxExtent(FVector(10.0f, 10.0f, 10.0f), false);
	HeadshotBox->bHiddenInGame = true;
	HeadshotBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	HeadshotBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	HeadshotBox->SetCollisionResponseToChannel(COLLISION_PROJECTILE, ECR_Overlap);

	CrouchedEyeHeight = BaseEyeHeight;

	TargetingSpeedModifier = 0.5f;
	CrouchingSpeedModifier = 0.5f;
	bIsTargeting = false;
	RunningSpeedModifier = 1.5f;
	bWantsToRun = false;
	bWantsToRunToggled = false;
	bWantsToFire = false;
	bUnlimitedClips = false;
	bHasWeaponsHolstered = true;

	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	Inventory.AddZeroed(2);

	//skill related stuff
	iMaxEncumbranceLimit = 4;

	//speech skill
	ArmenianSkill = 0;
	AzerbaijaniSkill = 0;
	SovietSkill = 0;

	//utility skills
	LockpickSkill = 0;
	CombatSkill = 0;
	SabotageSkill = 0;

	//pistol skills
	PistolSkill = 0;
	AssaultSkill = 0;
	MarksmanSkill = 0;

	KnifeAnimationDuration = 0.0f;
	GrenadeAnimationDuration = 0.0f;
}

void AFCCharacter::SetInstantKill(bool Set = true)
{
	bInstantKill = Set;
}

void AFCCharacter::Landed(const FHitResult& Hit) 
{
	if (GetVelocity().Z < -700.0f)
	{
		FPointDamageEvent FallingDamage;
		FallingDamage.DamageTypeClass = FallingDamageType;
		TakeDamage(-GetVelocity().Z - 1400.0f, FallingDamage, GetController(), this);
	}

	Super::Landed(Hit);
}

//main code
void AFCCharacter::PostInitializeComponents() 
{
	Super::PostInitializeComponents();
	Health = GetMaxHealth();
	Stamina = BaseStamina;
	SpawnDefaultInventory();
}

void AFCCharacter::Destroyed() 
{
	Super::Destroyed();
	DestroyInventory();
}

void AFCCharacter::PawnClientRestart() 
{
	Super::PawnClientRestart();
	SetCurrentWeapon(CurrentWeapon);
}

void AFCCharacter::PossessedBy(class AController* InController) 
{
	Super::PossessedBy(InController);
}

void AFCCharacter::OnRep_PlayerState() 
{
	Super::OnRep_PlayerState();
}

void AFCCharacter::OnRep_LastTakeHitInfo()
{
	if (LastTakeHitInfo.bKilled)
		OnDeath(LastTakeHitInfo.ActualDamage, LastTakeHitInfo.GetDamageEvent(), LastTakeHitInfo.PawnInstigator.Get(), LastTakeHitInfo.DamageCauser.Get());
	else
		PlayHit(LastTakeHitInfo.ActualDamage, LastTakeHitInfo.GetDamageEvent(), LastTakeHitInfo.PawnInstigator.Get(), LastTakeHitInfo.DamageCauser.Get());
}

FRotator AFCCharacter::GetAimOffsets() const
{
	const FVector AimDirWS = GetBaseAimRotation().Vector();
	const FVector AimDirLS = ActorToWorld().InverseTransformVectorNoScale(AimDirWS);
	const FRotator AimRotLS = AimDirLS.Rotation();

	return AimRotLS;
}

void AFCCharacter::OnCameraUpdate(const FVector& CameraLocation, const FRotator& CameraRotation, int32& fWideScreenValue)
{	
	USkeletalMeshComponent* DefMesh1P = Cast<USkeletalMeshComponent>(GetClass()->GetDefaultSubobjectByName(TEXT("PawnMesh1P")));
	FMatrix DefMeshLS;
	FVector NewCameraSettings, WeaponCameraSettings;
	float WideScreenValue = fWideScreenValue;

	if (CurrentWeapon)
	{
		if (!IsTargeting())
			WeaponCameraSettings = CurrentWeapon->GetWeaponCameraSettings();
		else
			WeaponCameraSettings = CurrentWeapon->GetWeaponIronSightCameraSettings();
	}
	else
		WeaponCameraSettings = FVector::ZeroVector;

	if (bIsTargeting)
		WeaponCameraSettings += FVector(IronSightCameraSettings.X, IronSightCameraSettings.Y, IronSightCameraSettings.Z);
	else 
		WeaponCameraSettings += FVector(CameraSettings.X + (WideScreenValue / 24.0f), CameraSettings.Y, CameraSettings.Z);

	DefMeshLS = FRotationTranslationMatrix(DefMesh1P->RelativeRotation, DefMesh1P->RelativeLocation + WeaponCameraSettings);
	const FMatrix LocalToWorld = ActorToWorld().ToMatrixWithScale();

	// Mesh rotating code expect uniform scale in LocalToWorld matrix
	const FRotator RotCameraPitch(CameraRotation.Pitch, 0.0f, 0.0f);
	const FRotator RotCameraYaw(0.0f, CameraRotation.Yaw, 0.0f);

	const FMatrix LeveledCameraLS = FRotationTranslationMatrix(RotCameraYaw, CameraLocation) * LocalToWorld.Inverse();
	const FMatrix PitchedCameraLS = FRotationMatrix(RotCameraPitch) * LeveledCameraLS;
	const FMatrix MeshRelativeToCamera = DefMeshLS * LeveledCameraLS.Inverse();
	const FMatrix PitchedMesh = MeshRelativeToCamera * PitchedCameraLS;

	Mesh1P->SetRelativeLocationAndRotation(PitchedMesh.GetOrigin(), PitchedMesh.Rotator());
}


//////////////////////////////////////////////////////////////////////////
// Damage & death
void AFCCharacter::ThrowGrenade()
{
	HudRunoffTime = 6.0f;

	if (GrenadeClass != NULL && GrenadesCarried > 0 && !bHasWeaponsHolstered) 
	{
		const FRotator SpawnRotation = GetAimOffsets();

		const FVector DefaultGunOffset = FVector(100.0f, 30.0f, 10.0f);
		const FVector SpawnLocation = GetActorLocation() + SpawnRotation.RotateVector(DefaultGunOffset);

		UWorld* const World = GetWorld();
		if (World != NULL)
			World->SpawnActor<AFifthColumnProjectile>(GrenadeClass, SpawnLocation, GetControlRotation()); //this lets the player throw the grenade

		if (ThrowAnimation != NULL)
		{
			UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
			if (AnimInstance != NULL)
				AnimInstance->Montage_Play(ThrowAnimation, 1.f);
		}

		GrenadesCarried--;
	}
}

void AFCCharacter::FellOutOfWorld(const class UDamageType& dmgType) 
{
	Die(Health, FDamageEvent(dmgType.GetClass()), NULL, NULL);
}

void AFCCharacter::KilledBy(APawn* EventInstigator)
{
	if (!bIsDying)
	{
		AController* Killer = NULL;
		if (EventInstigator != NULL)
		{
			Killer = EventInstigator->Controller;
			LastHitBy = NULL;
		}

		Die(Health, FDamageEvent(UDamageType::StaticClass()), Killer, NULL);
	}
}

float AFCCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	APlayerController* MyPC = Cast<APlayerController>(Controller);

	if (Health <= 0.f)
		return 0.f;

	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f) 
	{
		Health -= ActualDamage;
		if (Health <= 0 || bInstantKill == true)
			Die(ActualDamage, DamageEvent, EventInstigator, DamageCauser);
		else
			PlayHit(ActualDamage, DamageEvent, EventInstigator ? EventInstigator->GetPawn() : NULL, DamageCauser);

		MakeNoise(1.0f, EventInstigator ? EventInstigator->GetPawn() : this);
	}

	return ActualDamage;
}

void AFCCharacter::AddHealth(float HealthAdd) 
{
	APlayerController* MyPC = Cast<APlayerController>(Controller);
	AFCCharacter* Character = Cast<AFCCharacter>(MyPC->GetCharacter());

	if (Health < GetMaxHealth() && !WithdrawalEffect)
		Health += HealthAdd;

	if (Health > GetMaxHealth())
		Health = GetMaxHealth();
}

void AFCCharacter::InduceDrugEffects(bool DrugPlayer)
{
	WithdrawalEffect = DrugPlayer;

	if (!WithdrawalEffect)
		TimeUntilWithdrawal = BaseTimeUntilWithdrawal;
}

float AFCCharacter::GetBaseTimeUntilWithdrawal() const 
{
	return BaseTimeUntilWithdrawal;
}

float AFCCharacter::GetBaseStamina() const 
{
	return BaseStamina;
}

float AFCCharacter::GetTimeUntilWithdrawal() const
{
	return TimeUntilWithdrawal;
}

float AFCCharacter::GetStamina() const
{
	return Stamina;
}

void AFCCharacter::RestockAllAmmo()
{
	if (Inventory[0])
		Inventory[0]->GiveAmmo(Inventory[0]->GetMaxAmmo() - Inventory[0]->GetCurrentAmmo());

	if (Inventory[1])
		Inventory[1]->GiveAmmo(Inventory[1]->GetMaxAmmo() - Inventory[1]->GetCurrentAmmo());
}

int32 AFCCharacter::GetMoney() const
{
	return Money;
}

bool AFCCharacter::CanDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser) const
{
	if ( bIsDying
		|| IsPendingKill()
		|| GetWorld()->GetAuthGameMode() == NULL)
		return false;

	return true;
}


bool AFCCharacter::Die(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser)
{
	if (!CanDie(KillingDamage, DamageEvent, Killer, DamageCauser))
		return false;

	Health = FMath::Min(0.0f, Health);

	UDamageType const* const DamageType = DamageEvent.DamageTypeClass ? DamageEvent.DamageTypeClass->GetDefaultObject<UDamageType>() : GetDefault<UDamageType>();
	Killer = GetDamageInstigator(Killer, *DamageType);

	AController* const KilledPlayer = (Controller != NULL) ? Controller : Cast<AController>(GetOwner());

	NetUpdateFrequency = GetDefault<AFCCharacter>()->NetUpdateFrequency;
	GetCharacterMovement()->ForceReplicationUpdate();

	OnDeath(KillingDamage, DamageEvent, Killer ? Killer->GetPawn() : NULL, DamageCauser);
	return true;
}

void AFCCharacter::OnDeath(float KillingDamage, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser)
{
	OnDying();

	if (bIsDying)
		return;

	bReplicateMovement = false;
	bTearOff = true;
	bIsDying = true;

	ReplicateHit(KillingDamage, DamageEvent, PawnInstigator, DamageCauser, true);

	APlayerController* PC = Cast<APlayerController>(Controller);
	if (PC && DamageEvent.DamageTypeClass)
	{
		UDamageType *DamageType = Cast<UDamageType>(DamageEvent.DamageTypeClass->GetDefaultObject());
		GetMesh()->bHiddenInGame = true;
	}

	if (DeathSound && Mesh1P && Mesh1P->IsVisible())
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());

	// remove all weapons and items
	DestroyInventory();

	DetachFromControllerPendingDestroy();

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);

	if (GetMesh())
	{
		static FName CollisionProfileName(TEXT("Ragdoll"));
		GetMesh()->SetCollisionProfileName(CollisionProfileName);
		GetMesh()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	}

	SetActorEnableCollision(true);
	SetRagdollPhysics();
}

void AFCCharacter::PlayHit(float DamageTaken, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser)
{
	ReplicateHit(DamageTaken, DamageEvent, PawnInstigator, DamageCauser, false);

	APlayerController* PC = Cast<APlayerController>(Controller);
	if (PC && DamageEvent.DamageTypeClass)
		UDamageType *DamageType = Cast<UDamageType>(DamageEvent.DamageTypeClass->GetDefaultObject());

	if (DamageTaken > 0.f) 
	{
		ApplyDamageMomentum(DamageTaken, DamageEvent, PawnInstigator, DamageCauser);

		if (PawnInstigator && PawnInstigator != this && PawnInstigator->IsLocallyControlled())
			APlayerController* InstigatorPC = Cast<APlayerController>(PawnInstigator->Controller);
	}
}

void AFCCharacter::SetRagdollPhysics() 
{
	bool bInRagdoll = false;

	if (IsPendingKill())
		bInRagdoll = false;

	else if (!GetMesh() || !GetMesh()->GetPhysicsAsset())
		bInRagdoll = false;
	else 
	{
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->WakeAllRigidBodies();
		GetMesh()->bBlendPhysics = true;
		bInRagdoll = true;
	}

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->SetComponentTickEnabled(true);

	if (!bInRagdoll && !IsAlive()) 
	{
		TurnOff();
		SetLifeSpan(0.0f);
	}
}

void AFCCharacter::ReplicateHit(float Damage, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser, bool bKilled) 
{
	const float TimeoutTime = GetWorld()->GetTimeSeconds() + 0.5f;

	FDamageEvent const& LastDamageEvent = LastTakeHitInfo.GetDamageEvent();
	if ((PawnInstigator == LastTakeHitInfo.PawnInstigator.Get()) && (LastDamageEvent.DamageTypeClass == LastTakeHitInfo.DamageTypeClass)) 
	{
		if (bKilled && LastTakeHitInfo.bKilled)
			return;

		Damage += LastTakeHitInfo.ActualDamage;

		if (Cast<AFCAIController>(GetController()))
			Cast<AFCAIController>(GetController())->MarkInterest(PawnInstigator);
	}

	LastTakeHitInfo.ActualDamage = Damage;
	LastTakeHitInfo.PawnInstigator = Cast<AFCCharacter>(PawnInstigator);
	LastTakeHitInfo.DamageCauser = DamageCauser;
	LastTakeHitInfo.SetDamageEvent(DamageEvent);
	LastTakeHitInfo.bKilled = bKilled;
	LastTakeHitInfo.EnsureReplication();
	LastTakeHitTimeTimeout = TimeoutTime;
}

FText AFCCharacter::GetCharacterName() const 
{
	return CharacterName;
}

FText AFCCharacter::GetHeadsupName() const
{
	return HeadsupName;
}

void AFCCharacter::SetHeadsupName(FText Input) 
{
	HeadsupName = Input;
}

bool AFCCharacter::GetShowWeaponHUD() 
{
	if (HudRunoffTime > 0)
		return true;
	else
		return false;
}

bool AFCCharacter::GetHasWeaponsHolstered() const
{
	return bHasWeaponsHolstered;
}

void AFCCharacter::SpawnDefaultInventory() 
{
	AddWeapon(GetWorld()->SpawnActor<AFCWeapon>(DefaultGunSlot));
	AddWeapon(GetWorld()->SpawnActor<AFCWeapon>(DefaultPistolSlot));

	for (int32 i = 0; i < DefaultInventoryObjects.Num(); i++)
		AddWeapon(GetWorld()->SpawnActor<AInventoryItem>(DefaultInventoryObjects[i]));

	// equip first weapon in inventory
	EquipWeapon(Inventory[0]);
}

void AFCCharacter::DestroyInventory()
{
	RemoveWeapon(Inventory[0]);
	RemoveWeapon(Inventory[1]);
}

bool AFCCharacter::AddWeapon(AFCWeapon* Weapon)
{
	if (Weapon) 
	{
		AFCWeapon* TargetSlot = NULL;
		bool bReplaceWeapon = false;
		int32 WeaponType = Weapon->GetWeaponType();

		Weapon->OnEnterInventory(this);

		//Set Target Slot
		if (Inventory[0] && WeaponType == 1 && PistolSkill >= 3 || WeaponType == 0)
			TargetSlot = Inventory[0];
		else if (Inventory[1] && WeaponType != 4)
			TargetSlot = Inventory[1];

		if (TargetSlot) 
		{
			if (TargetSlot->GetAbstractName() == Weapon->GetAbstractName()) 
			{
				bReplaceWeapon = false;

				if (TargetSlot->GetCurrentAmmo() < TargetSlot->GetMaxAmmo()) 
				{
					TargetSlot->GiveAmmo(Weapon->GetCurrentAmmo());
					Weapon->Destroy();
				}
				else
					return false;
			}
			else 
			{
				bReplaceWeapon = true;
				RemoveWeapon(TargetSlot);
				TargetSlot = NULL;
			}
		}

		if (!TargetSlot)
			TargetSlot = Weapon;

		if (WeaponType == 1 && PistolSkill >= 3 || Weapon->GetWeaponType() == 0) 
		{
			Inventory[0] = TargetSlot;

			if (bReplaceWeapon)
				SetCurrentWeapon(TargetSlot);
		}
		else if (WeaponType != 4) 
		{
			ToggleHolster();

			Inventory[1] = TargetSlot;

			if (bReplaceWeapon)
				SetCurrentWeapon(TargetSlot);
		}
		else
			Inventory.Add(Weapon);
		
		return true;
	}
	return false;
}

void AFCCharacter::RemoveWeapon(AFCWeapon* Weapon) 
{
	if (Weapon) 
	{
		const FVector RandomLocation = FVector((rand() % 20), (rand() % 20), 30.0f);
		const FVector Location = GetActorLocation() +RandomLocation;

		Weapon->OnLeaveInventory();
		Weapon->SetActorLocation(Location);
		Weapon->Mesh3P->SetWorldLocation(Location);

		if (Inventory[1] == Weapon)
			Inventory[1] = NULL;
		else
			Inventory[0] = NULL;

		GetWorldTimerManager().SetTimer(TimerHandle_LifeSpanExpired, Weapon, &AFCWeapon::RagdollPhysics, 0.05f, false);
	}
}

void AFCCharacter::EquipWeapon(AFCWeapon* Weapon)
{
	HudRunoffTime = 6.0f;

	if (Weapon)
	{
		CurrentWeaponName = Weapon->GetName();
		CurrentWeaponIcon = Weapon->GetIcon();
		SetCurrentWeapon(Weapon);
	}
}

int32 AFCCharacter::GetCurrentAmmo() const
{
	if (CurrentWeapon)
		return (CurrentWeapon->GetCurrentAmmo()) - (CurrentWeapon->GetCurrentAmmoInClip());
	else
		return 0;
}

int32 AFCCharacter::GetCurrentAmmoInClip() const
{
	if (CurrentWeapon)
		return CurrentWeapon->GetCurrentAmmoInClip();
	else
		return 0;
}

bool AFCCharacter::CheckForCurrentWeapon()
{
	if (!bHasWeaponsHolstered && !CurrentWeapon || CurrentWeapon != Inventory[1] && CurrentWeapon != Inventory[0])
	{
		ChangeWeapon();
		return false;
	}
	else
		return true;
}

FText AFCCharacter::GetCurrentWeaponName() const
{
	return CurrentWeaponName;
}

UMaterialInstance* AFCCharacter::GetIcon() const
{
	return CurrentWeaponIcon;
}

void AFCCharacter::OnRep_CurrentWeapon(AFCWeapon* LastWeapon)
{
	SetCurrentWeapon(CurrentWeapon, LastWeapon);
}

void AFCCharacter::SetCurrentWeapon(class AFCWeapon* NewWeapon, class AFCWeapon* LastWeapon) 
{
	GlobalLastWeapon = LastWeapon;
	GlobalNewWeapon = NewWeapon;

	AFCCharacter::UnEquipWeapon();
	GetWorldTimerManager().SetTimer(WeaponEquip, this, &AFCCharacter::EquipWeapon, 0.1f, false);
}

void AFCCharacter::UnEquipWeapon()
{
	AFCWeapon* LocalLastWeapon = NULL;

	if (GlobalLastWeapon != NULL)
		LocalLastWeapon = GlobalLastWeapon;
	else if (GlobalNewWeapon != CurrentWeapon)
		LocalLastWeapon = CurrentWeapon;

	//unequip previous
	if (LocalLastWeapon)
		LocalLastWeapon->OnUnEquip();
}

//Holstering
void AFCCharacter::ToggleHolster()
{
	if (!bHasWeaponsHolstered && CurrentWeapon) //holster weapons
	{
		CurrentWeapon->OnUnEquip();
		RemoveWeapon(Inventory[1]);
		bHasWeaponsHolstered = true; //set Player status
		GetWorldTimerManager().SetTimer(HolsterHandle, this, &AFCPlayerCharacter::Holster, 0.5f, false);
	}
	else if (!bIsInDialogue) //unholster weapons
	{
		bHasWeaponsHolstered = false;
		CurrentWeapon == Inventory[0] || CurrentWeapon == Inventory[1] ? EquipWeapon(CurrentWeapon) : EquipWeapon(Inventory[0]);
	}
}

void AFCCharacter::Holster()
{
	Mesh1P->SetHiddenInGame(true);
}

void AFCCharacter::EquipWeapon()
{
	CurrentWeapon = GlobalNewWeapon;

	//equip new one
	if (GlobalNewWeapon)
	{
		GlobalNewWeapon->SetOwningPawn(this);
		GlobalNewWeapon->OnEquip();
	}
}

//WEAPONS
void AFCCharacter::StartWeaponFire() 
{
	if (bStopFiring) 
		bStopFiring = false;
	else if (CurrentWeapon)
	{
		bWantsToFire = true;
		CurrentWeapon->StartFire();
		!CurrentWeapon->bIsAutomatic ? bStopFiring = true : NULL;
	}
}

void AFCCharacter::StopWeaponFire() 
{
	if (bWantsToFire) 
	{
		bWantsToFire = false;
		CurrentWeapon ? CurrentWeapon->StopFire() : NULL;
	}
}

void AFCCharacter::Melee() 
{
	if (!bIsInDialogue) 
	{
		if (bHasWeaponsHolstered)
			ToggleHolster();

		KnifeAnimationDuration = KnifeAnimation->SequenceLength;

		TArray<AActor*> ActorsTouched;

		if (IsAlive()) 
		{
			GetOverlappingActors(ActorsTouched);

			for (int32 i = 0; i < ActorsTouched.Num(); i++) 
			{
				if (Cast<AFCCharacter>(ActorsTouched[i]) && Cast<AFCCharacter>(ActorsTouched[i])->IsAlive())
				{
					bIsPerformingMelee = true;
					MeleeTarget = Cast<AFCCharacter>(ActorsTouched[i]);
				}
			}

			if (MeleeTarget) 
			{
				GetWorldTimerManager().SetTimer(StabHandle, this, &AFCCharacter::Stab, KnifeAnimationDuration / 2, false);
				GetWorldTimerManager().SetTimer(EndStabHandle, this, &AFCCharacter::EndStab, KnifeAnimationDuration, false);
			}
		}
	}
}

void AFCCharacter::Stab() 
{
	GetWorldTimerManager().ClearTimer(StabHandle);
	FPointDamageEvent MeleeDamage;
	MeleeDamage.DamageTypeClass = MeleeDamageType;
	if (Cast<AFCCharacter>(MeleeTarget))
	{
		MeleeTarget->SetInstantKill(true);
		MeleeTarget->TakeDamage(1, MeleeDamage, GetController(), this);
	}
}

//unhide firearms when stabbing motion ends
void AFCCharacter::EndStab()
{
	GetWorldTimerManager().ClearTimer(EndStabHandle);
	bIsPerformingMelee = false;
}

AFCWeapon* AFCCharacter::GetCurrentWeapon() const
{
	return CurrentWeapon;
}

bool AFCCharacter::GetUnlimitedClips() const
{
	return bUnlimitedClips;
}

bool AFCCharacter::CanFire() const 
{
	if (!bIsInDialogue && !bIsPerformingMelee)
		return IsAlive();
	else
		return false;
}

bool AFCCharacter::CanReload() const 
{
	if (!bIsInDialogue)
		return IsAlive();
	else
		return false;
}

void AFCCharacter::SetTargeting(bool bNewTargeting) 
{
	bIsTargeting = bNewTargeting;

	if (CanFire() && TargetingSound)
			UGameplayStatics::PlaySoundAttached(TargetingSound, GetRootComponent());
	else if (CanFire() && Role < ROLE_Authority)
			SetTargeting(bNewTargeting);
}

//MOVEMENT
void AFCCharacter::SetRunning(bool bNewRunning, bool bToggle) 
{
	bWantsToRun = bNewRunning;
	bWantsToRunToggled = bNewRunning && bToggle;
}

//ANIMATIONS
bool AFCCharacter::GetIsReloading() const
{
	if (CurrentWeapon) return CurrentWeapon->GetIsReloading();
	else return false;
}

void AFCCharacter::ToggleCrouch()
{
	if (bIsCrouched == 0 && !bIsInDialogue)
	{
		if (IsRunning())
			SetRunning(false, false);

		Crouch();
	}
	else
		UnCrouch();
}

void AFCCharacter::OnRep_IsCrouched()
{
	if (GetCharacterMovement() && bIsCrouched)
		GetCharacterMovement()->Crouch(true);
	else if (GetCharacterMovement())
		GetCharacterMovement()->UnCrouch(true);
}

void AFCCharacter::Crouch(bool bClientSimulation)
{
	if (GetCharacterMovement() && CanCrouch())
		Cast<UFCCharacterMovement>(GetCharacterMovement())->bWantsToCrouch = true;
}

void AFCCharacter::UnCrouch(bool bClientSimulation)
{
	if (GetCharacterMovement())
		Cast<UFCCharacterMovement>(GetCharacterMovement())->bWantsToCrouch = false;
}

//Pervitin hotkey
void AFCCharacter::UsePervitin()
{
	for (int32 i = 0; i < GetInventoryCount(); i++)
		if (Inventory[i] && Inventory[i]->GetAbstractName() == "Pervitin")
			Inventory[i]->StartFire();
}

//Medkit hotkey
void AFCCharacter::UseMedkit()
{
	for (int32 i = 0; i < GetInventoryCount(); i++)
		if (Inventory[i] && Inventory[i]->GetAbstractName() == "Medkit")
			Inventory[i]->StartFire();
}

void AFCCharacter::MoveForward(float Val)
{
	if (Controller && Val != 0.f && !bIsInDialogue && !bIsPerformingMelee)
	{
		const bool bLimitRotation = (GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling());
		const FRotator Rotation = bLimitRotation ? GetActorRotation() : Controller->GetControlRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);
		AddMovementInput(Direction, Val);
	}
}

void AFCCharacter::MoveRight(float Val)
{
	if (Val != 0.f && !bIsInDialogue && !bIsPerformingMelee)
	{
		const FRotator Rotation = GetActorRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
		AddMovementInput(Direction, Val);
	}
}

void AFCCharacter::MoveUp(float Val)
{
	if (Val != 0.f && !bIsInDialogue && !bIsPerformingMelee && !(GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling()))
		AddMovementInput(FVector::UpVector, Val);
}

void AFCCharacter::TurnAtRate(float Val) 
{
	AddControllerYawInput(Val * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

//Note - include some Invert X and Y Axis stuff
void AFCCharacter::LookUpAtRate(float Val) 
{
	AddControllerPitchInput(Val * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

//Also used to activate inventory items
void AFCCharacter::OnStartFire() 
{
	if (!bHasWeaponsHolstered) 
	{
		if (IsRunning())
			SetRunning(false, false);
		StartWeaponFire();
	}
	else if (CurrentWeapon->GetWeaponType() == 4)
	{
		if (IsRunning())
			SetRunning(false, false);
		StartWeaponFire();
	}
	else
		ToggleHolster();
}

void AFCCharacter::OnStopFire() 
{
	StopWeaponFire();
}

void AFCCharacter::OnStartTargeting() 
{
	APlayerController* MyPC = Cast<APlayerController>(Controller);
	if (MyPC && Stamina > 0)
	{
		if (IsRunning())
			SetRunning(false, false);
		SetTargeting(true);
	}
}

void AFCCharacter::OnStopTargeting()
{
	SetTargeting(false);
}

void AFCCharacter::ChangeWeapon() 
{
	APlayerController* MyPC = Cast<APlayerController>(Controller);
	AFCWeapon* NextWeapon;

	if (CurrentWeapon == Inventory[1])
		NextWeapon = Inventory[0];
	else
		NextWeapon = Inventory[1];

	if (NextWeapon && NextWeapon->GetCurrentAmmo() > 0)
		EquipWeapon(NextWeapon);
}

void AFCCharacter::NextItem() 
{
	if (bHasWeaponsHolstered) 
	{
		HudRunoffTime = 6.0f;
		int32 ItemCount = Inventory.IndexOfByKey(CurrentWeapon) + 1;

		if (CurrentWeapon && ItemCount < Inventory.Num())
		{
			if (Inventory[ItemCount] == nullptr)
				ItemCount++;

			EquipWeapon(Inventory[ItemCount]);
		}
		else if (CurrentWeapon)
			EquipWeapon(Inventory[0 % Inventory.Num()]);
		else if (Inventory.Num() > 0)
			EquipWeapon(Inventory[Inventory.Num() - 1]);
	}
	else
		ChangeWeapon();
}

void AFCCharacter::PreviousItem() 
{
	if (bHasWeaponsHolstered) 
	{
		HudRunoffTime = 6.0f;
		int32 ItemCount = Inventory.IndexOfByKey(CurrentWeapon) - 1;

		if (CurrentWeapon && ItemCount > 0)
		{
			if (Inventory[ItemCount] == nullptr)
				ItemCount--;

			EquipWeapon(Inventory[ItemCount]);
		}
		else if (CurrentWeapon)
			EquipWeapon(Inventory[Inventory.Num() - 1]);
		else if (Inventory.Num() > 0)
			EquipWeapon(Inventory[0 % Inventory.Num()]);
	}
	else
		ChangeWeapon(); 
}

void AFCCharacter::OnReload() 
{
	APlayerController* MyPC = Cast<APlayerController>(Controller);
	HudRunoffTime = 6.0f;

	if (CurrentWeapon)
		CurrentWeapon->StartReload();
}

void AFCCharacter::OnStartRunning()
{
	APlayerController* MyPC = Cast<APlayerController>(Controller);
	
	if (MyPC && Stamina > 0)
	{
		if (IsTargeting()) SetTargeting(false);

		if (IsCrouching()) ToggleCrouch();

		StopWeaponFire();
		SetRunning(true, false);
	}
}

void AFCCharacter::OnStartRunningToggle() 
{
	APlayerController* MyPC = Cast<APlayerController>(Controller);
	if (MyPC && !bIsCrouched)
	{
		if (IsTargeting()) SetTargeting(false);

		StopWeaponFire();
		SetRunning(true, true);
	}
}

void AFCCharacter::OnStopRunning() 
{
	SetRunning(false, false);
}

bool AFCCharacter::IsRunning() const 
{
	if (!GetCharacterMovement())
		return false;

	return (bWantsToRun || bWantsToRunToggled) && !GetVelocity().IsZero() && (GetVelocity().GetSafeNormal2D() | GetActorRotation().Vector()) > -0.1;
}

void AFCCharacter::Tick(float DeltaSeconds) 
{
	Super::Tick(DeltaSeconds);

	Cast<UFCCharacterMovement>(GetCharacterMovement())->HandleCrouching(DeltaSeconds);

	if (bWantsToRunToggled && !IsRunning()) SetRunning(false, false);
	if (HudRunoffTime > 0) HudRunoffTime -= 0.05f;

	if (bHasWeaponsHolstered) Mesh1P->SetHiddenInGame(true);
	else Mesh1P->SetHiddenInGame(false);
}

AFCWeapon* AFCCharacter::GetWeapon() const 
{
	return CurrentWeapon;
}

int32 AFCCharacter::GetInventoryCount() const
{
	return Inventory.Num();
}

//Check whether player has any item with im
bool AFCCharacter::HasAnyItem()
{
	if (!CurrentWeapon)
	{
		NextItem();
		if (!CurrentWeapon)
			return false;
		else
			return true;
	}
	else
		return true; 
}

bool AFCCharacter::HasItem(AInventoryItem* Item) 
{
	if (Item && Inventory.Contains(Item)) return true;

	return false; 
}

//Check holstering status
bool AFCCharacter::CheckHolstering() 
{
	return bHasWeaponsHolstered;
}

AFCWeapon* AFCCharacter::GetInventoryWeapon(int32 index) const 
{
	if (index != 2) return Inventory[1];
	else return Inventory[0];
}

USkeletalMeshComponent* AFCCharacter::GetPawnMesh() const 
{
	return IsFirstPerson() ? Mesh1P : GetMesh();
}

bool AFCCharacter::AddItem(AInventoryItem* Item) 
{
	for (int32 i = 0; i < Inventory.Num(); i++) 
		if (Inventory[i]->GetAbstractName() == Item->GetAbstractName())
			return false;

	Inventory.Add(Item);
	return true;
}

void AFCCharacter::SetGrenadesCarried(int32 iDesiredAmount) 
{
	if (iDesiredAmount <= MaxAmountGrenades)
		GrenadesCarried = iDesiredAmount;
}

int32 AFCCharacter::GetGrenadesCarried() const 
{
	return GrenadesCarried;
}

FName AFCCharacter::GetWeaponAttachPoint() const
{
	return WeaponAttachPoint;
}

float AFCCharacter::GetTargetingSpeedModifier() const
{
	return TargetingSpeedModifier;
}

bool AFCCharacter::IsCrouching() const 
{
	return bIsCrouched;
}

bool AFCCharacter::IsTargeting() const 
{
	return bIsTargeting;
}

bool AFCCharacter::IsThrowingGrenade() const 
{
	return GrenadeAnimationDuration > 0.0f;
}

bool AFCCharacter::IsStabbing() const 
{
	return bIsPerformingMelee;
}

float AFCCharacter::GetRunningSpeedModifier() const 
{
	return RunningSpeedModifier * IncreasedRunningPace;
}

float AFCCharacter::GetCrouchingSpeedModifier() const
{
	return CrouchingSpeedModifier;
}

bool AFCCharacter::IsFiring() const 
{
	return bWantsToFire;
}

bool AFCCharacter::IsFirstPerson() const 
{
	return IsAlive() && Controller && Controller->IsLocalPlayerController();
}

int32 AFCCharacter::GetMaxHealth() const 
{
	return MaxHealth;
}

float AFCCharacter::GetHealth() const 
{
	return Health;
}

bool AFCCharacter::IsAlive() const 
{
	return Health > 0;
}

//Replication
void AFCCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}