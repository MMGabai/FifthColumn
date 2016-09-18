//Copyright (c) 2016, Mordechai M. Gabai

#include "FCTypes.generated.h"
#pragma once

UENUM()
namespace EFCPhysMaterialType
{
	enum Type
	{
		Default,
		Concrete,
		Dirt,
		Water,
		Metal,
		Wood,
		Grass,
		Glass,
		Flesh,
		Unknown,
	};
}

#define FC_SURFACE_Default		SurfaceType_Default
#define FC_SURFACE_Concrete     SurfaceType1
#define FC_SURFACE_Dirt	        SurfaceType2
#define FC_SURFACE_Water        SurfaceType3
#define FC_SURFACE_Metal        SurfaceType4
#define FC_SURFACE_Wood         SurfaceType5
#define FC_SURFACE_Grass        SurfaceType6
#define FC_SURFACE_Glass        SurfaceType7
#define FC_SURFACE_Flesh        SurfaceType8

USTRUCT()
struct FDecalData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category=Decal)
	UMaterial* DecalMaterial;

	UPROPERTY(EditDefaultsOnly, Category=Decal)
	float DecalSize;

	UPROPERTY(EditDefaultsOnly, Category=Decal)
	float LifeSpan;

	FDecalData()
		: DecalSize(256.f)
		, LifeSpan(10.f)
	{
	}
};

USTRUCT()
struct FTakeHitInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	float ActualDamage;

	UPROPERTY()
	UClass* DamageTypeClass;

	//Who hit us
	UPROPERTY()
	TWeakObjectPtr<class AFCCharacter> PawnInstigator;

	UPROPERTY()
	TWeakObjectPtr<class AActor> DamageCauser;

	UPROPERTY()
	int32 DamageEventClassID;

	UPROPERTY()
	uint32 bKilled:1;

private:

	//A rolling counter used to ensure the struct is dirty and will replicate.
	UPROPERTY()
	uint8 EnsureReplicationByte;

	//Describes general damage.
	UPROPERTY()
	FDamageEvent GeneralDamageEvent;

	//Describes point damage, if that is what was received.
	UPROPERTY()
	FPointDamageEvent PointDamageEvent;

	//Describes radial damage, if that is what was received.
	UPROPERTY()
	FRadialDamageEvent RadialDamageEvent;

public:
	FTakeHitInfo()
		: ActualDamage(0)
		, DamageTypeClass(NULL)
		, PawnInstigator(NULL)
		, DamageCauser(NULL)
		, DamageEventClassID(0)
		, bKilled(false)
		, EnsureReplicationByte(0)
	{}

	FDamageEvent& GetDamageEvent()
	{
		switch (DamageEventClassID)
		{
		case FPointDamageEvent::ClassID:
			if (PointDamageEvent.DamageTypeClass == NULL)
				PointDamageEvent.DamageTypeClass = DamageTypeClass ? DamageTypeClass : UDamageType::StaticClass();

			return PointDamageEvent;

		case FRadialDamageEvent::ClassID:
			if (RadialDamageEvent.DamageTypeClass == NULL)
				RadialDamageEvent.DamageTypeClass = DamageTypeClass ? DamageTypeClass : UDamageType::StaticClass();

			return RadialDamageEvent;

		default:
			if (GeneralDamageEvent.DamageTypeClass == NULL)
				GeneralDamageEvent.DamageTypeClass = DamageTypeClass ? DamageTypeClass : UDamageType::StaticClass();

			return GeneralDamageEvent;
		}
	}

	void SetDamageEvent(const FDamageEvent& DamageEvent)
	{
		DamageEventClassID = DamageEvent.GetTypeID();
		switch (DamageEventClassID)
		{
		case FPointDamageEvent::ClassID:
			PointDamageEvent = *((FPointDamageEvent const*)(&DamageEvent));
			break;
		case FRadialDamageEvent::ClassID:
			RadialDamageEvent = *((FRadialDamageEvent const*)(&DamageEvent));
			break;
		default:
			GeneralDamageEvent = DamageEvent;
		}

		DamageTypeClass = DamageEvent.DamageTypeClass;
	}
	
	void EnsureReplication() {
		EnsureReplicationByte++; }
};