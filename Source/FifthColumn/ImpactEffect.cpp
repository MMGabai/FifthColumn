//Copyright (c) 2016, Mordechai M. Gabai

#pragma once
#include "FifthColumn.h"
#include "FCtypes.h"

AImpactEffect::AImpactEffect(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) 
{
	bAutoDestroyWhenFinished = true;
}

void AImpactEffect::PostInitializeComponents() 
{
	Super::PostInitializeComponents();

	UPhysicalMaterial* HitPhysMat = SurfaceHit.PhysMaterial.Get();
	EPhysicalSurface HitSurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitPhysMat);

	UParticleSystem* ParticleSystem = GetImpactFX(HitSurfaceType);
	if (ParticleSystem)
		UGameplayStatics::SpawnEmitterAtLocation(this, ParticleSystem, GetActorLocation(), GetActorRotation());

	USoundCue* SoundCue = GetSound(HitSurfaceType);
	if (SoundCue)
		UGameplayStatics::PlaySoundAtLocation(this, SoundCue, GetActorLocation());

	if (DefaultDecal.DecalMaterial)
	{
		FRotator RandomDecalRotation = SurfaceHit.ImpactNormal.Rotation();
		RandomDecalRotation.Roll = FMath::FRandRange(-180.0f, 180.0f);

		UGameplayStatics::SpawnDecalAttached(DefaultDecal.DecalMaterial, FVector(DefaultDecal.DecalSize, DefaultDecal.DecalSize, 1.0f),
			SurfaceHit.Component.Get(), SurfaceHit.BoneName,
			SurfaceHit.ImpactPoint, RandomDecalRotation, EAttachLocation::KeepWorldPosition,
			DefaultDecal.LifeSpan);
	}
}

UParticleSystem* AImpactEffect::GetImpactFX(TEnumAsByte<EPhysicalSurface> SurfaceType) const 
{
	UParticleSystem* ImpactFX = NULL;

	switch (SurfaceType)
	{
	case FC_SURFACE_Concrete: ImpactFX = ConcreteFX; break;
	case FC_SURFACE_Dirt: ImpactFX = DirtFX; break;
	case FC_SURFACE_Water: ImpactFX = WaterFX; break;
	case FC_SURFACE_Metal: ImpactFX = MetalFX; break;
	case FC_SURFACE_Wood: ImpactFX = WoodFX; break;
	case FC_SURFACE_Grass: ImpactFX = GrassFX; break;
	case FC_SURFACE_Glass: ImpactFX = GlassFX; break;
	case FC_SURFACE_Flesh: ImpactFX = FleshFX; break;
	default: ImpactFX = DefaultFX; break;
	}

	return ImpactFX;
}

USoundCue* AImpactEffect::GetSound(TEnumAsByte<EPhysicalSurface> SurfaceType) const
{
	USoundCue* SoundFX = NULL;

	switch (SurfaceType)
	{
	case FC_SURFACE_Concrete: SoundFX = ConcreteSoundFX; break;
	case FC_SURFACE_Dirt: SoundFX = DirtSoundFX; break;
	case FC_SURFACE_Water: SoundFX = WaterSoundFX; break;
	case FC_SURFACE_Metal: SoundFX = MetalSoundFX; break;
	case FC_SURFACE_Wood: SoundFX = WoodSoundFX; break;
	case FC_SURFACE_Grass: SoundFX = GrassSoundFX; break;
	case FC_SURFACE_Glass: SoundFX = GlassSoundFX; break;
	case FC_SURFACE_Flesh: SoundFX = FleshSoundFX; break;
	default: SoundFX = DefaultSoundFX; break;
	}

	return SoundFX;
}