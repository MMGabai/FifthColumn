//Copyright (c) 2017, Mordechai M. Gabai

#include "FifthColumn.h"
#include "FCGameMode.h"

AFCGameMode::AFCGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	EnemyHPMultiplier = 1.0f;
	PlayerHPMultiplier = 1.0f;
	SkillIncreaseMultiplier = 1.0f;
	FOV = 90.0f;
	BlockedDialogueNames.AddZeroed(5);
	ResolutionScale = 4;
	AspectRatioScale = 1;
	YAxisInverted = false;
	XAxisInverted = false;

	//Set FCPLayerController as Default Player Controller
	PlayerControllerClass = AFCPlayerController::StaticClass();
	DefaultPawnClass = AFCPlayerCharacter::StaticClass();
}

void AFCGameMode::AddIntoBlockedDialogueName(FString AbstractName) 
{
	BlockedDialogueNames.Add(AbstractName);
}

void AFCGameMode::RemoveFromBlockedDialogueName(FString AbstractName) 
{
	BlockedDialogueNames.Remove(AbstractName);
}

bool AFCGameMode::CheckForDialogueName(FString AbstractName)
{
	return BlockedDialogueNames.Contains(AbstractName);
}

UGameUserSettings* AFCGameMode::GetUserSettings() const 
{
	return GEngine->GetGameUserSettings();
}

//Graphix Options
//Resolution
FText AFCGameMode::GetGameResolution()
{
	const FText ResolutionText = FText::Format(FText::FromString("{0}x{1}"), FText::FromString(FString::FromInt(CurrentResolution.X)), FText::FromString(FString::FromInt(CurrentResolution.Y)));

	return ResolutionText;
}

FText AFCGameMode::GetAspectRatio()
{
	FText AspectRatioText;

	switch (AspectRatioScale)
	{
	case 1:
		AspectRatioText = FText::FromString("16:9");
		break;
	case 2:
		AspectRatioText = FText::FromString("16:10");
		break;
	case 3:
		AspectRatioText = FText::FromString("4:3");
		break;
	}

	return AspectRatioText;
}


void AFCGameMode::SetGameResolution(int32 ResolutionX, int32 ResolutionY)
{
	FIntPoint Resolution = FIntPoint(ResolutionX, ResolutionY);
	GEngine->GetGameUserSettings()->SetScreenResolution(Resolution);
}

void AFCGameMode::IncreaseAspectRatio()
{
	switch (AspectRatioScale)
	{
	case 1:
		AspectRatioScale = 2;
		break;
	case 2:
		AspectRatioScale = 1;
		break;
	/*case 3:
		AspectRatioScale = 1;
		break;*/
	}
}

void AFCGameMode::DecreaseAspectRatio()
{
	switch (AspectRatioScale)
	{
	case 1:
		AspectRatioScale = 2;
		break;
	case 2:
		AspectRatioScale = 1;
		break;
	/*case 3:
		AspectRatioScale = 2;
		break;*/
	}
}


void AFCGameMode::GetCurrentGameResolution()
{
	UGameUserSettings* GameUserSettings = GEngine->GetGameUserSettings();
	FIntPoint Resolution = GameUserSettings->GetScreenResolution();
	CurrentResolution.X = Resolution.X;
	CurrentResolution.Y = Resolution.Y;
}

void AFCGameMode::IncreaseResolution() 
{
	ResolutionScale++;
	AspectRatioScale = 1;
	CurrentResolution.Y = 180 * ResolutionScale;

	switch (AspectRatioScale)
	{
	case 1:
		CurrentResolution.X = (CurrentResolution.Y / 9) * 16;
		break;
	case 2:
		CurrentResolution.X = (CurrentResolution.Y / 10) * 16;
		break;
	case 3:
		CurrentResolution.X = (CurrentResolution.Y / 3) * 4;
		break;
	}

	//Max resolution is 1920 by 1080 for now
	CurrentResolution.X = FMath::Min(1920, CurrentResolution.X);
	CurrentResolution.Y = FMath::Min(1080, CurrentResolution.Y);

	SetGameResolution(CurrentResolution.X, CurrentResolution.Y);
	
}

void AFCGameMode::DecreaseResolution()
{
	ResolutionScale--;
	AspectRatioScale = 1;
	CurrentResolution.Y = 180 * ResolutionScale;
	
	switch (AspectRatioScale)
	{
	case 1:
		CurrentResolution.X = (CurrentResolution.Y / 9) * 16;
		break;
	case 2:
		CurrentResolution.X = (CurrentResolution.Y / 10) * 16;
		break;
	case 3:
		CurrentResolution.X = (CurrentResolution.Y / 3) * 4;
		break;
	}

	//Min resolution is 852 by 480
	CurrentResolution.X = FMath::Max(852, CurrentResolution.X);
	CurrentResolution.Y = FMath::Max(480, CurrentResolution.Y);

	SetGameResolution(CurrentResolution.X, CurrentResolution.Y);

}

//Fullscreen vs. windowed
void AFCGameMode::SetFullScreen() 
{
	GEngine->GetGameUserSettings()->SetFullscreenMode(EWindowMode::Fullscreen);
}

void AFCGameMode::SetWindowed() 
{
	GEngine->GetGameUserSettings()->SetFullscreenMode(EWindowMode::Windowed);
}

bool AFCGameMode::GetFullscreenSettings() const
{
	return (GEngine->GetGameUserSettings()->GetFullscreenMode()) == EWindowMode::Fullscreen;
}

//AA Options
int32 AFCGameMode::GetAA() const
{
	return GEngine->GetGameUserSettings()->ScalabilityQuality.AntiAliasingQuality;
}

void AFCGameMode::SetAA(int32 AA)
{
	GEngine->GetGameUserSettings()->ScalabilityQuality.AntiAliasingQuality = AA;
}

//VSync
bool AFCGameMode::GetVSync() const
{
	return GEngine->GetGameUserSettings()->bUseVSync;
}

void AFCGameMode::SetVSync(bool VSyncOn) 
{
	GEngine->GetGameUserSettings()->bUseVSync = VSyncOn;
}

//Gamma Options
float AFCGameMode::GetGamma() const 
{
	return GEngine->GetDisplayGamma();
}

void AFCGameMode::SetGammaOption(float GammaOption) 
{
	GEngine->DisplayGamma = GammaOption;
}

//Shadows Options
int32 AFCGameMode::GetShadowQuality() const
{
	return GEngine->GetGameUserSettings()->ScalabilityQuality.ShadowQuality;
}

void AFCGameMode::SetShadowQualityOption(int32 ShadowQualityOption)
{
	GEngine->GetGameUserSettings()->ScalabilityQuality.ShadowQuality = ShadowQualityOption;
}

//Postprocessing Options
int32 AFCGameMode::GetPostProcessing() const
{
	return GEngine->GetGameUserSettings()->ScalabilityQuality.PostProcessQuality;
}

void AFCGameMode::SetPostProcessingOption(int32 PostProcessingOption) 
{ 
	GEngine->GetGameUserSettings()->ScalabilityQuality.PostProcessQuality = PostProcessingOption;
}

//Effects Options
int32 AFCGameMode::GetFXOption() const
{
	return GEngine->GetGameUserSettings()->ScalabilityQuality.EffectsQuality;
}

void AFCGameMode::SetFXOption(int32 FXOption)
{
	GEngine->GetGameUserSettings()->ScalabilityQuality.EffectsQuality = FXOption;
}

//Texture Options
int32 AFCGameMode::GetTextureOption() const
{
	return GEngine->GetGameUserSettings()->ScalabilityQuality.TextureQuality;
}

void AFCGameMode::SetTextureOption(int32 TextureOption)
{
	GEngine->GetGameUserSettings()->ScalabilityQuality.TextureQuality = TextureOption;
}

//View Distance Options
float AFCGameMode::GetViewdistance() const
{
	return GEngine->GetGameUserSettings()->ScalabilityQuality.ViewDistanceQuality;
}

void AFCGameMode::SetViewdistance(float ViewDistance) { GEngine->GetGameUserSettings()->ScalabilityQuality.ViewDistanceQuality = ViewDistance; }

//Controls

//Gameplay Options
void AFCGameMode::InvertYAxis(bool Invert) 
{
	YAxisInverted = Invert;
}

void AFCGameMode::InvertXAxis(bool Invert)
{
	XAxisInverted = Invert;
}

float AFCGameMode::GetFOV() const 
{
	return FOV;
}

void AFCGameMode::SetFOV(float FOVinput)
{
	FOV = FOVinput;
}

//Aim sensitivity
float AFCGameMode::GetAimSensitivity() const
{
	return AimSensitivity;
}

void AFCGameMode::SetAimSensitivity(float AimSensitivityInput)
{
	AimSensitivity = AimSensitivityInput;
}

float AFCGameMode::GetPlayerHealthOffset() const
{
	return PlayerHPMultiplier;
}

void AFCGameMode::SetPlayerHealthOffset(float PlayerHealthOffsetInput)
{
	PlayerHPMultiplier = PlayerHealthOffsetInput;
}

//Enemy Health offset
float AFCGameMode::GetEnemyHealthOffset() const 
{
	return EnemyHPMultiplier;
}

void AFCGameMode::SetEnemyHealthOffset(float PlayerHealthOffsetInput)
{
	EnemyHPMultiplier = PlayerHealthOffsetInput;
}

//Skill offset
float AFCGameMode::GetSkillOffset() const
{
	return SkillIncreaseMultiplier;
}

void AFCGameMode::SetSkillOffset(float SkillOffsetInput) 
{
	SkillIncreaseMultiplier = SkillOffsetInput;
}

//Audio Options

//APPLY SETTINGS
void AFCGameMode::ApplySettingsInGame() 
{
	GEngine->GetGameUserSettings()->ApplySettings(true);
	GEngine->GetGameUserSettings()->SaveSettings();
}

void AFCGameMode::RevertSettingsInGame()
{
	GEngine->GetGameUserSettings()->RevertVideoMode();
}