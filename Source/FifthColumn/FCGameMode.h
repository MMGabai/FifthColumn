//Copyright (c) 2017, Mordechai M. Gabai

#pragma once

#include "GameFramework/GameMode.h"
#include "FCGameMode.generated.h"

/**
 * 
 */
UCLASS()
class FIFTHCOLUMN_API AFCGameMode : public AGameMode
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, Category = Difficulty)
		float EnemyHPMultiplier;

	UPROPERTY(EditAnywhere, Category = Difficulty)
		float PlayerHPMultiplier;
	
	UPROPERTY(EditAnywhere, Category = Difficulty)
		float SkillIncreaseMultiplier;

	UFUNCTION(BlueprintCallable, Category = "World State")
		void AddIntoBlockedDialogueName(FString AbstractName);

	UFUNCTION(BlueprintCallable, Category = "World State")
		void RemoveFromBlockedDialogueName(FString AbstractName);

	UFUNCTION(BlueprintCallable, Category = "World State")
		bool CheckForDialogueName(FString AbstractName);

	UFUNCTION(BlueprintCallable, Category = "Config")
		UGameUserSettings* GetUserSettings() const;

	UFUNCTION(BlueprintCallable, Category = config)
		FText GetGameResolution();

	UFUNCTION(BlueprintCallable, Category = config)
		FText GetAspectRatio();

	//GRAPHICS OPTIONS
	//Resolution options
	UFUNCTION(BlueprintCallable, Category = config)
		void SetGameResolution(int32 ResolutionX, int32 ResolutionY);

	UFUNCTION(BlueprintCallable, Category = config)
		void GetCurrentGameResolution();

	UFUNCTION(BlueprintCallable, Category = config)
		void IncreaseResolution();

	UFUNCTION(BlueprintCallable, Category = config)
		void DecreaseResolution();

	UFUNCTION(BlueprintCallable, Category = config)
		void IncreaseAspectRatio();

	UFUNCTION(BlueprintCallable, Category = config)
		void DecreaseAspectRatio();

	int32 AspectRatioScale;
	int32 ResolutionScale;

public:
	//Fullscreen Options
	UFUNCTION(BlueprintCallable, Category = config)
		void SetFullScreen();

	UFUNCTION(BlueprintCallable, Category = config)
		void SetWindowed();

	UFUNCTION(BlueprintCallable, Category = config)
		bool GetFullscreenSettings() const;

	//AA Options
	UFUNCTION(BlueprintCallable, Category = config)
		int32 GetAA() const;

	UFUNCTION(BlueprintCallable, Category = config)
		void SetAA(int32 AA);

	//VSync Options
	UFUNCTION(BlueprintCallable, Category = config)
		bool GetVSync() const;

	UFUNCTION(BlueprintCallable, Category = config)
		void SetVSync(bool VSyncOn);

	//Gamma Options
	UFUNCTION(BlueprintCallable, Category = config)
		float GetGamma() const;

	UFUNCTION(BlueprintCallable, Category = config)
		void SetGammaOption(float GammaOption);

	//Shadows Options
	UFUNCTION(BlueprintCallable, Category = config)
		int32 GetShadowQuality() const;

	UFUNCTION(BlueprintCallable, Category = config)
		void SetShadowQualityOption(int32 ShadowQualityOption);

	//Postprocessing Options
	UFUNCTION(BlueprintCallable, Category = config)
		int32 GetPostProcessing() const;

	UFUNCTION(BlueprintCallable, Category = config)
		void SetPostProcessingOption(int32 PostProcessingOption);

	//Effects Options
	UFUNCTION(BlueprintCallable, Category = config)
		int32 GetFXOption() const;

	UFUNCTION(BlueprintCallable, Category = config)
		void SetFXOption(int32 FXOption);

	//Texture Options
	UFUNCTION(BlueprintCallable, Category = config)
		int32 GetTextureOption() const;

	UFUNCTION(BlueprintCallable, Category = config)
		void SetTextureOption(int32 TextureOption);

	//View Distance Options
	UFUNCTION(BlueprintCallable, Category = config)
		float GetViewdistance() const;

	UFUNCTION(BlueprintCallable, Category = config)
		void SetViewdistance(float ViewDistance);

	//GAMEPLAY SETTINGS
	//Axis inversion
	UFUNCTION(BlueprintCallable, Category = config)
		void InvertYAxis(bool Invert = true);

	bool YAxisInverted;

	UFUNCTION(BlueprintCallable, Category = config)
		void InvertXAxis(bool Invert = true);

	bool XAxisInverted;

	//Field of View
	UFUNCTION(BlueprintCallable, Category = config)
		float GetFOV() const;

	UFUNCTION(BlueprintCallable, Category = config)
		void SetFOV(float FOVinput);

	UPROPERTY(EditDefaultsOnly, Category = config)
		float FOV;

	//Aim sensitivity
	UFUNCTION(BlueprintCallable, Category = config)
		float GetAimSensitivity() const;

	UFUNCTION(BlueprintCallable, Category = config)
		void SetAimSensitivity(float AimSensitivityInput);

	float AimSensitivity;

	//Player Health offset
	UFUNCTION(BlueprintCallable, Category = config)
		float GetPlayerHealthOffset() const;

	UFUNCTION(BlueprintCallable, Category = config)
		void SetPlayerHealthOffset(float PlayerHealthOffsetInput);

	//Enemy Health offset
	UFUNCTION(BlueprintCallable, Category = config)
		float GetEnemyHealthOffset() const;

	UFUNCTION(BlueprintCallable, Category = config)
		void SetEnemyHealthOffset(float EnemyHealthOffsetInput);

	//Skill offset
	UFUNCTION(BlueprintCallable, Category = config)
		float GetSkillOffset() const;

	UFUNCTION(BlueprintCallable, Category = config)
		void SetSkillOffset(float SkillOffsetInput);

	//APPLY SETTINGS
	UFUNCTION(BlueprintCallable, Category = config)
		void ApplySettingsInGame();

	UFUNCTION(BlueprintCallable, Category = config)
		void RevertSettingsInGame();

private:

	UPROPERTY(EditAnywhere, Category = config)
		FIntPoint CurrentResolution;

	UPROPERTY(EditAnywhere, Category = config)
		FIntPoint CurrentAspectRatio;

	//List of abstract names of dialogue that are locked
	UPROPERTY(EditAnywhere, Category = "World State")
		TArray<FString> BlockedDialogueNames;
};

