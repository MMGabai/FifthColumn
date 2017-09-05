//Copyright (c) 2017, Mordechai M. Gabai
#pragma once
#include "FCCharacterMovement.generated.h"

UCLASS()
class FIFTHCOLUMN_API UFCCharacterMovement : public UCharacterMovementComponent
{
	GENERATED_UCLASS_BODY()
public:
	virtual float GetMaxSpeed() const override;

	virtual void Crouch(bool bClientSimulation = false) override;

	virtual void UnCrouch(bool bClientSimulation = false) override;

	virtual bool IsMovingOnGround() const override;

	//Makes sure the crouching looks fluid
	virtual void HandleCrouching(float DeltaTime);

private:
	bool ShouldCrouch;
};

