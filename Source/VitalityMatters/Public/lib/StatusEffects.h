// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

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
};