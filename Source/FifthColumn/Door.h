//Copyright (c) 2017, Mordechai M. Gabai

#pragma once

#include "GameFramework/Actor.h"
#include "Door.generated.h"

/**
 * 
 */
UCLASS(abstract)
class FIFTHCOLUMN_API ADoor : public AActivatableObject
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Mesh)
		USceneComponent* SceneComponent;

	//Do a Tick, this is necessary for the door opening mechanics to work
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = Door)
		virtual void OpenDoor(float DeltaSeconds);

	UFUNCTION(BlueprintCallable, Category = Door)
		virtual void CloseDoor(float DeltaSeconds);

	int32 GetDoorLevel() const;
	int32 GetXPGained() const;

	bool GetbLocked() const;
	bool GetbCanBeDestroyed() const;

public:
	ADoor();

	virtual void Activate(class AFCCharacter* Pawn, bool bForce = false) override;

protected:

	float Yaw; //the basic yaw from which the door opens

	virtual void PostInitializeComponents() override;

	UPROPERTY(EditDefaultsOnly, Category = Config)
		float DoorOpeningSpeed;

	UPROPERTY(EditAnywhere, Category = Config)
		float OpeningAngle; //The amount of degrees traversed by opening the door

	UPROPERTY(EditAnywhere, Category = Config)
		int32 DoorLevel; //the amount of skill with the lockpick needed to bypass the lock

	UPROPERTY(EditAnywhere, Category = Config)
		int32 XPGained; //XP gained by breaking/bypassing the lock

	//UPROPERTY(EditDefaultsOnly, Category = Config)
		//FString Key; //determines which inventory item can be used as key

	UPROPERTY(EditDefaultsOnly, Category = Config)
		bool bDoorOpened;

	UPROPERTY(EditAnywhere, Category = Config)
		bool bLocked; //if true, a lock or key is needed to break in

	UPROPERTY(EditDefaultsOnly, Category = Config)
		bool bCanBeDestroyed; //can be destroyed using weapons

	//sound played when player opens door
	UPROPERTY(EditDefaultsOnly, Category = Effects)
		USoundCue* OpeningSound;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
		USoundCue* LockedSound;
};
