//Copyright (c) 2016, Mordechai M. Gabai


#pragma once
#include "FifthColumn.h"
#include "DisturbanceSource.h"
#include "FCWeapon.generated.h"

namespace EWeaponState
{
	enum Type
	{
		Idle,
		Firing,
		Reloading,
		Equipping,
		Unequipping,
	};
}

USTRUCT()
struct FWeaponData
{
	GENERATED_USTRUCT_BODY()

	//max ammo
	UPROPERTY(EditDefaultsOnly, Category=Ammo)
	int32 MaxAmmo;

	//clip size  
	UPROPERTY(EditDefaultsOnly, Category=Ammo)
		int32 AmmoPerClip;

	//initial clips  
	UPROPERTY(EditDefaultsOnly, Category=Ammo)
		int32 InitialClips;

	//recoil
	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
		float RecoilAmount;

	//time between two consecutive shots  
	UPROPERTY(EditDefaultsOnly, Category=WeaponStat)
	float TimeBetweenShots;

	//failsafe reload duration if weapon doesn't have any animation for it  
	UPROPERTY(EditDefaultsOnly, Category=WeaponStat)
	float NoAnimReloadDuration;

	//defaults
	FWeaponData()
	{
		MaxAmmo = 100;
		AmmoPerClip = 20;
		InitialClips = 4;
		TimeBetweenShots = 0.2f;
		NoAnimReloadDuration = 1.0f;
		RecoilAmount = 1.0f;
	}
};

UCLASS(abstract, Blueprintable)
class FIFTHCOLUMN_API AFCWeapon : public AFCObject
{
	GENERATED_UCLASS_BODY()

	virtual void BeginPlay() override;

	//perform initial setup  
	virtual void PostInitializeComponents() override;

	virtual void Destroyed() override;

	//AMMO
	virtual void GiveAmmo(int AddAmount);

	//consume a bullet  
	void UseAmmo();

	//weapon type query
	UFUNCTION(BlueprintCallable, Category = Inventory)
		int32 GetWeaponType() const;

	//set weapon type: 0 is pistol, 1 is assault weapons, 2 are long-range rifles and 4 are items
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
		int32 iWeaponType;

	//weapon is being equipped by owner pawn 
	virtual void OnEquip();

	//weapon is now equipped by owner pawn 
	virtual void OnEquipFinished();

	//weapon is holstered by owner pawn 
	virtual void OnUnEquip();

	//add to inventory 
	virtual void OnEnterInventory(AFCCharacter* NewOwner);

	//remove from inventory
	virtual void OnLeaveInventory();

		//attaches weapon mesh to pawn's mesh 
	virtual void AttachMeshToPawn() override;

	//detaches weapon mesh from pawn 
	virtual void DetachMeshFromPawn() override;

	//check if it's currently equipped
	bool IsEquipped() const;

	//check if mesh is already attached
	bool IsAttachedToPawn() const;

	//INPUT
	//start weapon fire
	virtual void StartFire();

	//stop weapon fire
	virtual void StopFire();

	//start weapon reload
	virtual void StartReload(bool bFromReplication = false);
	virtual void StartReloadNoPar(); //alternative way of calling start reload, in case parameters are not allowed

	//interrupt weapon reload
	virtual void StopReload();

	virtual void ReloadWeapon();

	//automatic weapon or not
	UPROPERTY(EditDefaultsOnly, Category = Config)
		bool bIsAutomatic;

	//silenced weapon or not
	UPROPERTY(EditDefaultsOnly, Category = Config)
		bool bIsSilenced;

	//loudness model
	UPROPERTY(EditDefaultsOnly, Category = Config)
		TSubclassOf<ADisturbanceSource> LoudnessModel;

	//CONTROL
	//check if weapon can fire
	bool CanFire() const;

	//check if weapon can be reloaded
	bool CanReload() const;

	//READING
	//get the muzzle location of the weapon
	FVector GetMuzzleLocation() const;

	//get direction of weapon's muzzle
	FVector GetMuzzleDirection() const;

	//get current weapon state
	EWeaponState::Type GetCurrentState() const;

	//get whether the user is reloading the weapon
	UFUNCTION(BlueprintCallable, Category = Animation)
	bool GetIsReloading() const;

	UFUNCTION(BlueprintCallable, Category = Animation)
	bool GetIsFiring() const;

	UFUNCTION(BlueprintCallable, Category = Animation)
	bool GetIsEquipping() const;

	//get current ammo amount (total)
	int32 GetCurrentAmmo() const;

	//get current ammo amount (clip)
	int32 GetCurrentAmmoInClip() const;

	//get clip size
	int32 GetAmmoPerClip() const;

	//get max ammo amount
	int32 GetMaxAmmo() const;

	//get weapon mesh (needs pawn owner to determine variant)
	USkeletalMeshComponent* GetWeaponMesh() const;

	//get pawn owner
	UFUNCTION(BlueprintCallable, Category="Game|Weapon")
	class AFCCharacter* GetPawnOwner() const;

	//icon displayed on the HUD when weapon is equipped as primary
	UFUNCTION(BlueprintCallable, Category = HUD)
		UMaterialInstance* GetIcon() const;

	//icon displayed on the HUD when weapon is equipped as primary
	UPROPERTY(EditDefaultsOnly, Category=HUD)
		UMaterialInstance* Icon;

	//how many icons to draw per clip
	UPROPERTY(EditDefaultsOnly, Category=HUD)
	float AmmoIconsCount;

	//defines spacing between primary ammo icons (left side)
	UPROPERTY(EditDefaultsOnly, Category=HUD)
	int32 PrimaryClipIconOffset;

	//defines spacing between secondary ammo icons (left side)
	UPROPERTY(EditDefaultsOnly, Category=HUD)
	int32 SecondaryClipIconOffset;

	//set the weapon's owning pawn
	void SetOwningPawn(AFCCharacter* AFCCharacter);

	//gets last time when this weapon was switched to
	float GetEquipStartedTime() const;

	//gets the duration of equipping weapon
	float GetEquipDuration() const;

	///Handle Pickup Stuff
	//to fire when picked up
	virtual void PickupOnTouch(class AFCCharacter* Pawn);

	//the weapon as seen in the player hands
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		USkeletalMeshComponent* Mesh1P;

	virtual void SetLocation(const FVector Location);

	FVector GetWeaponCameraSettings() const;
	FVector GetWeaponIronSightCameraSettings() const;

	void RagdollPhysics();

	//TIMER HANDLES
	FTimerHandle EquipFinishedHandle;
	FTimerHandle StopReloadHandle;
	FTimerHandle ReloadWeaponHandle;
	FTimerHandle FiringHandle;

	//ANIMATION
	UFUNCTION(BlueprintCallable, Category = Animation)
		UAnimSequence* GetIdleAnim() const;

	UFUNCTION(BlueprintCallable, Category = Animation)
		UAnimSequence* GetReloadAnim() const;

	UFUNCTION(BlueprintCallable, Category = Animation)
		UAnimSequence* GetEquipAnim() const;

	UFUNCTION(BlueprintCallable, Category = Animation)
		UAnimSequence* GetFireAnim() const;

	UFUNCTION(BlueprintCallable, Category = Animation)
		UAnimSequence* GetRunAnim() const;

	UFUNCTION(BlueprintCallable, Category = Animation)
		UAnimSequence* GetHardAimAnim() const;

	//find public hit
	UFUNCTION(BlueprintCallable, Category = AI)
		FHitResult GetWeaponTrace(const FVector& TraceFrom, const FVector& TraceTo) const;

protected:

	virtual void Tick(float DeltaSeconds) override;

	float RecoilDeltaSeconds;

	//pawn owner
	UPROPERTY(Transient, ReplicatedUsing=OnRep_MyPawn)
	AFCCharacter* MyPawn;

	//blueprint event: pickup disappears
	UFUNCTION(BlueprintImplementableEvent)
		virtual void OnPickedUpEvent();

	UFUNCTION()
		void OnRep_IsActive();

	virtual void OnPickedUp();

	virtual void SpawnPickup();

	//weapon data
	UPROPERTY(EditDefaultsOnly, Category=Config)
	FWeaponData WeaponConfig;

	//impact
	UPROPERTY(EditDefaultsOnly, Category = Effects)
		TSubclassOf<class AActor> ImpactTemplate;

	//name of bone/socket for muzzle in weapon mesh
	UPROPERTY(EditDefaultsOnly, Category=Effects)
		FName MuzzleAttachPoint;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
		FName EjectAttachPoint;

	//FX for muzzle flash
	UPROPERTY(EditDefaultsOnly, Category=Effects)
		UParticleSystem* MuzzleFX;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
		UParticleSystem* EjectionFX;

	//spawned component for muzzle FX
	UPROPERTY(Transient)
		UParticleSystemComponent* MuzzlePSC;

	//spawned component for ejection FX
	UPROPERTY(Transient)
		UParticleSystemComponent* EjectionPSC;

	//spawned component for second muzzle FX (Needed for split screen)
	UPROPERTY(Transient)
		UParticleSystemComponent* MuzzlePSCSecondary;

	//camera shake on firing
	UPROPERTY(EditDefaultsOnly, Category=Effects)
	TSubclassOf<UCameraShake> FireCameraShake;

	//force feedback
	UPROPERTY(EditDefaultsOnly, Category=Effects)
	UForceFeedbackEffect *FireForceFeedback;

	//single fire sound (bLoopedFireSound not set)
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* FireSound;

	//out of ammo sound
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* OutOfAmmoSound;

	//reload sound
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* ReloadSound;

	//reload animations
	UPROPERTY(EditDefaultsOnly, Category = Animation)
		UAnimSequence* IdleAnim;

	//reload animations
	UPROPERTY(EditDefaultsOnly, Category=Animation)
		UAnimSequence* ReloadAnim;

	//equip animations
	UPROPERTY(EditDefaultsOnly, Category=Animation)
		UAnimSequence* EquipAnim;

	//fire animations
	UPROPERTY(EditDefaultsOnly, Category=Animation)
		UAnimSequence* FireAnim;

	//Running
	UPROPERTY(EditDefaultsOnly, Category = Animation)
		UAnimSequence* RunAnim;

	//Hard Aim animations
	UPROPERTY(EditDefaultsOnly, Category = Animation)
		UAnimSequence* HardAimAnim;

	//equip sound
	UPROPERTY(EditDefaultsOnly, Category = Sound)
		USoundCue* EquipSound;

	//is muzzle FX looped?
	UPROPERTY(EditDefaultsOnly, Category=Effects)
	uint32 bLoopedMuzzleFX : 1;

	//is fire sound looped?
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	uint32 bLoopedFireSound : 1;

	//is fire animation playing?
	uint32 bPlayingFireAnim : 1;

	//is weapon currently equipped?
	uint32 bIsEquipped : 1;

	//is weapon fire active?
	uint32 bWantsToFire : 1;

	//is reload animation playing?
	UPROPERTY(Transient, ReplicatedUsing=OnRep_Reload)
	uint32 bPendingReload : 1;

	//is equip animation playing?
	uint32 bPendingEquip : 1;

	//weapon is refiring
	uint32 bRefiring;

	//current weapon state
	EWeaponState::Type CurrentState;

	//time of last successful weapon fire
	float LastFireTime;

	//last time when this weapon was switched to
	float EquipStartedTime;

	//how much time weapon needs to be equipped
	float EquipDuration;

	//current total ammo
	UPROPERTY(Transient, Replicated)
	int32 CurrentAmmo;

	//current ammo - inside clip
	UPROPERTY(Transient, Replicated)
	int32 CurrentAmmoInClip;

	UPROPERTY(EditDefaultsOnly, Category = Camera)
		FVector WeaponCameraSettings;

	UPROPERTY(EditDefaultsOnly, Category = Camera)
		FVector IronSightWeaponCameraSettings;

	//REPLICATION
	UFUNCTION()
	void OnRep_MyPawn();

	UFUNCTION()
	void OnRep_Reload();

	//Called in network play to stop cosmetic fx (e.g. for a looping shot).
	virtual void StopSimulatingWeaponFire();

	//WEAPONS
	//weapon specific fire implementation
	virtual void FireWeapon() PURE_VIRTUAL(AFCWeapon::FireWeapon,);

	//handle weapon fire
	void HandleFiring();

	//firing started
	virtual void OnFireStarted();

	//firing finished
	virtual void OnFireFinished();

	//update weapon state
	void SetWeaponState(EWeaponState::Type NewState);

	//determine current weapon state
	void DetermineWeaponState();

	//play weapon sounds
	UAudioComponent* PlayWeaponSound(USoundCue* Sound);

	//Get the aim of the weapon, allowing for adjustments to be made by the weapon
	virtual FVector GetAdjustedAim() const;

	//Get the aim of the camera
	FVector GetCameraAim() const;

	//get the originating location for camera damage
	FVector GetCameraDamageStartLocation(const FVector& AimDir) const;

	//find hit
	FHitResult WeaponTrace(const FVector& TraceFrom, const FVector& TraceTo) const;
};

