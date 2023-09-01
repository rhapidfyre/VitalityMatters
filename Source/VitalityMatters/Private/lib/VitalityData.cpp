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
	for (FStVitalityStatMap& StatMap : CoreStats)
	{
		if (StatMap.StatEnum == StatEnum)
		{
			StatMap.MapValue = NewValue;
			return;
		}
	}
	CoreStats.Add( FStVitalityStatMap(StatEnum, NewValue) );
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
	for (FStVitalityDamageMap& DamageMap : DamageBonuses)
	{
		if (DamageMap.DamageType == DamageEnum)
		{
			DamageMap.MapValue = NewValue;
			return;
		}
	}
	DamageBonuses.Add( FStVitalityDamageMap(DamageEnum, NewValue) );
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
	for (FStVitalityDamageMap& DamageMap : DamageResistances)
	{
		if (DamageMap.DamageType == DamageEnum)
		{
			DamageMap.MapValue = NewValue;
			return;
		}
	}
	DamageResistances.Add( FStVitalityDamageMap(DamageEnum, NewValue) );
	OnDamageResistanceUpdated.Broadcast(DamageEnum);
}

/**
 * @brief Returns the value of the requested enum, internally iterating CoreStats
 * @param StatEnum The core stat enum to search for
 * @return The value of the core stat, or zero if invalid or missing
 */
float FStVitalityStats::GetCoreStatValue(const EVitalityStat StatEnum) const
{
	for (const FStVitalityStatMap& DamageMap : CoreStats)
	{
		if (DamageMap.StatEnum == StatEnum)
			return DamageMap.MapValue;
	}
	return 0.f;
}

/**
 * @brief Returns the value of the requested enum, internally iterating DamageBonuses
 * @param DamageEnum The damage resistance enum to search for
 * @return The value of the damage resistance, or zero if invalid or missing
 */
float FStVitalityStats::GetDamageBonusValue(const EDamageType DamageEnum) const
{
	for (const FStVitalityDamageMap& DamageMap : DamageBonuses)
	{
		if (DamageMap.DamageType == DamageEnum)
			return DamageMap.MapValue;
	}
	return 0.f;
}

/**
 * @brief Returns the value of the requested enum, internally iterating DamageResistances
 * @param DamageEnum The damage resistance enum to search for
 * @return The value of the damage resistance, or zero if invalid or missing
 */
float FStVitalityStats::GetDamageResistValue(const EDamageType DamageEnum) const
{
	for (const FStVitalityDamageMap& DamageMap : DamageResistances)
	{
		if (DamageMap.DamageType == DamageEnum)
			return DamageMap.MapValue;
	}
	return 0.f;
}

