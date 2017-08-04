//Copyright (c) 2016, Mordechai M. Gabai

#pragma once
#include "FifthColumn.h"

ANPC::ANPC(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) 
{
	FactionID = 0; //1 is Pro-Armenian, 2 is Pro-Azerbaijani, 3 is Pro-Soviet, 0 is generic

	bUnlimitedClips = true;
	bUseControllerRotationYaw = true;

	AIControllerClass = AFCAIController::StaticClass();
}

void ANPC::PostInitializeComponents() 
{
	Super::PostInitializeComponents();
	AFCGameMode* GameMode = Cast<AFCGameMode>(GetWorld()->GetAuthGameMode());
	float EnemyHealthOffset;
	GameMode ? EnemyHealthOffset = GameMode->GetEnemyHealthOffset() : 0.0f;
	
	Role == ROLE_Authority && EnemyHealthOffset != NULL ? MaxHealth *= EnemyHealthOffset : NULL;
}

void ANPC::SetHostileToPlayer(bool hostile) 
{
	bIsHostileToPlayer = hostile;
}

bool ANPC::GetHostileToPlayer() const 
{
	return bIsHostileToPlayer;
}

void ANPC::SetStartingDialogue(ADialogue* NewStartingDialogue)
{

}

void ANPC::OnDeath(float KillingDamage, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser)
{
	Super::OnDeath(KillingDamage, DamageEvent, PawnInstigator, DamageCauser);
	AFCPlayerController* PlayerController = Cast<AFCPlayerController>(DamageCauser->GetInstigatorController());

	//Give XP to the killer
	if (PlayerController)
	{
		Cast<AFCPlayerCharacter>(PlayerController->GetCharacter())->AddXP(XPOnKill);

		if (GetWorld()->GetFirstPlayerController()->GetPawn() == DamageCauser->GetInstigatorController()->GetPawn())
			PlayerController->SetDispositionTowardsPlayer(FactionID, -5, true);
	}
}

int32 ANPC::GetFaction() const
{
	return FactionID;
}

int32 ANPC::GetDialogueLevel() const 
{
	return DialogueLevel;
}

bool ANPC::IsEnemyFor(AController* TestPC) const
{
	if (TestPC == Controller || TestPC == NULL)
		return false;

	APlayerState* TestPlayerState = Cast<APlayerState>(TestPC->PlayerState);
	APlayerState* MyPlayerState = Cast<APlayerState>(PlayerState);
	AFCPlayerCharacter* TestCharacter = Cast<AFCPlayerCharacter>(TestPC->GetPawn());
	AFCPlayerController* TestController = Cast<AFCPlayerController>(TestPC);

	bool bReturnBool = false;

	if (TestController)
	{
		float FactionDisposition = TestController->GetDispositionTowardsPlayer(FactionID);

		bReturnBool = (FactionDisposition < 40 || TestCharacter && TestCharacter->GetHasWeaponsHolstered() == false &&
			FactionDisposition < 60 || TestCharacter && TestCharacter->GetIsTrespassing() == true && FactionDisposition < 80);
	}

	if (GetWorld()->GetFirstPlayerController() != TestPC && FactionID != NULL && TestPC != NULL)
		bReturnBool = (FactionID != Cast<ANPC>(TestPC->GetPawn())->FactionID && FactionID != 0);

	return bReturnBool;
}