//Copyright (c) 2017, Mordechai M. Gabai
#include "FifthColumn.h"
#include "NPC.h"

ANPC::ANPC(const FObjectInitializer& ObjectInitializer) :
Super(ObjectInitializer.SetDefaultSubobjectClass<UFCCharacterMovement>(ACharacter::CharacterMovementComponentName))
{
	FactionID = 0; //5 is Pro-German, 1 is Pro-Armenian, 2 is Pro-Azerbaijani, 3 is Pro-Soviet, 4 is mercenary, 0 is generic

	bUnlimitedClips = true;
	bUseControllerRotationYaw = true;

	AIControllerClass = AFCAIController::StaticClass();
}

void ANPC::PostInitializeComponents() 
{
	Super::PostInitializeComponents();
	AFCGameMode* GameMode = Cast<AFCGameMode>(GetWorld()->GetAuthGameMode());
	float EnemyHealthOffset = 0.0f;
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
	//NewStartingDialogue->SubTSubclassOf<ADialogue*>;
}

void ANPC::OnDeath(float KillingDamage, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser)
{
	Super::OnDeath(KillingDamage, DamageEvent, PawnInstigator, DamageCauser);

	if (DamageCauser)
	{
		AFCPlayerController* PlayerController = Cast<AFCPlayerController>(DamageCauser->GetInstigatorController());

		//Give XP to the killer
		if (PlayerController)
		{
			AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(PlayerController->GetCharacter());
			Player->AddXP(XPOnKill);

			if (GetWorld()->GetFirstPlayerController()->GetPawn() == DamageCauser->GetInstigatorController()->GetPawn())
				Player->SetDispositionTowardsCharacter(FactionID, -5);
		}
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
	AFCAIController* AIController = Cast<AFCAIController>(TestPC);
	AFCPlayerCharacter* TestCharacter = Cast<AFCPlayerCharacter>(TestPC->GetPawn());

	float FactionDisposition;

	bool bReturnBool = false;

	//One for the player
	if (TestCharacter)
	{
		FactionDisposition = TestCharacter->GetDispositionTowardsCharacter(FactionID);

		bReturnBool = (FactionDisposition < 40 || TestCharacter && TestCharacter->GetHasWeaponsHolstered() == false &&
			FactionDisposition < 60 || TestCharacter && TestCharacter->GetIsTrespassing() == true && FactionDisposition < 80);
	}
	else if (AIController)
	{
		if (FactionID != Cast<ANPC>(TestPC->GetPawn())->FactionID && FactionID != 0)
			bReturnBool = true;
	}

	return bReturnBool;
}