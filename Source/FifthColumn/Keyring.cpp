//Copyright (c) 2016, Mordechai M. Gabai


#include "FifthColumn.h"
#include "Keyring.h"

/// This is the code for the keyring, an inventory item that contains all the keys you collect along your journey.
// Sets default values
AKeyring::AKeyring()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AKeyring::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AKeyring::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

