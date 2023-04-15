
#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EVitalityStatus : uint8
{
	DEAD		UMETA(Display = "Dead"),
	ALIVE		UMETA(Display = "Healthy"),
	INJURED		UMETA(Display = "Injured"),
	DOWN		UMETA(Display = "Unconscious"),
};

UENUM(BlueprintType)
enum class EVitalityCategories : uint8
{
	HEALTH		UMETA(DisplayName = "Health"),
	STAMINA     UMETA(DisplayName = "Stamina"),
	MAGIC		UMETA(DisplayName = "Magic"),
	HUNGER		UMETA(DisplayName = "Hunger"),
	THIRST		UMETA(DisplayName = "Thirst"),
};