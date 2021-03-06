//Copyright (c) 2016, Mordechai M. Gabai

#include "FifthColumn.h"
#include "FCPersistentWorld.h"

UFCPersistentWorld::UFCPersistentWorld(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UFCPersistentWorld::SaveGame() 
{
	UGameplayStatics::SaveGameToSlot(this, SaveGameName, UserIndex);
}

void UFCPersistentWorld::LoadGame(FString GameNameInput, int32 UserIndexInput) 
{
	UGameplayStatics::LoadGameFromSlot(GameNameInput, UserIndexInput);
}