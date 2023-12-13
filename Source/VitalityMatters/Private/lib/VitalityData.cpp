// Copyright Take Five Games, LLC 2023 - All Rights Reserved

#include "lib/VitalityData.h"

/**
 * @brief Sets the value of the core stat, running the appropriate
 *        logic and triggering delegates.
 * @param StatEnum The vitality stat to modify
 * @param NewValue The new value
 */
void FStVitalityStats::SetCoreStat(const EVitalityStat StatEnum, const int NewValue)
{
	const int enumAsIndex = static_cast<int>(StatEnum);
	CoreStats[enumAsIndex] = NewValue;
	OnCoreStatUpdated.Broadcast(StatEnum);
}

/**
 * @brief Sets the value of the damage bonus, running the appropriate
 *        logic and triggering delegates.
 * @param DamageEnum The damage stat to modify
 * @param NewValue The new value
 */
void FStVitalityStats::SetDamageBonus(const EDamageType DamageEnum, const int NewValue)
{
	const int enumAsIndex = static_cast<int>(DamageEnum);
	DamageBonuses[enumAsIndex] = NewValue;
	OnDamageBonusUpdated.Broadcast(DamageEnum);
}


/**
 * @brief Sets the value of the damage bonus, running the appropriate
 *        logic and triggering delegates.
 * @param DamageEnum The damage stat to modify
 * @param NewValue The new value
 */
void FStVitalityStats::SetDamageResistance(const EDamageType DamageEnum, const int NewValue)
{
	const int enumAsIndex = static_cast<int>(DamageEnum);
	DamageResists[enumAsIndex] = NewValue;
	OnDamageResistanceUpdated.Broadcast(DamageEnum);
}

/**
 * @brief Returns the value of the requested enum, internally iterating CoreStats
 * @param StatEnum The core stat enum to search for
 * @return The value of the core stat, or zero if invalid or missing
 */
float FStVitalityStats::GetCoreStatValue(const EVitalityStat StatEnum) const
{
	const int enumAsIndex = static_cast<int>(StatEnum);
	if (!CoreStats.IsValidIndex(enumAsIndex))
		return 0.f;
	return CoreStats[enumAsIndex];
}

/**
 * @brief Returns the value of the requested enum, internally iterating DamageBonuses
 * @param DamageEnum The damage resistance enum to search for
 * @return The value of the damage resistance, or zero if invalid or missing
 */
float FStVitalityStats::GetDamageBonusValue(const EDamageType DamageEnum) const
{
	const int enumAsIndex = static_cast<int>(DamageEnum);
	if (!DamageBonuses.IsValidIndex(enumAsIndex))
		return 0.f;
	return DamageBonuses[enumAsIndex];
}

/**
 * @brief Returns the value of the requested enum, internally iterating DamageResistances
 * @param DamageEnum The damage resistance enum to search for
 * @return The value of the damage resistance, or zero if invalid or missing
 */
float FStVitalityStats::GetDamageResistValue(const EDamageType DamageEnum) const
{
	const int enumAsIndex = static_cast<int>(DamageEnum);
	if (!DamageResists.IsValidIndex(enumAsIndex))
		return 0.f;
	return DamageResists[enumAsIndex];
}

