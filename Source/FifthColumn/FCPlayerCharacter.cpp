//Copyright (c) 2016, Mordechai M. Gabai

#include "FifthColumn.h"
#include "FCPlayerCharacter.h"

AFCPlayerCharacter::AFCPlayerCharacter(const FObjectInitializer& ObjectInitializer) :
Super(ObjectInitializer.SetDefaultSubobjectClass<UFCCharacterMovement>(ACharacter::CharacterMovementComponentName))
{
	bIsTrespassing = false;
}

//DIALOGUE
FText AFCPlayerCharacter::GetDialogueText() const 
{
	return CurrentDialogue->GetDialogueText();
}

ADialogue* AFCPlayerCharacter::GetCurrentDialogue() const 
{
	return CurrentDialogue;
}

ADialogue* AFCPlayerCharacter::GetDialogueToLoad() const 
{
	return DialogueToLoad;
}

void AFCPlayerCharacter::StartDialogue(class AFCCharacter* Other) 
{
	DialoguePartner = Cast<ANPC>(Other);
	DialogueToLoad = GetWorld()->SpawnActor<ADialogue>(DialoguePartner->StartingDialogue);
	SetCurrentDialogue(DialogueToLoad);
	bIsInDialogue = true; 
}

void AFCPlayerCharacter::EndDialogue() 
{
	bIsInDialogue = false;

	//Destroy loaded dialogue data
	CurrentDialogue->UnloadDialogueOptions(true); 
}

void AFCPlayerCharacter::SetCurrentDialogue(class ADialogue* DialogueToLoadInput) 
{
	DialogueToLoad = DialogueToLoadInput;
	bool DialogueTreeOption = DialogueToLoad->GetDialogueTreeOption();
	int32 DialogueLevel = DialoguePartner->GetDialogueLevel();

	//put the speech check here
	bool SpeechCheckSucces = false;

	if (DialogueToLoad->GetSpeechCheck())
		DialogueLevel <= GetSkill(DialoguePartner->GetFaction() + 3) ? SpeechCheckSucces = true : SpeechCheckSucces = false;
	else
		SpeechCheckSucces = true;

	if (SpeechCheckSucces)
		DialogueToLoad->NotifyDialogueSuccess();
	else
		DialogueToLoad->NotifyDialogueFailure();

	//check for dialogue tree
	if (DialogueTreeOption || !CurrentDialogueOptions)
	{
		CurrentDialogueOptions ? CurrentDialogueOptions->UnloadDialogueOptions(true) : NULL;

		CurrentDialogueOptions = DialogueToLoad;
		CurrentDialogueOptions->LoadDialogueOptions();
	}

	DialogueToLoad->ToggleAvailability();
	CurrentDialogue = DialogueToLoad; 
	LoadedDialogueOptions = CurrentDialogueOptions->GetLoadedDialogueOptions();
}

bool AFCPlayerCharacter::GetIsInDialogue() const 
{
	return bIsInDialogue;
}

AFCCharacter* AFCPlayerCharacter::GetDialoguePartner() const 
{
	return DialoguePartner;
}

//get dialogue options
ADialogue* AFCPlayerCharacter::GetDialogueOption(int choice) const
{
	return LoadedDialogueOptions[choice];
}

//checks whether the dialogue options are available
bool AFCPlayerCharacter::DialogueOptionAvailable(int32 choice) const 
{
	if (CurrentDialogue->GetLoadedDialogueOptions()[choice] && CurrentDialogue->GetLoadedDialogueOptions()[choice]->GetAvailable())
		return true;
	else
		return false;
}

//skill related stuff
void AFCPlayerCharacter::AddXP(int32 XPtoAdd) 
{
	UGameplayStatics::PlaySoundAtLocation(this, XPSound, GetActorLocation());
	Cast<AFCPlayerController>(Controller)->SendNotification(FText::FromString("+" + FString::FromInt(XPtoAdd) + "XP"));
	HudRunoffTime = 6.0f;

	if (this)
	{
		XP += XPtoAdd;

		if (XP >= XPRequiredForLevelling)
		{
			XP -= XPRequiredForLevelling;
			LevelUp();
		}
	}
}

void AFCPlayerCharacter::LevelUp() 
{
	UGameplayStatics::PlaySoundAtLocation(this, LevelSound, GetActorLocation());
	Cast<AFCPlayerController>(Controller)->SendNotification(FText::FromString("Level Up!"));
	HudRunoffTime = 6.0f;

	XPRequiredForLevelling *= 2;
	Level++;
	SkillPointsToGive++; 
}

int32 AFCPlayerCharacter::CheckXP() const 
{
	return XP;
}

int32 AFCPlayerCharacter::GetSkillPointsToSpend() const 
{
	return SkillPointsToGive;
}

int32 AFCPlayerCharacter::CheckXPRequiredForLevelling() const 
{
	return XPRequiredForLevelling;
}

int32 AFCPlayerCharacter::CheckLevel() const 
{
	return Level;
}

void AFCPlayerCharacter::IncreaseSkill(int32 SkillCategory) 
{
	SkillPointsToGive--;

	switch (SkillCategory)
	{
	case 1:
		PistolSkill++;
		break;
	case 2:
		AssaultSkill++;
		break;
	case 3:
		MarksmanSkill++;
		break;
	case 4:
		ArmenianSkill++;
		break;
	case 5:
		AzerbaijaniSkill++;
		break;
	case 6:
		SovietSkill++;
		break;
	case 7:
		LockpickSkill++;
		break;
	case 8:
		CombatSkill++;
		break;
	case 9:
		SabotageSkill++;
		break;
	}
}

int32 AFCPlayerCharacter::GetSkill(int32 SkillCategory) 
{
	switch (SkillCategory)
	{
	case 1:
		return PistolSkill;
	case 2:
		return AssaultSkill;
	case 3:
		return MarksmanSkill;
	case 4:
		return ArmenianSkill;
	case 5:
		return AzerbaijaniSkill;
	case 6:
		return SovietSkill;
	case 7:
		return LockpickSkill;
	case 8:
		return CombatSkill;
	case 9:
		return SabotageSkill;
	}

	return 0;
}

bool AFCPlayerCharacter::GetSkillAvailability(int32 SkillCategory) 
{
	if (SkillPointsToGive > 0) 
	{
		switch (SkillCategory)
		{
		case 1:
			return (PistolSkill < 5);
		case 2:
			return (AssaultSkill < 5);
		case 3:
			return (MarksmanSkill < 5);
		case 4:
			return (ArmenianSkill < 5);
		case 5:
			return (AzerbaijaniSkill < 5);
		case 6:
			return (SovietSkill < 5);
		case 7:
			return (LockpickSkill < 5);
		case 8:
			return (CombatSkill < 5);
		case 9:
			return (SabotageSkill < 5);
		} 
	}

	return false; 
}

//INVENTORY
void AFCPlayerCharacter::Use()
{
	if (CurrentActorTouched)
	{
		AActivatableObject* CurrentObjectTouched = Cast<AActivatableObject>(CurrentActorTouched);
		AFCWeapon* CurrentWeaponTouched = Cast<AFCWeapon>(CurrentActorTouched);
		ANPC* CurrentCharacterTouched = Cast<ANPC>(CurrentActorTouched);

		if (CurrentObjectTouched && CurrentObjectTouched->GetIsActive())
			CurrentObjectTouched->Activate(this);
		else if (CurrentWeaponTouched && CurrentWeaponTouched->GetIsActive())
			CurrentWeaponTouched->PickupOnTouch(this);
		else if (CurrentCharacterTouched && CurrentCharacterTouched->IsAlive())
		{
			APlayerController* MyPC = Cast<APlayerController>(CurrentCharacterTouched->GetInstigatorController());

			if (!(CurrentCharacterTouched->IsEnemyFor(MyPC)) && CheckHolstering())
				StartDialogue(CurrentCharacterTouched); //commence dialogue screen for player
		}
	}
}

void AFCPlayerCharacter::ToggleTrespassing(bool ToggleTrue)
{
	bIsTrespassing && ToggleTrue ? bIsTrespassing = false : bIsTrespassing = true;
}

bool AFCPlayerCharacter::GetIsTrespassing() const
{
	return bIsTrespassing;
}

void AFCPlayerCharacter::Tick(float DeltaSeconds) 
{
	APlayerController* MyPC = Cast<APlayerController>(Controller);

	if (MyPC)
	{
		//Get centre point of screen
		FVector CameraLoc;
		FRotator CameraRot;
		FHitResult HitActor;
		FCollisionQueryParams TraceParameters;
		GetActorEyesViewPoint(CameraLoc, CameraRot);

		//Determine Distance
		FVector StartLoc = CameraLoc + (CameraRot.Vector() * 65.0f);
		FVector EndLoc = CameraLoc + (CameraRot.Vector() * 240.0f);

		GetWorld()->LineTraceSingleByChannel(
			HitActor,
			StartLoc,
			EndLoc,
			ECC_WorldStatic,
			TraceParameters
			);

		//Induce interaction
		FHitResult* ActorFound = &HitActor;

		CurrentActorTouched = ActorFound->GetActor();

		AFCObject* CurrentObjectTouched = Cast<AFCObject>(CurrentActorTouched);
		AFCCharacter* CurrentCharacterTouched = Cast<AFCCharacter>(CurrentActorTouched);

		//if (CurrentActorTouched)
		//	 Distance = (CurrentActorTouched->GetActorLocation() - GetActorLocation()).SizeSquared();

		if (bHasWeaponsHolstered)
		{
			GetMesh()->SetHiddenInGame(true);
			//GetWeapon()->GetWeaponMesh()->SetHiddenInGame(true);
		}

		if (CurrentObjectTouched && CurrentObjectTouched->GetIsActive())
			HeadsupName = CurrentObjectTouched->GetName();
		else if (CurrentCharacterTouched && CurrentCharacterTouched->IsAlive())
			HeadsupName = CurrentCharacterTouched->GetCharacterName();
		else
		{
			HeadsupName = FText::FromString("");
			CurrentActorTouched = NULL;
		}

		SetHeadsupName(HeadsupName);

		//auto heal when player is not in withdrawal
		if (Health < GetMaxHealth() && Stamina > 0 && bHasWeaponsHolstered)
		{
			Health += 5 * DeltaSeconds;
			Health > GetMaxHealth() ? Health = GetMaxHealth() : NULL;
		}
	}

	if (!WithdrawalEffect)
	{
		Stamina < BaseStamina ? Stamina += 5 : NULL;

		TimeUntilWithdrawal -= 0.025f;

		if (CombatSkill >= 3)
			TimeUntilWithdrawal += 0.03f;

		if (TimeUntilWithdrawal < 1)
		{
			WithdrawalEffect = true;
			TimeUntilWithdrawal = 0.0f;
		}
	}

	IsRunning() ? Stamina -= 12.0f : NULL;

	if (Stamina <= 0)
	{
		SetRunning(false, false);
		SetTargeting(false);
	}

	Super::Tick(DeltaSeconds); 
}

//INPUT
void AFCPlayerCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent) 
{
	check(InputComponent);

	//Movement
	InputComponent->BindAxis("MoveForward", this, &AFCCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AFCCharacter::MoveRight);
	InputComponent->BindAxis("MoveUp", this, &AFCCharacter::MoveUp);
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &AFCCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &AFCCharacter::LookUpAtRate);

	InputComponent->BindAction("Run", IE_Pressed, this, &AFCCharacter::OnStartRunning);
	InputComponent->BindAction("RunToggle", IE_Pressed, this, &AFCCharacter::OnStartRunningToggle);
	InputComponent->BindAction("Run", IE_Released, this, &AFCCharacter::OnStopRunning);
	InputComponent->BindAction("Use", IE_Pressed, this, &AFCPlayerCharacter::Use);
	InputComponent->BindAction("Crouch", IE_Pressed, this, &AFCCharacter::ToggleCrouch);
	InputComponent->BindAction("Jump", IE_Pressed, this, &AFCCharacter::Jump);

	//Weapons related stuff
	InputComponent->BindAction("Fire", IE_Pressed, this, &AFCCharacter::OnStartFire);
	InputComponent->BindAction("Fire", IE_Released, this, &AFCCharacter::OnStopFire);
	InputComponent->BindAction("IronSights", IE_Pressed, this, &AFCCharacter::OnStartTargeting);
	InputComponent->BindAction("IronSights", IE_Released, this, &AFCCharacter::OnStopTargeting);
	InputComponent->BindAction("ThrowGrenade", IE_Pressed, this, &AFCCharacter::ThrowGrenade);
	InputComponent->BindAction("Melee", IE_Pressed, this, &AFCPlayerCharacter::Melee);

	//Inventory Management stuff
	InputComponent->BindAction("Holster", IE_Pressed, this, &AFCPlayerCharacter::ToggleHolster);
	InputComponent->BindAction("NextWeapon", IE_Pressed, this, &AFCCharacter::NextItem);
	InputComponent->BindAction("PreviousWeapon", IE_Pressed, this, &AFCCharacter::PreviousItem);
	InputComponent->BindAction("Reload", IE_Pressed, this, &AFCCharacter::OnReload);
	//InputComponent->BindAction("Pervitin", IE_Pressed, this, &AFCCharacter::UsePervitin); //TODO: Discard Item
	//InputComponent->BindAction("Medkit", IE_Pressed, this, &AFCCharacter::UseMedkit);
}

void AFCPlayerCharacter::ToggleHolster()
{
	//Check if player has weapons
	if (Inventory[0] == NULL && Inventory[1] == NULL)
	{
		Cast<AFCPlayerController>(Controller)->SendNotification(FText::FromString("You cannot unholster weapons you do not have!"));

		return;
	}

	if (!bHasWeaponsHolstered && CurrentWeapon) //holster weapons
	{
		CurrentWeapon->OnUnEquip();
		RemoveWeapon(Inventory[1]); //Remove SMG's
		bHasWeaponsHolstered = true; //set Player status
		GetWorldTimerManager().SetTimer(HolsterHandle, this, &AFCPlayerCharacter::Holster, 0.5f, false);
	}
	else if (!bIsInDialogue) //unholster weapons
	{
		bHasWeaponsHolstered = false;
		CurrentWeapon == Inventory[0] || CurrentWeapon == Inventory[1] ? EquipWeapon(CurrentWeapon) : EquipWeapon(Inventory[0]);
	}
}



void AFCPlayerCharacter::Melee()
{
	if (!bIsInDialogue)
	{
		if (bHasWeaponsHolstered)
			ToggleHolster();

		KnifeAnimationDuration = KnifeAnimation->SequenceLength;

		TArray<AActor*> ActorsTouched;

		if (IsAlive())
		{
			GetOverlappingActors(ActorsTouched);

			for (int32 i = 0; i < ActorsTouched.Num(); i++)
			{
				if (Cast<AFCCharacter>(ActorsTouched[i]) && Cast<AFCCharacter>(ActorsTouched[i])->IsAlive())
				{
					bIsPerformingMelee = true;
					MeleeTarget = Cast<AFCCharacter>(ActorsTouched[i]);
				}
			}

			if (MeleeTarget)
			{
				GetWorldTimerManager().SetTimer(StabHandle, this, &AFCCharacter::Stab, KnifeAnimationDuration / 2, false);
				GetWorldTimerManager().SetTimer(EndStabHandle, this, &AFCCharacter::EndStab, KnifeAnimationDuration, false);
			}
		}
	}
}

//BOOKS
void AFCPlayerCharacter::SetCurrentBook(AActivatableObject* BookInput) 
{
	CurrentBook = BookInput;
}

AActivatableObject* AFCPlayerCharacter::GetCurrentBook() const
{
	return CurrentBook;
}

bool AFCPlayerCharacter::GetIsReading() const 
{
	return bIsReading;
}

void AFCPlayerCharacter::SetIsReading(bool SetValue)
{
	bIsReading = SetValue;
}

//All your quest needs
AQuest* AFCPlayerCharacter::SelectActiveQuest(TSubclassOf<AQuest> SelectedQuest) 
{
	AQuest* Quest = GetWorld()->SpawnActor<AQuest>(SelectedQuest, FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f));

	for (int32 i = 0; i < ActiveQuests.Num(); i++)
		if (ActiveQuests[i]->GetAbstractName() == Quest->GetAbstractName())
			return ActiveQuests[i];

	return Quest;
}

//check for active quest
bool AFCPlayerCharacter::HasPrimaryActiveQuest()
{
	if (!PrimaryActiveQuest)
	{
		SelectNextQuest();
		if (!PrimaryActiveQuest)
			return false;
		else
			return true;
	}
	else
		return true;
}

void AFCPlayerCharacter::SelectPrevQuest() 
{
	if (PrimaryActiveQuest)
	{
		const int32 ItemCount = ActiveQuests.IndexOfByKey(PrimaryActiveQuest);
		AQuest* PrevQuest = PrimaryActiveQuest;

		if (ActiveQuests.Num() - 1 > 0)
			PrevQuest = ActiveQuests[ItemCount - 1];

		if (ItemCount > 0)
			PrimaryActiveQuest = PrevQuest;
	}
	else if (ActiveQuests.Num() > 0)
		PrimaryActiveQuest = ActiveQuests[0];
}

void AFCPlayerCharacter::SelectNextQuest() 
{
	if (PrimaryActiveQuest)
	{
		const int32 ItemCount = ActiveQuests.IndexOfByKey(PrimaryActiveQuest);
		AQuest* NextQuest = PrimaryActiveQuest;

		if (ActiveQuests.Num() > ItemCount + 1)
			NextQuest = ActiveQuests[ItemCount + 1];

		if ((ItemCount + 1) < ActiveQuests.Num())
			PrimaryActiveQuest = NextQuest;
	}
	else if (ActiveQuests.Num() > 0)
		PrimaryActiveQuest = ActiveQuests[0];
}

AQuest* AFCPlayerCharacter::GetPrimaryActiveQuest() const
{	
	return PrimaryActiveQuest; 
}