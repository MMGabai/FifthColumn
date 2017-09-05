//Copyright (c) 2017, Mordechai M. Gabai
#pragma once

#include "FifthColumn.h"
#include "FCWeapon_Instant.generated.h"

USTRUCT()
struct FInstantHitInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FVector Origin;

	UPROPERTY()
	float ReticleSpread;

	UPROPERTY()
	int32 RandomSeed;
};

USTRUCT()
struct FInstantWeaponData
{
	GENERATED_USTRUCT_BODY()

	//base weapon spread (degrees)
	UPROPERTY(EditDefaultsOnly, Category=Accuracy)
	float WeaponSpread;

	//targeting spread modifier
	UPROPERTY(EditDefaultsOnly, Category=Accuracy)
	float TargetingSpreadMod;

	//continuous firing: spread increment
	UPROPERTY(EditDefaultsOnly, Category=Accuracy)
	float FiringSpreadIncrement;

	//continuous firing: max increment
	UPROPERTY(EditDefaultsOnly, Category=Accuracy)
	float FiringSpreadMax;

	//weapon range
	UPROPERTY(EditDefaultsOnly, Category=WeaponStat)
	float WeaponRange;

	//damage amount
	UPROPERTY(EditDefaultsOnly, Category=WeaponStat)
	int32 HitDamage;

	//type of damage
	UPROPERTY(EditDefaultsOnly, Category=WeaponStat)
	TSubclassOf<UDamageType> DamageType;

	//hit verification: scale for bounding box of hit actor
	UPROPERTY(EditDefaultsOnly, Category=HitVerification)
	float ClientSideHitLeeway;

	//hit verification: threshold for dot product between view direction and hit direction
	UPROPERTY(EditDefaultsOnly, Category=HitVerification)
	float AllowedViewDotHitDir;

	//defaults
	FInstantWeaponData()
	{
		WeaponSpread = 5.0f;
		TargetingSpreadMod = 0.25f;
		FiringSpreadIncrement = 1.0f;
		FiringSpreadMax = 10.0f;
		WeaponRange = 10000.0f;
		HitDamage = 10;
		DamageType = UDamageType::StaticClass();
		ClientSideHitLeeway = 200.0f;
		AllowedViewDotHitDir = 0.8f;
	}
};

// A weapon where the damage impact occurs instantly upon firing
UCLASS(abstract)
class FIFTHCOLUMN_API AFCWeapon_Instant : public AFCWeapon
{
	GENERATED_BODY()
public:
	AFCWeapon_Instant();

	//get current spread
	float GetCurrentSpread() const;

	//weapon config
	UFUNCTION(BlueprintCallable, Category = Config)
		FInstantWeaponData GetInstantConfig() const;

protected:

	//weapon config
	UPROPERTY(EditDefaultsOnly, Category=Config)
	FInstantWeaponData InstantConfig;

	//impact effects
	//UPROPERTY(EditDefaultsOnly, Category=Effects)
	//TSubclassOf<class AFCImpactEffect> ImpactTemplate;

	//smoke trail
	UPROPERTY(EditDefaultsOnly, Category=Effects)
	UParticleSystem* TrailFX;

	//param name for beam target in smoke trail
	UPROPERTY(EditDefaultsOnly, Category=Effects)
	FName TrailTargetParam;

	//instant hit notify for replication
	UPROPERTY(Transient, ReplicatedUsing=OnRep_HitNotify)
	FInstantHitInfo HitNotify;

	//current spread from continuous firing
	float CurrentFiringSpread;
	
	//WEAPONS
	void NotifyHit(const FHitResult Impact, FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float ReticleSpread);

	void NotifyMiss(FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float ReticleSpread);

	void ProcessInstantHit(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandomSeed, float ReticleSpread);

	void ProcessInstantHit_Confirmed(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandomSeed, float ReticleSpread);

	//check if weapon should deal damage to actor
	bool ShouldDealDamage(AActor* TestActor) const;

	//handle damage
	void DealDamage(const FHitResult& Impact, const FVector& ShootDir);

	//weapon specific fire implementation
	virtual void FireWeapon() override;

	//update spread on firing
	virtual void OnFireFinished() override;

	//EFFECTS REPLICATION
	UFUNCTION()
	void OnRep_HitNotify();

	//called in network play to do the cosmetic fx 
	void SimulateInstantHit(const FVector& Origin, int32 RandomSeed, float ReticleSpread);

	//spawn effects for impact
	void SpawnImpactEffects(const FHitResult& Impact);
};
