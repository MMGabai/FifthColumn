//Copyright (c) 2016, Mordechai M. Gabai

#pragma once

#include "FifthColumn.h"

AFCWeapon_Instant::AFCWeapon_Instant(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) 
{
	CurrentFiringSpread = 0.0f;
}

//WEAPONS
void AFCWeapon_Instant::FireWeapon() 
{
	const int32 RandomSeed = FMath::Rand();
	FRandomStream WeaponRandomStream(RandomSeed);
	const float CurrentSpread = GetCurrentSpread();
	const float ConeHalfAngle = FMath::DegreesToRadians(CurrentSpread * 0.5f);

	const FVector AimDir = GetAdjustedAim();
	const FVector StartTrace = GetCameraDamageStartLocation(AimDir);
	const FVector ShootDir = WeaponRandomStream.VRandCone(AimDir, ConeHalfAngle, ConeHalfAngle);
	const FVector EndTrace = StartTrace + ShootDir * InstantConfig.WeaponRange;

	const FHitResult Impact = WeaponTrace(StartTrace, EndTrace);
	ProcessInstantHit(Impact, StartTrace, ShootDir, RandomSeed, CurrentSpread);

	CurrentFiringSpread = FMath::Min(InstantConfig.FiringSpreadMax, CurrentFiringSpread + InstantConfig.FiringSpreadIncrement);
}

void AFCWeapon_Instant::NotifyHit(const FHitResult Impact, FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float ReticleSpread) 
{
	const float WeaponAngleDot = FMath::Abs(FMath::Sin(ReticleSpread * PI / 180.f));

	if (Instigator && (Impact.GetActor() || Impact.bBlockingHit)) 
	{
		const FVector Origin = GetMuzzleLocation();
		const FVector ViewDir = (Impact.Location - Origin).GetSafeNormal();

		const float ViewDotHitDir = FVector::DotProduct(Instigator->GetViewRotation().Vector(), ViewDir);
		if (ViewDotHitDir > InstantConfig.AllowedViewDotHitDir - WeaponAngleDot && CurrentState != EWeaponState::Idle)
			Impact.GetActor() == NULL && Impact.bBlockingHit ? ProcessInstantHit_Confirmed(Impact, Origin, ShootDir, RandomSeed, ReticleSpread) :
				ProcessInstantHit_Confirmed(Impact, Origin, ShootDir, RandomSeed, ReticleSpread);
	}
}

void AFCWeapon_Instant::NotifyMiss(FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float ReticleSpread) 
{
	const FVector Origin = GetMuzzleLocation();

	HitNotify.Origin = Origin;
	HitNotify.RandomSeed = RandomSeed;
	HitNotify.ReticleSpread = ReticleSpread;
	const FVector EndTrace = Origin + ShootDir * InstantConfig.WeaponRange;
}

void AFCWeapon_Instant::ProcessInstantHit(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandomSeed, float ReticleSpread)
{
	if (MyPawn && MyPawn->IsLocallyControlled()) 
	{
		if (Impact.GetActor())
			NotifyHit(Impact, ShootDir, RandomSeed, ReticleSpread);
		else if (Impact.GetActor() == NULL) 
		{
			if (Impact.bBlockingHit)
				NotifyHit(Impact, ShootDir, RandomSeed, ReticleSpread);
			else
				NotifyMiss(ShootDir, RandomSeed, ReticleSpread);
		}
	}

	// process a confirmed hit
	ProcessInstantHit_Confirmed(Impact, Origin, ShootDir, RandomSeed, ReticleSpread);
}

void AFCWeapon_Instant::ProcessInstantHit_Confirmed(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandomSeed, float ReticleSpread) 
{
	ShouldDealDamage(Impact.GetActor()) ? DealDamage(Impact, ShootDir) : NULL;

	HitNotify.Origin = Origin;
	HitNotify.RandomSeed = RandomSeed;
	HitNotify.ReticleSpread = ReticleSpread;

	const FVector EndTrace = Origin + ShootDir * InstantConfig.WeaponRange;
	const FVector EndPoint = Impact.GetActor() ? Impact.ImpactPoint : EndTrace;
	SpawnImpactEffects(Impact);
}

bool AFCWeapon_Instant::ShouldDealDamage(AActor* TestActor) const
{
	if (TestActor) return true;
	return false;
}

void AFCWeapon_Instant::DealDamage(const FHitResult& Impact, const FVector& ShootDir) 
{
	FPointDamageEvent PointDmg;
	PointDmg.DamageTypeClass = InstantConfig.DamageType;
	PointDmg.HitInfo = Impact;
	PointDmg.ShotDirection = ShootDir;
	PointDmg.Damage = InstantConfig.HitDamage;

	Impact.GetActor()->TakeDamage(PointDmg.Damage, PointDmg, MyPawn->Controller, this);
}

void AFCWeapon_Instant::OnFireFinished() 
{
	Super::OnFireFinished();
	CurrentFiringSpread = 0.0f;
}

float AFCWeapon_Instant::GetCurrentSpread() const
{
	float FinalSpread = InstantConfig.WeaponSpread + CurrentFiringSpread;
	if (MyPawn)
	{
		Cast<AFCPlayerCharacter>(MyPawn) ? FinalSpread -= (4 - Cast<AFCPlayerCharacter>(MyPawn)->GetSkill(iWeaponType)) / 10 : NULL;

		MyPawn->IsTargeting() ? FinalSpread *= InstantConfig.TargetingSpreadMod : NULL;
	}

	return FinalSpread;
}

//REPLICAION & EFFECTS
void AFCWeapon_Instant::OnRep_HitNotify()
{
	SimulateInstantHit(HitNotify.Origin, HitNotify.RandomSeed, HitNotify.ReticleSpread);
}

FInstantWeaponData AFCWeapon_Instant::GetInstantConfig() const
{
	return InstantConfig;
}

void AFCWeapon_Instant::SimulateInstantHit(const FVector& ShotOrigin, int32 RandomSeed, float ReticleSpread)
{
	FRandomStream WeaponRandomStream(RandomSeed);
	const float ConeHalfAngle = FMath::DegreesToRadians(ReticleSpread * 0.5f);

	const FVector StartTrace = ShotOrigin;
	const FVector AimDir = GetAdjustedAim();
	const FVector ShootDir = WeaponRandomStream.VRandCone(AimDir, ConeHalfAngle, ConeHalfAngle);
	const FVector EndTrace = StartTrace + ShootDir * InstantConfig.WeaponRange;

	FHitResult Impact = WeaponTrace(StartTrace, EndTrace);
	Impact.bBlockingHit ? SpawnImpactEffects(Impact) : NULL;
}

void AFCWeapon_Instant::SpawnImpactEffects(const FHitResult& Impact) 
{
	if (ImpactTemplate && Impact.bBlockingHit) 
	{
		FHitResult UseImpact = Impact;

		if (!Impact.Component.IsValid()) 
		{
			const FVector StartTrace = Impact.ImpactPoint + Impact.ImpactNormal * 10.0f;
			const FVector EndTrace = Impact.ImpactPoint - Impact.ImpactNormal * 10.0f;
			FHitResult Hit = WeaponTrace(StartTrace, EndTrace);
			UseImpact = Hit;
		}

		AImpactEffect* EffectActor = GetWorld()->SpawnActorDeferred<AImpactEffect>(ImpactTemplate, Impact.ImpactPoint, Impact.ImpactNormal.Rotation());
		if (EffectActor)
		{
			EffectActor->SurfaceHit = UseImpact;
			UGameplayStatics::FinishSpawningActor(EffectActor, FTransform(Impact.ImpactNormal.Rotation(), Impact.ImpactPoint));
		}
	}
}

void AFCWeapon_Instant::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}