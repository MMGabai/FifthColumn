//Copyright (c) 2016, Mordechai M. Gabai

#pragma once

#include "InventoryItem.h"
#include "Keyring.generated.h"

UCLASS()
class FIFTHCOLUMN_API AKeyring : public AInventoryItem
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKeyring();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	
	
};
