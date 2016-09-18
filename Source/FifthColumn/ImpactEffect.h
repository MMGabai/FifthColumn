//Copyright (c) 2016, Mordechai M. Gabai

#pragma once

#include "GameFramework/Actor.h"
#include "ImpactEffect.generated.h"

UCLASS(abstract, Blueprintable)
class FIFTHCOLUMN_API AImpactEffect : public AActor
{
	GENERATED_UCLASS_BODY()

	//surface data for spawning
	UPROPERTY(BlueprintReadOnly, Category = Surface)
		FHitResult SurfaceHit;

	//impact FX on concrete
	UPROPERTY(EditDefaultsOnly, Category = Visual)
		UParticleSystem* ConcreteFX;

	//impact FX on metal
	UPROPERTY(EditDefaultsOnly, Category = Visual)
		UParticleSystem* MetalFX;

	//impact FX on metal
	UPROPERTY(EditDefaultsOnly, Category = Visual)
		UParticleSystem* DirtFX;

	//impact FX on water
	UPROPERTY(EditDefaultsOnly, Category = Visual)
		UParticleSystem* WaterFX;

	//impact FX on glass
	UPROPERTY(EditDefaultsOnly, Category = Visual)
		UParticleSystem* GlassFX;

	//impact FX on flesh
	UPROPERTY(EditDefaultsOnly, Category = Visual)
		UParticleSystem* FleshFX;

	//impact FX on grass
	UPROPERTY(EditDefaultsOnly, Category = Visual)
		UParticleSystem* GrassFX;

	//impact FX on wood
	UPROPERTY(EditDefaultsOnly, Category = Visual)
		UParticleSystem* WoodFX;

	//impact FX for most stuff
	UPROPERTY(EditDefaultsOnly, Category = Visual)
		UParticleSystem* DefaultFX;

	//spawn effect
	virtual void PostInitializeComponents() override;
	
	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	struct FDecalData DefaultDecal;

protected:

	UParticleSystem* GetImpactFX(TEnumAsByte<EPhysicalSurface> SurfaceType) const;

	USoundCue* GetSound(TEnumAsByte<EPhysicalSurface> SurfaceType) const;

	UPROPERTY(EditDefaultsOnly, Category = Defaults)
		USoundCue* ConcreteSoundFX;

	UPROPERTY(EditDefaultsOnly, Category = Defaults)
		USoundCue* DirtSoundFX;

	UPROPERTY(EditDefaultsOnly, Category = Defaults)
		USoundCue*  WaterSoundFX;

	UPROPERTY(EditDefaultsOnly, Category = Defaults)
		USoundCue* MetalSoundFX;

	UPROPERTY(EditDefaultsOnly, Category = Defaults)
		USoundCue*  WoodSoundFX;

	UPROPERTY(EditDefaultsOnly, Category = Defaults)
		USoundCue*  GrassSoundFX;

	UPROPERTY(EditDefaultsOnly, Category = Defaults)
		USoundCue* GlassSoundFX;

	UPROPERTY(EditDefaultsOnly, Category = Defaults)
		USoundCue* FleshSoundFX;

	UPROPERTY(EditDefaultsOnly, Category = Defaults)
		USoundCue* DefaultSoundFX;
};
