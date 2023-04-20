// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "StatusEffects.generated.h"

// A list of all beneficial effects available
UENUM(BlueprintType)
enum class EEffectsBeneficial : uint8
{
	NONE		UMETA(DisplayName = "No Effect"),
	NOURISHED	UMETA(DisplayName = "Nourished"),   // Health Regen
	HYDRATED	UMETA(DisplayName = "Hydrated"),    // Stamina Regen
	RESTED		UMETA(DisplayName = "Well Rested"), // Faster
	FOCUSED		UMETA(DisplayName = "Focused")		// Magic Regen
};

UENUM(BlueprintType)
enum class EEffectsDetrimental : uint8
{
	NONE	UMETA(DisplayName = "No Effect"),
	TIRED	UMETA(DisplayName = "Exhausted"),		// Can't Sprint
	SEASICK	UMETA(DisplayName = "Seasick"),			// Faster Hunger Degradation
	HEAT	UMETA(DisplayName = "Heat Exhaustion"), // Faster Thirst Degradation
	COLD	UMETA(DisplayName = "Freezing"),		// Slower Stamina Regen
	LEG		UMETA(DisplayName = "Leg Injured"),		// Slower move speed
	DIZZY	UMETA(DisplayName = "Disoriented"),		// Slower magic regen
	DEAF	UMETA(DisplayName = "Deafened"),		// Muffled Sound Effects
	ALERT	UMETA(DisplayName = "Vigilant"),		// In Combat or Dangerous Area
	ARM		UMETA(DisplayName = "Arm Injured"),		// Slower draw/attack times
	BLIND	UMETA(DisplayName = "Blinded"),			// Vision Obscured
	POISON	UMETA(DisplayName = "Poisoned"),		// Health Drain
	HUNGER  UMETA(DisplayName = "Starving"),		// Calories <= 0
	THIRST  UMETA(DisplayName = "Dehydrated"),		// Hydration <= 0
	DEAD	UMETA(DisplayName = "Dead")
};

USTRUCT(BlueprintType)
struct FStVitalityEffects : public FTableRowBase
{
	GENERATED_BODY()
	// The proper name of this effect. Usually the same as the data table row name
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName properName = "UniqueName";
	// The effect to apply. Overrides detrimentEffect
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EEffectsBeneficial benefitEffect = EEffectsBeneficial::NONE;
	// The effect to apply. Overriden if benefitEffect is not NONE
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EEffectsDetrimental detrimentEffect = EEffectsDetrimental::NONE;
	// The display name of the effect (such as hover text)
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString effectTitle = "Effect";
	// The icon to show when this effect is applied
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UTexture2D* effectIcon = nullptr;
	// This effect is permanent until death, destruction or manual removal.
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool isPersistent = false;
	// The maximum time the effect can last.
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float effectSeconds = 0.f;
	// If true, the optional actor spawned will attach to the Actor owning the vitality component
	// Does nothing if optionalClass is unused.
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool attachOnSpawn = true;
	// Optional actor class to spawn. Useful for sounds, effects, etc.
	// Leave as None/Empty/Null if not desired
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSubclassOf<AActor> optionalClass;
	// If true, disallows sprinting while this effect is active
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool disableSprinting = false;
};