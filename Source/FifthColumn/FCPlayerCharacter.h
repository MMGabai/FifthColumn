//Copyright (c) 2016, Mordechai M. Gabai

#pragma once

#include "FCCharacter.h"
#include "FCPlayerCharacter.generated.h"

UCLASS()
class FIFTHCOLUMN_API AFCPlayerCharacter : public AFCCharacter
{
	GENERATED_UCLASS_BODY()

	//pawn mesh: 1st person view
	UPROPERTY(EditDefaultsOnly, Category = Mesh)
		USkeletalMeshComponent* Mesh1P;

	//pawn mesh: 1st person view
	UPROPERTY(EditDefaultsOnly, Category = Mesh)
		USpringArmComponent* SpringArm;

	TSubclassOf<class APlayerCameraManager> CameraManager;

	virtual void BeginPlay() override;

	//Add Weapon into inventory
	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
		virtual bool AddWeapon(class AFCWeapon* Weapon) override;

	//equips weapon from inventory
	virtual void EquipWeapon(class AFCWeapon* Weapon) override;

	//next item
	void NextItem();

	//previous item
	void PreviousItem();

	void RemoveItem();

public: 

	//HUD
	UFUNCTION(BlueprintCallable, Category = "HUD")
		float GetHudRunOffTime() const;

	UFUNCTION(BlueprintCallable, Category = "HUD")
		void ExtendHudRunOffTime();

	bool HasActorTouched();

	UFUNCTION(BlueprintCallable, Category = "HUD")
		FText GetAmmoAmount();

	UFUNCTION(BlueprintCallable, Category = "HUD")
		FText GetWeaponName();

	UFUNCTION(BlueprintCallable, Category = "HUD")
		bool GetHandIcon();

	UFUNCTION(BlueprintCallable, Category = "HUD")
		bool GetWeaponNameEnabled();

	//GAMEPLAY
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void SendNotification(FText Message);

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		FText GetLatestMessage();

	UFUNCTION(BlueprintCallable, Category = "World State")
		virtual void SetDispositionTowardsCharacter(int32 choice, float change, bool redistribute = false) override;

	//sound played on notify
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
		USoundCue* NotificationSound;

	//sound played on notify
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
		USoundCue* FactionsChangedSound;

	TArray<FText> MessageStack;

	//Used to determine showing hand icon
	UFUNCTION(BlueprintCallable, Category = "HUD")
		bool GetHasTouchedActor();

	//DIALOGUE
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
		void StartDialogue(class AFCCharacter* Other);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
		void EndDialogue();

	//load new dialogue upon selection
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
		void SetCurrentDialogue(class ADialogue* DialogueToLoadInput);

	//Get Dialogue Options
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	class ADialogue* GetDialogueOption(int32 choice) const;

	//check dialogue option availability
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
		bool DialogueOptionAvailable(int32 choice) const;

	//dialogue related
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
		bool GetIsInDialogue() const;

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	class ADialogue* GetCurrentDialogue() const;

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	class ADialogue* GetDialogueToLoad() const;

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
		AFCCharacter* GetDialoguePartner() const;

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
		FText GetDialogueText() const;

	//JOURNAL
	//get current quest text
	UFUNCTION(BlueprintCallable, Category = "Assignments")
		FText GetCurrentQuestText();

	//get current quest description
	UFUNCTION(BlueprintCallable, Category = "Assignments")
		FText GetCurrentQuestDesc();

	UFUNCTION(BlueprintCallable, Category = Skill)
		void AddXP(int32 XPtoAdd);

	UFUNCTION(BlueprintCallable, Category = Skill)
		void LevelUp();

	UFUNCTION(BlueprintCallable, Category = Skill)
		int32 CheckXP() const;

	UFUNCTION(BlueprintCallable, Category = Skill)
		int32 GetSkillPointsToSpend() const;

	UFUNCTION(BlueprintCallable, Category = Skill)
		int32 CheckXPRequiredForLevelling() const;

	UFUNCTION(BlueprintCallable, Category = Skill)
		int32 CheckLevel() const;

	UFUNCTION(BlueprintCallable, Category = Skill)
		void IncreaseSkill(int32 SkillCategory);

	UFUNCTION(BlueprintCallable, Category = Skill)
		int32 GetSkill(int32 SkillCategory);

	UFUNCTION(BlueprintCallable, Category = Skill)
		bool GetSkillAvailability(int32 SkillCategory);

	//BOOK
	UFUNCTION(BlueprintCallable, Category = Book)
		bool GetIsReading() const;

	UFUNCTION(BlueprintCallable, Category = Book)
		void SetIsReading(bool SetValue = true);

	UFUNCTION(BlueprintCallable, Category = Book)
		void SetCurrentBook(AActivatableObject* BookInput);

	UFUNCTION(BlueprintCallable, Category = Book)
		AActivatableObject* GetCurrentBook() const;

	/// QUESTS
	UPROPERTY(VisibleDefaultsOnly, Category = Quest)
		TArray<class AQuest*> ActiveQuests;

	UFUNCTION(BlueprintCallable, Category = Quest)
		AQuest* SelectActiveQuest(TSubclassOf<AQuest> SelectedQuest);

	bool HasPrimaryActiveQuest();

	UFUNCTION(BlueprintCallable, Category = Quest)
		AQuest* GetPrimaryActiveQuest() const;

	UFUNCTION(BlueprintCallable, Category = Quest)
		void SelectNextQuest();

	UFUNCTION(BlueprintCallable, Category = Quest)
		void SelectPrevQuest();

	AQuest* PrimaryActiveQuest;

	//Update the character. (Running, health etc).
	virtual void Tick(float DeltaSeconds) override;

	//INPUT
	//setup pawn specific input handlers 
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	void ToggleHolster();

	void Melee();

	//STEALTH
	UFUNCTION(BlueprintCallable, Category = Behavior)
		bool GetIsTrespassing() const;

	//toggle trespassing
	UFUNCTION(BlueprintCallable, Category = Behavior)
		void ToggleTrespassing(bool ToggleTrue = true);

private:

	//STEALTH
	bool bIsTrespassing;

	//HUD
	float HUDRunOffTime;

protected:

	TArray<ADialogue*> LoadedDialogueOptions;

	//INVENTORY
	//use item in the game world
	void Use();

	AActor* CurrentActorTouched;

	//DIALOGUE
	class ANPC* DialoguePartner;

	//The dialogue currently loaded and to be shown
	UPROPERTY(Transient)
	class ADialogue* CurrentDialogue;

	UPROPERTY(Transient)
	class ADialogue* CurrentDialogueOptions;

	UPROPERTY(Transient)
	class ADialogue* DialogueToLoad;

	//BOOK
	bool bIsReading;

	//The book currently loaded and to be shown
	UPROPERTY(Transient)
	class AActivatableObject* CurrentBook;
};
