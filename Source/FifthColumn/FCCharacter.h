//Copyright (c) 2016, Mordechai M. Gabai

#pragma once

#include "FCTypes.h"
#include "InventoryItem.h"
#include "FifthColumn.h"
#include "FCCharacter.generated.h"

UCLASS(abstract)
class FIFTHCOLUMN_API AFCCharacter : public ACharacter
{
	GENERATED_UCLASS_BODY()

public:
	//everything through the headshot box will become instant lethal upon hitting the character
	UPROPERTY(VisibleDefaultsOnly, Category = Gameplay)
		UBoxComponent* HeadshotBox;

	UFUNCTION(BlueprintCallable, Category = Gameplay)
		void SetInstantKill(bool Set);

	//will handle falling damage
	virtual void Landed(const FHitResult& Hit) override;

	//spawn inventory, setup initial variables
	virtual void PostInitializeComponents() override;

	//Update the character. (Running, health etc).
	virtual void Tick(float DeltaSeconds) override;

	//cleanup inventory
	virtual void Destroyed() override;

	//update mesh for first person view
	virtual void PawnClientRestart() override;

	//perform PlayerState related setup 
	virtual void PossessedBy(class AController* InController) override;

	//perform PlayerState related setup
	virtual void OnRep_PlayerState() override;

	//Add camera pitch to first person mesh
	void OnCameraUpdate(const FVector& CameraLocation, const FRotator& CameraRotation, int32& fWideScreenValue);

	//get aim offsets
	UFUNCTION(BlueprintCallable, Category="Game|Weapon")
		FRotator GetAimOffsets() const;

	//get whether character has unlimited clips
	bool GetUnlimitedClips() const;

	//INVENTORY
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		bool HasAnyItem();

	//check whether player owns the item of the parameter
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		bool HasItem(AInventoryItem* Item);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		bool CheckHolstering();

	//UFUNCTION(BlueprintCallable, Category = "Inventory")
	//	bool GetShowWeaponHUD();

	//Add Weapon into inventory
	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
		virtual bool AddWeapon(class AFCWeapon* Weapon);

	//equips weapon from inventory
	virtual void EquipWeapon(class AFCWeapon* Weapon);

	//remove weapon from inventory 
	void RemoveWeapon(class AFCWeapon* Weapon);

	//Find in inventory
	class AFCWeapon* FindWeapon(TSubclassOf<class AFCWeapon> WeaponClass);

	//WEAPONS
	//starts weapon fire
	void StartWeaponFire();

	//[local] stops weapon fire
	void StopWeaponFire();

	//All the melee stuff
	void Stab();
	void EndStab();

	UFUNCTION(BlueprintCallable, Category = Weapon)
		AFCWeapon* GetCurrentWeapon() const;

	//check if pawn can fire weapon
	bool CanFire() const;

	//check if pawn can reload weapon
	bool CanReload() const;

	//change hardaim state
	void SetTargeting(bool bNewTargeting);

	//MOVEMENT
	virtual void OnRep_IsCrouched() override;

	//Make the character start crouching
	virtual void Crouch(bool bClientSimulation = false) override;

	//Make the character stop crouching
	virtual void UnCrouch(bool bClientSimulation = false) override;

	virtual void UsePervitin();

	virtual void UseMedkit();

	//change running state
	void SetRunning(bool bNewRunning, bool bToggle);

	UFUNCTION(BlueprintCallable, Category = Animation)
		bool GetIsReloading() const;

	//Move forward
	void MoveForward(float Val);

	//strafe in both directions (negative parameter values for left)
	void MoveRight(float Val);

	//move forwards (negative parameter values to go backward)
	void MoveUp(float Val);

	//Frame rate independent turn
	void TurnAtRate(float Val);

	//Frame rate independent lookup
	void LookUpAtRate(float Val);

	//activate/deactivate crouch
	void ToggleCrouch(); 

	//lobs a grenade
	//void ThrowGrenade();

	//determines whether the character is looking for items to pickup or interactable characters
	bool bIsPerformingMelee;

	//player pressed start fire action
	void OnStartFire();

	//player released start fire action
	void OnStopFire();

	//player pressed targeting action
	void OnStartTargeting();

	//player released targeting action 
	void OnStopTargeting();

	//switch between gun and pistol
	void ChangeWeapon();

	//player pressed reload action
	void OnReload();

	//player pressed run action
	void OnStartRunning();

	//player pressed toggled run action
	void OnStartRunningToggle();

	//player released run action
	void OnStopRunning();

	//READING
	UFUNCTION(BlueprintCallable, Category = Config)
		FText GetCharacterName() const;

	UFUNCTION(BlueprintCallable, Category = Config)
		FText GetHeadsupName() const;

	UFUNCTION(BlueprintCallable, Category = Config)
		virtual void SetHeadsupName(FText Input);

	//get mesh
	//USkeletalMeshComponent* GetPawnMesh() const;

	//get currently equipped weapon
	UFUNCTION(BlueprintCallable, Category="Game|Weapon")
		class AFCWeapon* GetWeapon() const;

	//get weapon attach point
	FName GetWeaponAttachPoint() const;

	//check total number of inventory items
	int32 GetInventoryCount() const;

	//check ammo
	UFUNCTION(BlueprintCallable, Category = "Game|Player")
		int32 GetCurrentAmmo() const;

	//Check clips
	UFUNCTION(BlueprintCallable, Category = "Game|Player")
		int32 GetCurrentAmmoInClip() const;

	//inventory items
	bool AddItem(AInventoryItem* Item);

	UPROPERTY(Transient, Replicated)
		TArray<class AFCWeapon*> Inventory;

	//get weapon from inventory at index
	class AFCWeapon* GetInventoryWeapon(int32 index) const;

	//get weapon taget modifier speed
	UFUNCTION(BlueprintCallable, Category="Game|Weapon")
		float GetTargetingSpeedModifier() const;

	//get targeting state
	UFUNCTION(BlueprintCallable, Category="Game|Weapon")
		bool IsTargeting() const;

	//get firing state
	UFUNCTION(BlueprintCallable, Category="Game|Weapon")
		bool IsFiring() const;

	//get the modifier value for running speed
	UFUNCTION(BlueprintCallable, Category=Pawn)
		float GetRunningSpeedModifier() const;

	//Get Crouching Speed
	UFUNCTION(BlueprintCallable, Category = Pawn)
		float GetCrouchingSpeedModifier() const;

	//get running state
	UFUNCTION(BlueprintCallable, Category=Pawn)
		bool IsRunning() const;

	//get crouching state
	UFUNCTION(BlueprintCallable, Category = Pawn)
		bool IsCrouching() const;

	//get throwing grenade
	UFUNCTION(BlueprintCallable, Category = Pawn)
		bool IsThrowingGrenade() const;

	UFUNCTION(BlueprintCallable, Category = Pawn)
		bool IsStabbing() const;

	//get camera view type
	UFUNCTION(BlueprintCallable, Category=Mesh)
		virtual bool IsFirstPerson() const;

	//get max health
	UFUNCTION(BlueprintCallable, Category = Health)
		int32 GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = Health)
		float GetHealth() const;

	//check if pawn is still alive
	UFUNCTION(BlueprintCallable, Category = Inventory)
		bool IsAlive() const;

	UPROPERTY(EditDefaultsOnly, Category = Inventory)
		int32 iMaxEncumbranceLimit;

	/// WEAPONS
	UFUNCTION(BlueprintCallable, Category = Inventory)
		bool GetHasWeaponsHolstered() const;

	//currently equipped weapon
	UFUNCTION(BlueprintCallable, Category = Inventory)
		bool CheckForCurrentWeapon();

	UFUNCTION(BlueprintCallable, Category = Inventory)
		FText GetCurrentWeaponName() const;

	//icon displayed on the HUD when weapon is equipped as primary
	UFUNCTION(BlueprintCallable, Category = HUD)
		UMaterialInstance* GetIcon() const;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_CurrentWeapon)
		 class AFCWeapon* CurrentWeapon;

	//INPUT
	FTimerHandle HolsterHandle;

protected:

	//skill related stuff
	UPROPERTY(EditAnywhere, Category = Skill)
		int32 Level;

	UPROPERTY(EditAnywhere, Category = Skill)
		int32 SkillPointsToGive;

	//experience
	int32 XP;

	//the amount of money a character carries
	int32 Money;

	//determines the amount of XP needed for LEVEL 2, gets multiplied by two for each additional level
	UPROPERTY(EditDefaultsOnly, Category = Skill)
		int32 XPRequiredForLevelling; 

	//the amount of skill points you get for bringing death to this foe
	UPROPERTY(EditDefaultsOnly, Category = Skill)
		int32 XPOnKill;

	//DIALOGUE
	UPROPERTY(EditDefaultsOnly, Category = Dialogue)
		bool bIsInDialogue;

	//ANIMATIONS
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Grenade)
		UAnimMontage* ThrowAnimation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Knife)
		UAnimSequence* KnifeAnimation;

	//character to die by your knife attack
	AFCCharacter* MeleeTarget;

	float KnifeAnimationDuration;

	float GrenadeAnimationDuration;

	//socket or bone name for attaching weapon mesh
	UPROPERTY(EditDefaultsOnly, Category=Inventory)
		FName WeaponAttachPoint;

	//default inventory objects
	UPROPERTY(EditAnywhere, Category = Inventory)
		TArray<TSubclassOf<class AFCWeapon> > DefaultInventoryObjects;

	//speech skills
	UPROPERTY(EditDefaultsOnly, Category = Skill)
		int32 ArmenianSkill;

	UPROPERTY(EditDefaultsOnly, Category = Skill)
		int32 AzerbaijaniSkill;

	UPROPERTY(EditDefaultsOnly, Category = Skill)
		int32 SovietSkill;

	//lockpick
	UPROPERTY(EditDefaultsOnly, Category = Skill)
		int32 LockpickSkill;

	//combat skills
	UPROPERTY(EditDefaultsOnly, Category = Skill)
		int32 CombatSkill;

	//sabotage
	UPROPERTY(EditDefaultsOnly, Category = Skill)
		int32 SabotageSkill;

	UPROPERTY(Transient, ReplicatedUsing=OnRep_LastTakeHitInfo)
	struct FTakeHitInfo LastTakeHitInfo;

	float LastTakeHitTimeTimeout;

	UPROPERTY(EditDefaultsOnly, Category=Inventory)
		float TargetingSpeedModifier;

	UPROPERTY(Transient, Replicated)
		uint8 bIsTargeting : 1;

	UPROPERTY(EditDefaultsOnly, Category=Pawn)
		float RunningSpeedModifier;

	UPROPERTY(EditDefaultsOnly, Category = Pawn)
		float CrouchingSpeedModifier;

	//current running state
	UPROPERTY(Transient, Replicated)
		uint8 bWantsToRun : 1;

	//from gamepad running is toggled
	uint8 bWantsToRunToggled : 1;

	uint8 bWantsToFire : 1;

	//Base turn rate, in deg/sec. Other scaling may affect final turn rate.
	float BaseTurnRate;

	//Base lookup rate, in deg/sec. Other scaling may affect final lookup rate.
	float BaseLookUpRate;

	//sound played on death
	UPROPERTY(EditAnywhere, Category=Pawn)
		USoundCue* DeathSound;

	//sound played on XPup
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
		USoundCue* XPSound;

	//sound played on Level up
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
		USoundCue* LevelSound;

	UPROPERTY(EditDefaultsOnly, Category=Pawn)
		USoundCue* TargetingSound;

	FORCEINLINE virtual void TornOff() { SetLifeSpan(0.f); };

public:

	//notification when killed
	UFUNCTION(BlueprintCallable, Category = Health)
		virtual void OnDeath(float KillingDamage, struct FDamageEvent const& DamageEvent, class APawn* InstigatingPawn, class AActor* DamageCauser);

	//check if pawn is in its dying state
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Health)
	uint32 bIsDying:1;

	//Blueprint event for when character dies
	UFUNCTION(BlueprintImplementableEvent)
		void OnDying();

	//Current health of the Pawn
		float Health;

	//max health of a pawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Health)
		float MaxHealth;

	//Take damage, handle death 
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;

	//Add health
	UFUNCTION(BlueprintCallable, Category = Health)
		virtual void AddHealth(float HealthAdd);

	//induce effects of drugs upon the player
	UFUNCTION(BlueprintCallable, Category = Health)
		void ChangeModifiers();

	UFUNCTION(BlueprintCallable, Category = Health)
		float GetBaseTimeUntilWithdrawal() const;

	UFUNCTION(BlueprintCallable, Category = Health)
		float GetBaseStamina() const;

	UFUNCTION(BlueprintCallable, Category = Health)
		float GetTimeUntilWithdrawal() const;

	UFUNCTION(BlueprintCallable, Category = Health)
		float GetStamina() const;

	//fill the ammo of this pawn
	UFUNCTION(BlueprintCallable, Category = Ammo)
		virtual void RestockAllAmmo();

	//how much reichsmark does this player carry
	UFUNCTION(BlueprintCallable, Category = Inventory)
		int32 GetMoney() const;

//FACTIONS
public:

	UFUNCTION(BlueprintCallable, Category = "World State")
		virtual void Redistribute(int32 choice, int32 change);

	UPROPERTY(EditAnywhere, Category = "World State")
		float ArmenianFactionDisposition;

	UPROPERTY(EditAnywhere, Category = "World State")
		float AzerbaijaniFactionDisposition;

	UPROPERTY(EditAnywhere, Category = "World State")
		float SovietFactionDisposition;

	UPROPERTY(EditAnywhere, Category = "World State")
		float MercenaryFactionDisposition;

	UPROPERTY(EditAnywhere, Category = "World State")
		float SSFactionDisposition;

	UFUNCTION(BlueprintCallable, Category = "World State")
		virtual void SetDispositionTowardsCharacter(int32 choice, float change, bool redistribute = false);

	UFUNCTION(BlueprintCallable, Category = "World State")
		float GetDispositionTowardsCharacter(int32 choice) const;

	//Kill this pawn
	virtual void KilledBy(class APawn* EventInstigator);

	//Returns True if the pawn can die in the current state
	virtual bool CanDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser) const;

	//Kills pawn
	UFUNCTION(BlueprintCallable, Category = Health)
		virtual bool Die(float KillingDamage, struct FDamageEvent const& DamageEvent, class AController* Killer, class AActor* DamageCauser);

	UFUNCTION(BlueprintCallable, Category = Health)
		virtual void SimplyDie();

	// Die when we fall out of the world.
	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;

	FTimerHandle StabHandle;
	FTimerHandle EndStabHandle;

protected:

	//unlimited clips, used by AI
	UPROPERTY(EditAnywhere, Category = AI)
		bool bUnlimitedClips;

	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
		TSubclassOf<UDamageType> MeleeDamageType;

	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
		TSubclassOf<UDamageType> FallingDamageType;

	bool bInstantKill;

	//automatic weapon stop firing
	bool bStopFiring;

	//CAMERA
	UPROPERTY(EditDefaultsOnly, Category = Camera)
		FVector CameraSettings;

	UPROPERTY(EditDefaultsOnly, Category = Camera)
		FVector IronSightCameraSettings;

	bool WithdrawalEffect;

	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
		float BaseTimeUntilWithdrawal;

	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
		float BaseStamina;

	float TimeUntilWithdrawal;

	float Stamina;

	//increased running pace
	float IncreasedRunningPace;

	//play effects on hit
	virtual void PlayHit(float DamageTaken, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser);

	//switch to ragdoll
	void SetRagdollPhysics();

	//sets up the replication for taking a hit
	void ReplicateHit(float Damage, struct FDamageEvent const& DamageEvent, class APawn* InstigatingPawn, class AActor* DamageCauser, bool bKilled);

	//play hit or death
	UFUNCTION()
		void OnRep_LastTakeHitInfo();

	//INVENTORY
	//whether weapon is holstered or not
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
		bool bHasWeaponsHolstered;

	//updates current weapon
	void SetCurrentWeapon(class AFCWeapon* NewWeapon, class AFCWeapon* LastWeapon = NULL);
	void UnEquipWeapon();
	void EquipWeapon();

	class AFCWeapon* GlobalNewWeapon;
	AFCWeapon* LocalLastWeapon;
	AFCWeapon* GlobalLastWeapon;

	FTimerHandle WeaponEquip;

	//current weapon rep handler
	UFUNCTION()
	void OnRep_CurrentWeapon(class AFCWeapon* LastWeapon);

	//spawns default inventory
	void SpawnDefaultInventory();

	//remove all weapons from inventory and destroy them
	void DestroyInventory();

	//LOCALISATION
	//the name by which to identify the item for end-users
	UPROPERTY(EditAnywhere, Category = Localisation)
		FText CharacterName;

	UPROPERTY(Transient)
		FText HeadsupName;

	UPROPERTY(Transient)
		FText CurrentWeaponName;

	UPROPERTY(Transient)
		UMaterialInstance* CurrentWeaponIcon;
};
