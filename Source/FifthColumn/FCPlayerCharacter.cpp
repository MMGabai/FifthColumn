//Copyright (c) 2017, Mordechai M. Gabai

#include "FifthColumn.h"
#include "Blueprint/UserWidget.h"
#include "FCPlayerCameraManager.h"
#include "FCPlayerCharacter.h"

AFCPlayerCharacter::AFCPlayerCharacter(const FObjectInitializer& ObjectInitializer) :
Super(ObjectInitializer.SetDefaultSubobjectClass<UFCCharacterMovement>(ACharacter::CharacterMovementComponentName))
{
	bIsTrespassing = false;
	bHasWeaponsHolstered = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->AttachParent = GetCapsuleComponent();
	SpringArm->TargetOffset = FVector::FVector(0.0f, 0.0f, 55.0f);
	SpringArm->bUsePawnControlRotation = true;

	GetCapsuleComponent()->SetCollisionObjectType(ECC_Pawn);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Block);

	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PlayerArms"));
	Mesh1P->AttachParent = SpringArm;
	Mesh1P->bOnlyOwnerSee = true;
	Mesh1P->bTreatAsBackgroundForOcclusion = true;
	Mesh1P->bOwnerNoSee = false;
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->bReceivesDecals = true;
	Mesh1P->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	Mesh1P->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	Mesh1P->bChartDistanceFactor = false;
	Mesh1P->SetCollisionObjectType(ECC_Pawn);
	Mesh1P->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	Mesh1P->SetCollisionResponseToAllChannels(ECR_Block);	

	GetMesh()->AttachParent = Mesh1P;

	ArmenianFactionDisposition = 40.0f;
	AzerbaijaniFactionDisposition = 40.0f;
	SovietFactionDisposition = 40.0f;
	MercenaryFactionDisposition = 40.0f;
	SSFactionDisposition = 40.0f;
}

void AFCPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	HUDRunOffTime = 0.0f;

	if (CameraManager == nullptr)
	{
		CameraManager = AFCPlayerCameraManager::StaticClass();
		//Cast<APlayerController*>(GetController())->PlayerCameraManagerClass = AFCPlayerCameraManager::StaticClass();
	}

	//Create Widgets
	SendNotification(FText::FromString("Press 'J' to open the Journal."));
}

//Add Weapon into inventory
bool AFCPlayerCharacter::AddWeapon(class AFCWeapon* Weapon)
{
	if (Weapon)
	{
		HUDRunOffTime = 6.0f;
		Super::AddWeapon(Weapon); //please do not remove this

		return true;
	}

	return false;
}

//equips weapon from inventory
void AFCPlayerCharacter::EquipWeapon(class AFCWeapon* Weapon)
{
	if (Weapon)
	{
		HUDRunOffTime = 6.0f;
		Super::EquipWeapon(Weapon);
	}
}

void AFCPlayerCharacter::NextItem()
{
	HUDRunOffTime = 6.0f;
	int32 ItemCount = Inventory.IndexOfByKey(CurrentWeapon) + 1;

	if (Inventory.Num() > 1)
	{
		if (ItemCount < Inventory.Num())
		{
			//skip slot if it is empty
			if (Inventory[ItemCount] == nullptr)
				ItemCount++;

			EquipWeapon(Inventory[ItemCount]);
		}
		else
			EquipWeapon(Inventory[0]);
	}
}

void AFCPlayerCharacter::PreviousItem()
{
	HUDRunOffTime = 6.0f;
	int32 ItemCount = Inventory.IndexOfByKey(CurrentWeapon) - 1;

	if (Inventory.Num() > 1)
	{
		if (ItemCount > 0)
		{
			//skip slot if it is empty
			if (Inventory[ItemCount] == nullptr)
				ItemCount--;

			EquipWeapon(Inventory[ItemCount]);
		}
		else
			EquipWeapon(Inventory[Inventory.Num() - 1]);
	}
}

void AFCPlayerCharacter::RemoveItem()
{
	if (CurrentWeapon)
	{
		SendNotification(FText::FromString("You have dropped a " + CurrentWeapon->GetName().ToString()));
		RemoveWeapon(CurrentWeapon);
	}
}

//HUD
float AFCPlayerCharacter::GetHudRunOffTime() const
{
	return HUDRunOffTime;
}

void AFCPlayerCharacter::ExtendHudRunOffTime()
{
	HUDRunOffTime = 6.0f;
}

FText AFCPlayerCharacter::GetAmmoAmount()
{
	AFCWeapon* Weapon;
	Weapon = GetWeapon();

	if (Weapon == NULL)
		return FText::GetEmpty();

	int32 CurrentWeaponType = Weapon->GetWeaponType();
	int32 CurrentAmmoInClip = GetCurrentAmmoInClip();

	if (CurrentWeaponType == 4)
	{
		AInventoryItem* Item = Cast<AInventoryItem>(Weapon);
		if (Item->GetNoQuantity())
			return FText::GetEmpty();
		else
			return FText::FromString(FString::FromInt(CurrentAmmoInClip));
	}

	int32 CurrentAmmo = GetCurrentAmmo();
	FText AmmoAmountReturn = FText::FromString(FString::FromInt(CurrentAmmoInClip) + FString(" - ") + FString::FromInt(CurrentAmmo));

	return AmmoAmountReturn;
}

FText AFCPlayerCharacter::GetWeaponName()
{;
	AFCWeapon* Weapon;

	Weapon = GetWeapon();

	if (Weapon == NULL)
		return FText::GetEmpty();

	FText CurrentWeaponName = Weapon->GetName();

	return CurrentWeaponName;
}

bool AFCPlayerCharacter::GetHandIcon()
{
	if (IsAlive() && IsTargeting() && GetHasTouchedActor())
		return true;

	return false;
}

bool AFCPlayerCharacter::GetWeaponNameEnabled()
{
	if (IsAlive() && GetHudRunOffTime() > 0)
		return true;
	else
		return false;
}


void AFCPlayerCharacter::SendNotification(FText Message)
{
	ExtendHudRunOffTime();

	MessageStack.Push(Message);

	if (MessageStack.Num() > 5)
		MessageStack.RemoveAt(0);
}

FText AFCPlayerCharacter::GetLatestMessage()
{
	FText Message = FText::GetEmpty();
	int32 MessageStackCount = MessageStack.Num();

	if (MessageStackCount > 0)
	{
		FText LastMessage = MessageStack.Last();

		for (int32 i = MessageStackCount - 1; i >= 0; i--)
		{
			FText MessageIteration = MessageStack[i];

			Message = FText::FromString(MessageIteration.ToString() + "\n" + Message.ToString());
		}

		return Message;
	}

	SendNotification(FText::FromString("Notifications will be shown here."));
	return Message;
}

void AFCPlayerCharacter::SetDispositionTowardsCharacter(int32 choice, float change, bool redistribute)
{
	UGameplayStatics::SpawnSoundAttached(FactionsChangedSound, GetRootComponent());
	SendNotification(FText::FromString("Your actions have been noted."));

	Super::SetDispositionTowardsCharacter(choice, change, redistribute);
}

bool AFCPlayerCharacter::GetHasTouchedActor()
{
	AFCPlayerCharacter* Player = Cast<AFCPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if (Player == NULL)
		return false;

	if (Player->HasActorTouched())
		return true;
	else
		return false;
}

bool AFCPlayerCharacter::HasActorTouched()
{
	if (CurrentActorTouched)
	{
		HUDRunOffTime = 6.0f;
		return true;
	}
	else
		return false;
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
	DialogueToLoad->SetCharacter(DialoguePartner);

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

//get current quest text
FText AFCPlayerCharacter::GetCurrentQuestText()
{
	if (HasPrimaryActiveQuest() == true)
		return GetPrimaryActiveQuest()->GetName();

	return FText::GetEmpty();
}

//get current quest description
FText AFCPlayerCharacter::GetCurrentQuestDesc()
{
	if (HasPrimaryActiveQuest() == true)
		return GetPrimaryActiveQuest()->GetQuestDesc();

	return FText::GetEmpty();
}

//skill related stuff
void AFCPlayerCharacter::AddXP(int32 XPtoAdd) 
{
	UGameplayStatics::PlaySoundAtLocation(this, XPSound, GetActorLocation());
	SendNotification(FText::FromString("+" + FString::FromInt(XPtoAdd) + "XP"));

	HUDRunOffTime = 6.0f;

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
	SendNotification(FText::FromString("Level Up!"));
	
	HUDRunOffTime = 6.0f;

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
		//PistolSkill++;
		break;
	case 2:
		//AssaultSkill++;
		break;
	case 3:
		//MarksmanSkill++;
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
		//return PistolSkill;
	case 2:
		//return AssaultSkill;
	case 3:
		//return MarksmanSkill;
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
			return 0;
		case 2:
			return 0;
		case 3:
			return 0;
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

		if (bHasWeaponsHolstered)
			GetMesh()->SetHiddenInGame(true);

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

	//HUD Stuff
	if (HUDRunOffTime > 0.0f)
		HUDRunOffTime -= 0.01f;

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

	if (bHasWeaponsHolstered) Mesh1P->SetHiddenInGame(true);
	else Mesh1P->SetHiddenInGame(false);

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
	InputComponent->BindAxis("MoveForward", this, &AFCPlayerCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AFCPlayerCharacter::MoveRight);
	InputComponent->BindAxis("MoveUp", this, &AFCPlayerCharacter::MoveUp);
	InputComponent->BindAxis("Turn", this, &AFCPlayerCharacter::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &AFCPlayerCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &AFCPlayerCharacter::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &AFCPlayerCharacter::LookUpAtRate);

	InputComponent->BindAction("Run", IE_Pressed, this, &AFCPlayerCharacter::OnStartRunning);
	InputComponent->BindAction("RunToggle", IE_Pressed, this, &AFCPlayerCharacter::OnStartRunningToggle);
	InputComponent->BindAction("Run", IE_Released, this, &AFCPlayerCharacter::OnStopRunning);
	InputComponent->BindAction("Use", IE_Pressed, this, &AFCPlayerCharacter::Use);
	InputComponent->BindAction("Crouch", IE_Pressed, this, &AFCPlayerCharacter::ToggleCrouch);
	InputComponent->BindAction("Jump", IE_Pressed, this, &AFCPlayerCharacter::Jump);

	//Weapons related stuff
	InputComponent->BindAction("Fire", IE_Pressed, this, &AFCPlayerCharacter::OnStartFire);
	InputComponent->BindAction("Fire", IE_Released, this, &AFCPlayerCharacter::OnStopFire);
	InputComponent->BindAction("IronSights", IE_Pressed, this, &AFCPlayerCharacter::OnStartTargeting);
	InputComponent->BindAction("IronSights", IE_Released, this, &AFCPlayerCharacter::OnStopTargeting);
	//InputComponent->BindAction("ThrowGrenade", IE_Pressed, this, &AFCCharacter::ThrowGrenade); //TO BE REPLACED BY INVENTORY SYSTEM
	//InputComponent->BindAction("Melee", IE_Pressed, this, &AFCPlayerCharacter::Melee);

	//Inventory Management stuff
	InputComponent->BindAction("Holster", IE_Pressed, this, &AFCPlayerCharacter::ToggleHolster);
	InputComponent->BindAction("NextWeapon", IE_Pressed, this, &AFCPlayerCharacter::NextItem);
	InputComponent->BindAction("PreviousWeapon", IE_Pressed, this, &AFCPlayerCharacter::PreviousItem);
	InputComponent->BindAction("Reload", IE_Pressed, this, &AFCPlayerCharacter::OnReload);
	InputComponent->BindAction("RemoveItem", IE_Pressed, this, &AFCPlayerCharacter::RemoveItem);
}

void AFCPlayerCharacter::ToggleHolster()
{
	//Check if player has weapons
	if (GetWeapon() == nullptr)
	{
		SendNotification(FText::FromString("You cannot unholster weapons you do not have!"));

		return;
	}
	else if (GetWeapon()->GetWeaponType() == 4)
	{
		SendNotification(FText::FromString("You cannot unholster a non-weapon!"));
		CurrentWeapon->OnUnEquip();

		return;
	}

	if (!bHasWeaponsHolstered && CurrentWeapon) //holster weapons
	{
		CurrentWeapon->OnUnEquip();
		//RemoveWeapon(Inventory[1]); //Remove SMG's - DEPRECATED MECHANIC
		bHasWeaponsHolstered = true; //set Player status
		//GetWorldTimerManager().SetTimer(HolsterHandle, this, &AFCPlayerCharacter::Holster, 0.5f, false);
	}
	else if (!bIsInDialogue) //unholster weapons
	{
		bHasWeaponsHolstered = false;
		EquipWeapon(CurrentWeapon); //Inventory[0] || CurrentWeapon == Inventory[1] ? EquipWeapon(CurrentWeapon) : EquipWeapon(Inventory[0]);
	}
}

void AFCPlayerCharacter::Melee()
{
	if (!bIsInDialogue)
	{
		//if (bHasWeaponsHolstered)
		//	ToggleHolster();

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