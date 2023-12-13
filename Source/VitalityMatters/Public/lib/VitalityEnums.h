
#pragma once

#include "CoreMinimal.h"

// A vitality status is a welfare status such as alive, dead, injured, etc
UENUM(BlueprintType)
enum class EVitalityStatus : uint8
{
	DEAD = 0	UMETA(Display = "Dead"),
	ALIVE		UMETA(Display = "Healthy"),
	INJURED		UMETA(Display = "Injured"),
	DOWN		UMETA(Display = "Unconscious"),
	MAX			UMETA(Hidden)
};

// A vitality stat is a core stat such as strength or agility
UENUM(BlueprintType)
enum class EVitalityStat : uint8
{
	STRENGTH = 0	UMETA(DisplayName = "Strength"),
	AGILITY			UMETA(DisplayName = "Agility"),
	FORTITUDE		UMETA(DisplayName = "Fortitude"),
	INTELLECT		UMETA(DisplayName = "Intellect"),
	ASTUTENESS		UMETA(DisplayName = "Astuteness"),
	CHARISMA		UMETA(DisplayName = "Charisma"),
	MAX			UMETA(Hidden)
};

// A Vitality Category is a type of vitality value, such as health or hunger.
UENUM(BlueprintType)
enum class EVitalityCategory : uint8
{
	HEALTH		UMETA(DisplayName = "Health"),
	STAMINA     UMETA(DisplayName = "Stamina"),
	MAGIC		UMETA(DisplayName = "Magic"),
	HUNGER		UMETA(DisplayName = "Hunger"),
	THIRST		UMETA(DisplayName = "Thirst"),
	MAX			UMETA(Hidden)
};

// Combat State is used to track the actors sympathetic nervous system status
UENUM(BlueprintType)
enum class ECombatState : uint8
{
	RELAXED = 0 UMETA(DisplayName = "Relaxed"),
	ALERT    	UMETA(DisplayName = "Alert"),
	ENGAGED  	UMETA(DisplayName = "Engaged"),
	RECOVERY 	UMETA(DisplayName = "Recovering"),
	INJURED  	UMETA(DisplayName = "Incapacitated"),
	MAX			UMETA(DisplayName = "Not Applicable"),
};

// A list of all beneficial effects available
UENUM(BlueprintType)
enum class EEffectsBeneficial : uint8
{
	NOURISHED = 0	UMETA(DisplayName = "Nourished"),   // Health Regen
	HYDRATED		UMETA(DisplayName = "Hydrated"),    // Stamina Regen
	RESTED			UMETA(DisplayName = "Well Rested"), // Faster
	FOCUSED			UMETA(DisplayName = "Focused"),		// Magic Regen
	MAX				UMETA(Hidden)
};

// A list of all detrimental effects available
UENUM(BlueprintType)
enum class EEffectsDetrimental : uint8
{
	TIRED = 0	UMETA(DisplayName = "Exhausted"),		// Can't Sprint
	SEASICK		UMETA(DisplayName = "Seasick"),			// Faster Hunger Degradation
	HEAT		UMETA(DisplayName = "Heat Exhaustion"), // Faster Thirst Degradation
	COLD		UMETA(DisplayName = "Freezing"),		// Slower Stamina Regen
	LEG			UMETA(DisplayName = "Leg Injured"),		// Slower move speed
	DIZZY		UMETA(DisplayName = "Disoriented"),		// Slower magic regen
	DEAF		UMETA(DisplayName = "Deafened"),		// Muffled Sound Effects
	ALERT		UMETA(DisplayName = "Vigilant"),		// In Combat or Dangerous Area
	ARM			UMETA(DisplayName = "Arm Injured"),		// Slower draw/attack times
	BLIND		UMETA(DisplayName = "Blinded"),			// Vision Obscured
	POISON		UMETA(DisplayName = "Poisoned"),		// Health Drain
	HUNGER  	UMETA(DisplayName = "Starving"),		// Calories <= 0
	THIRST  	UMETA(DisplayName = "Dehydrated"),		// Hydration <= 0
	DEAD		UMETA(DisplayName = "Dead"),
	MAX			UMETA(Hidden)
};

// All of the damage types available for this game
UENUM(BlueprintType)
enum class EDamageType : uint8
{
	// Any damage not found or specified below. Cannot be resisted
	ADMIN = 0	UMETA(DisplayName="World"),
	
	// Any damage dealt by a physical force, such as a rock
	PHYSICAL	UMETA(DisplayName="Physical"),
	
	// Any damage dealt by a natural element, such as acid or weather
	NATURE		UMETA(DisplayName="Nature"),
	
	// Any damage dealt by an element that can't be categorized by heat, cold, etc
	ELEMENTAL	UMETA(DisplayName="Elemental"),
	
	// Any damage dealt by sound, such as a shock wave
	SONIC		UMETA(DisplayName="Sonic"),
	
	// Any damage dealt by heat, fire or radiant energy
	HEAT 		UMETA(DisplayName="Heat"),
	
	// Any damage dealt by frost, cold or ice
	COLD 		UMETA(DisplayName="Cold"),
	
	// Any damage dealt by electricity, such as lightning
	SHOCK		UMETA(DisplayName="Shock"),
	
	// Any damage dealt by acidic material, such as corrosives
	ACID 		UMETA(DisplayName="Acidic"),
	
	// Any damage dealt by uncategorized magical effects
	MAGIC 		UMETA(DisplayName="Magic"),
	
	// Any damage dealt by evil magic, such as necrotic energy
	DARK 		UMETA(DisplayName="Dark Magic"),
	
	// Any damage dealt by holy magic. Heals living things.
	HOLY 		UMETA(DisplayName="Divine Magic"),
	
	// Any damage dealt by sharp edges, such as a sword
	SLASH		UMETA(DisplayName="Slashing"),
	
	// Any damage dealt by rigid edges, such as a club
	BLUNT		UMETA(DisplayName="Blunt"),
	
	// Any damage dealt by stabbing, such as floor spikes and daggers
	PIERCE		UMETA(DisplayName="Piercing"),
	
	// Any damage dealt by toxic material, such as radiation or hazardous waste
	TOXIC		UMETA(DisplayName="Chemical"),

	MAX			UMETA(Hidden)
};

// A list of all values that wielding equipment can modify
// Is this obsolete?
UENUM(BlueprintType)
enum class EEquipmentValues : uint8
{
	RESIST_PHYSICAL	UMETA(DisplayName="Damage Mitigation"),
	RESIST_MAGIC	UMETA(DisplayName="Total Magic Resistance"),
	RESIST_DARK		UMETA(DisplayName="Dark Magic Resistance"),
	RESIST_HOLY		UMETA(DisplayName="Holy Magic Resistance"),
	RESIST_POISON	UMETA(DisplayName="Poison Resistance"),
	RESIST_DISEASE	UMETA(DisplayName="Disease Resistance"),
	RESIST_ACID		UMETA(DisplayName="Acid Resistance"),
	RESIST_FIRE		UMETA(DisplayName="Fire Resistance"),
	RESIST_COLD		UMETA(DisplayName="Cold Resistance"),
	RESIST_ELECTRIC	UMETA(DisplayName="Electric Resistance"),
	
	DAMAGE_MELEE	UMETA(DisplayName="Melee Damage Bonus"),
	DAMAGE_RANGE	UMETA(DisplayName="Range Damage Bonus"),
	DAMAGE_VARIANCE UMETA(DisplayName="Less Damage Variance"),
	DAMAGE_MAGIC	UMETA(DisplayName="Magic Damage"),
	DAMAGE_DARK		UMETA(DisplayName="Dark Magic Damage"),
	DAMAGE_HOLY		UMETA(DisplayName="Holy Magic Damage"),
	DAMAGE_POISON	UMETA(DisplayName="Poison Damage"),
	DAMAGE_FIRE		UMETA(DisplayName="Fire Damage"),
	DAMAGE_COLD		UMETA(DisplayName="Cold Damage"),
	DAMAGE_ELECTRIC	UMETA(DisplayName="Electric Damage"),
	
	HEALTH			UMETA(DisplayName="Max Health"),
	STAMINA			UMETA(DisplayName="Max Stamina"),
	MANA			UMETA(DisplayName="Max Magic"),
	HUNGER			UMETA(DisplayName="Hunger Consumption"),
	THIRST			UMETA(DisplayName="Thirst Consumption"),
	FOCUS			UMETA(DisplayName="Enhanced Focus"),
	
	STAT_STRENGTH	UMETA(DisplayName="Strength Bonus"),
	STAT_AGILITY	UMETA(DisplayName="Agility Bonus"),
	STAT_FORTITUDE	UMETA(DisplayName="Fortitude Bonus"),
	STAT_ASTUTENESS	UMETA(DisplayName="Astuteness Bonus"),
	STAT_INTELLECT	UMETA(DisplayName="Intellect Bonus"),
	STAT_CHARISMA	UMETA(DisplayName="Charisma Bonus"),
	
	MAX			UMETA(Hidden)
};