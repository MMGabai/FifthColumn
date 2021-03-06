//Copyright (c) 2017, Mordechai M. Gabai

#pragma once

#include "FifthColumn.h"
#include "GameFramework/Actor.h"
#include "FCObject.generated.h"

/* FCObject governs all inventory items, non-biological world objects and weapons */
UCLASS()
class FIFTHCOLUMN_API AFCObject : public AActor
{
	GENERATED_UCLASS_BODY()

	//How you will see it in the game world
	UPROPERTY(EditDefaultsOnly, Category = Mesh)
		USkeletalMeshComponent* Mesh3P;
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Weapon|Localisation")
		FString GetAbstractName() const;

	//get Name
	UFUNCTION(BlueprintCallable, Category = "Weapon|Localisation")
		FText GetName() const;

	UFUNCTION(BlueprintCallable, Category = Inventory)
		virtual bool GetIsActive() const;

	//sound played when player picks it up
	UPROPERTY(EditDefaultsOnly, Category = Effects)
		USoundCue* PickupSound;

	//The character who has picked up this pickup
	UPROPERTY(Transient, Replicated)
		AActor* PickedUpBy;

	UFUNCTION(BlueprintCallable, Category = Inventory)
		AActor* GetCharacter() const;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//attaches weapon mesh to pawn's mesh 
	virtual void AttachMeshToPawn();

	//detaches weapon mesh from pawn 
	virtual void DetachMeshFromPawn();
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

protected:

	//the name by which to identify the weapon for comparitive purposes
	UPROPERTY(EditDefaultsOnly, Category = Config)
		FString AbstractName;

	//is it ready for interactions?
	UPROPERTY(Transient, ReplicatedUsing = OnRep_IsActive)
		uint32 bIsActive : 1;

	//REPLICATION
	UFUNCTION()
		void OnRep_IsActive();

	UFUNCTION()
		void OnRep_MyPawn();

	//LOCALISATION
	//the name by which to identify the item for English-speaking users
	UPROPERTY(EditAnywhere, Category = Localisation)
		FText ObjectName;
};
