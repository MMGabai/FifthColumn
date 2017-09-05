//Copyright (c) 2017, Mordechai M. Gabai

#pragma once

#include "FifthColumnProjectile.generated.h"

UCLASS(abstract, Blueprintable)
class FIFTHCOLUMN_API AFifthColumnProjectile : public AActor
{
	GENERATED_BODY()
public:
	AFifthColumnProjectile();

	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
		USphereComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement)
		UProjectileMovementComponent* ProjectileMovement;

	UFUNCTION()
		void OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};

