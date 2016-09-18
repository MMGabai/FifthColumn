//Copyright (c) 2016, Mordechai M. Gabai

#pragma once

#include "FifthColumn.h"

UFCCharacterMovement::UFCCharacterMovement(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

float UFCCharacterMovement::GetMaxSpeed() const 
{
	float MaxSpeed = Super::GetMaxSpeed();

	const AFCCharacter* CharacterOwner = Cast<AFCCharacter>(PawnOwner);
	if (CharacterOwner->IsTargeting())
		MaxSpeed *= CharacterOwner->GetTargetingSpeedModifier();

	if (CharacterOwner->IsCrouching())
		MaxSpeed *= CharacterOwner->GetCrouchingSpeedModifier();

	if (CharacterOwner->IsRunning())
		MaxSpeed *= CharacterOwner->GetRunningSpeedModifier();

	return MaxSpeed;
}

void UFCCharacterMovement::Crouch(bool bClientSimulation) 
{
	float DeltaTime = GetWorld()->GetDeltaSeconds();

	if (!HasValidData() || CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() == CrouchedHalfHeight || !CanCrouchInCurrentState())
		return;

	if (bClientSimulation && CharacterOwner->Role == ROLE_SimulatedProxy) 
	{
		ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
		CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight());
		bShrinkProxyCapsule = false;
	}

	const float ComponentScale = CharacterOwner->GetCapsuleComponent()->GetShapeScale();
	const float OldUnscaledHalfHeight = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	ShouldCrouch = true;
	float HalfHeightAdjust = (OldUnscaledHalfHeight - CrouchedHalfHeight);
	float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;

	if (!bClientSimulation)
	{
		if (CrouchedHalfHeight > OldUnscaledHalfHeight)
		{
			static const FName NAME_CrouchTrace = FName(TEXT("CrouchTrace"));
			FCollisionQueryParams CapsuleParams(NAME_CrouchTrace, false, CharacterOwner);
			FCollisionResponseParams ResponseParam;
			InitCollisionParams(CapsuleParams, ResponseParam);
			const bool bEncroached = GetWorld()->OverlapBlockingTestByChannel(CharacterOwner->GetActorLocation() - FVector(ScaledHalfHeightAdjust / 2, 0.f, ScaledHalfHeightAdjust), FQuat::Identity,
				UpdatedComponent->GetCollisionObjectType(), GetPawnCapsuleCollisionShape(SHRINK_None), CapsuleParams, ResponseParam);
			//const bool bEncroached = GetWorld()->OverlapTest(CharacterOwner->GetActorLocation() - FVector(0.f, 0.f, ScaledHalfHeightAdjust), FQuat::Identity,
			//	UpdatedComponent->GetCollisionObjectType(), GetPawnCapsuleCollisionShape(SHRINK_None), CapsuleParams, ResponseParam);

			if (bEncroached)
			{
				CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), OldUnscaledHalfHeight);
				return;
			}
		}

		UpdatedComponent->MoveComponent(FVector(0.f, 0.f, -ScaledHalfHeightAdjust), CharacterOwner->GetActorRotation(), true);
		CharacterOwner->bIsCrouched = true;
	}

	bForceNextFloorCheck = true;

	AFCCharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<AFCCharacter>();
	HalfHeightAdjust = (DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - CrouchedHalfHeight);
	ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;

	AdjustProxyCapsuleSize();
	CharacterOwner->OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}


void UFCCharacterMovement::UnCrouch(bool bClientSimulation)
{
	if (!HasValidData())
		return;

	ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();

	if (CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() == DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight())
		return;

	const float CurrentCrouchedHalfHeight = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	const float ComponentScale = CharacterOwner->GetCapsuleComponent()->GetShapeScale();
	const float OldUnscaledHalfHeight = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	const float HalfHeightAdjust = DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - OldUnscaledHalfHeight;
	const float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;
	const FVector PawnLocation = CharacterOwner->GetActorLocation();

	check(CharacterOwner->GetCapsuleComponent());
	bool bUpdateOverlaps = false;
	ShouldCrouch = false;

	if (!bClientSimulation)	
	{
		static const FName NAME_CrouchTrace = FName(TEXT("CrouchTrace"));
		const float SweepInflation = KINDA_SMALL_NUMBER * 10.f;
		FCollisionQueryParams CapsuleParams(NAME_CrouchTrace, false, CharacterOwner);
		FCollisionResponseParams ResponseParam;
		InitCollisionParams(CapsuleParams, ResponseParam);
		const FCollisionShape StandingCapsuleShape = GetPawnCapsuleCollisionShape(SHRINK_HeightCustom, -SweepInflation); // Shrink by negative amount, so actually grow it.
		const ECollisionChannel CollisionChannel = UpdatedComponent->GetCollisionObjectType();
		bool bEncroached = true;

		if (!IsMovingOnGround()) 
		{
			bEncroached = GetWorld()->OverlapBlockingTestByChannel(PawnLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);
			//bEncroached = GetWorld()->OverlapTest(PawnLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);

			if (bEncroached) 
			{
				if (ScaledHalfHeightAdjust > 0.f) 
				{
					float PawnRadius, PawnHalfHeight;
					CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleSize(PawnRadius, PawnHalfHeight);
					const float ShrinkHalfHeight = PawnHalfHeight - PawnRadius;
					const float TraceDist = PawnHalfHeight - ShrinkHalfHeight;
					const FVector Down = FVector(0.f, 0.f, -TraceDist);

					FHitResult Hit(1.f);
					const FCollisionShape ShortCapsuleShape = GetPawnCapsuleCollisionShape(SHRINK_HeightCustom, ShrinkHalfHeight);
					const bool bBlockingHit = GetWorld()->SweepSingleByChannel(Hit, PawnLocation, PawnLocation + Down, FQuat::Identity, CollisionChannel, ShortCapsuleShape, CapsuleParams);
					if (Hit.bStartPenetrating)
						bEncroached = true;
					else 
					{
						const float DistanceToBase = (Hit.Time * TraceDist) + ShortCapsuleShape.Capsule.HalfHeight;
						const FVector NewLoc = FVector(PawnLocation.X, PawnLocation.Y, PawnLocation.Z - DistanceToBase + PawnHalfHeight + SweepInflation + MIN_FLOOR_DIST / 2.f);
						//bEncroached = GetWorld()->OverlapTest(NewLoc, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams);
						bEncroached = GetWorld()->OverlapBlockingTestByChannel(NewLoc, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams);

						if (!bEncroached)
							UpdatedComponent->MoveComponent(NewLoc - PawnLocation, CharacterOwner->GetActorRotation(), false);
					}
				}
			}
		}
		else
		{
			FVector StandingLocation = PawnLocation + FVector(0.f, 0.f, StandingCapsuleShape.GetCapsuleHalfHeight() - CurrentCrouchedHalfHeight);
			//bEncroached = GetWorld()->OverlapTest(StandingLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);
			bEncroached = GetWorld()->OverlapBlockingTestByChannel(StandingLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);

			if (bEncroached)
			{
				const float MinFloorDist = KINDA_SMALL_NUMBER * 10.f;
				if (CurrentFloor.bBlockingHit && CurrentFloor.FloorDist > MinFloorDist)
				{
					StandingLocation.Z -= CurrentFloor.FloorDist - MinFloorDist;
					bEncroached = GetWorld()->OverlapBlockingTestByChannel(StandingLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);
					//bEncroached = GetWorld()->OverlapTest(StandingLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);
				}
			}

			if (!bEncroached)
			{
				UpdatedComponent->MoveComponent(StandingLocation - PawnLocation, CharacterOwner->GetActorRotation(), false);
				bForceNextFloorCheck = true;
			}
		}

		if (bEncroached)
		{
			CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), OldUnscaledHalfHeight, false);
			CharacterOwner->GetCapsuleComponent()->UpdateBounds();
			return;
		}

		CharacterOwner->bIsCrouched = false;
	}
	else
		bShrinkProxyCapsule = true;

	bUpdateOverlaps = true;
	CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(), bUpdateOverlaps);

	AdjustProxyCapsuleSize();
	CharacterOwner->OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

bool UFCCharacterMovement::IsMovingOnGround() const 
{
	return Super::IsMovingOnGround();
}

void UFCCharacterMovement::HandleCrouching(float DeltaTime) 
{
	ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();

	if (ShouldCrouch)
		CharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(FMath::FInterpTo(CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(),
			CrouchedHalfHeight, DeltaTime, 5.0f));
	else
		CharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(FMath::FInterpTo(CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(),
		DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(), DeltaTime, 5.0f), false);

}