// Copyright Take Five Games, LLC 2023 - All Rights Reserved


#include "VitalityStatComponent.h"

#include "Net/UnrealNetwork.h"


UVitalityStatComponent::UVitalityStatComponent()
{
	SetIsReplicatedByDefault(true);
}

/**
 * @brief Returns the current value of the requested vitality stat
 * @param VitalityStat Stat Enum to return
 * @return The current value of the requested vitality stat
 */
float UVitalityStatComponent::GetVitalityStat(EVitalityStat VitalityStat) const
{
	switch (VitalityStat)
	{
	case EVitalityStat::STRENGTH:	return _Strength;
	case EVitalityStat::AGILITY:	return _Agility;
	case EVitalityStat::FORTITUDE: 	return _Fortitude;
	case EVitalityStat::INTELLECT: 	return _Intellect;
	case EVitalityStat::ASTUTENESS: return _Astuteness;
	default:						break;
	}
	return _Charisma;
}

/**
 * @brief Sets the given Vitality Stat to the exact value given
 * @param VitalityStat Stat Enum to be affected
 * @param NewValue The new value (exact) for the given Stat Enum
 */
void UVitalityStatComponent::SetVitalityStat(const EVitalityStat VitalityStat, float NewValue)
{
	switch (VitalityStat)
	{
	case EVitalityStat::STRENGTH:   SetStrength(NewValue);   break;
	case EVitalityStat::AGILITY:    SetAgility(NewValue);    break;
	case EVitalityStat::FORTITUDE:  SetFortitude(NewValue);  break;
	case EVitalityStat::INTELLECT:  SetIntellect(NewValue);  break;
	case EVitalityStat::ASTUTENESS: SetAstuteness(NewValue); break;
	case EVitalityStat::CHARISMA:   SetCharisma(NewValue);   break;
	default: break;
	}
}

/**
 * @brief Takes the current value of the given stat and modifies it
 * @param VitalityStat Stat Enum to affect
 * @param AddValue The amount to add (or remove)
 */
void UVitalityStatComponent::ModifyVitalityStat(EVitalityCategory VitalityStat, float AddValue)
{
	switch (VitalityStat)
	{
	case EVitalityStat::STRENGTH:   SetStrength(GetStrength() + AddValue);     break;
	case EVitalityStat::AGILITY:    SetAgility(GetAgility() + AddValue);       break;
	case EVitalityStat::FORTITUDE:  SetFortitude(GetFortitude() + AddValue);   break;
	case EVitalityStat::INTELLECT:  SetIntellect(GetIntellect() + AddValue);   break;
	case EVitalityStat::ASTUTENESS: SetAstuteness(GetAstuteness() + AddValue); break;
	case EVitalityStat::CHARISMA:   SetCharisma(GetCharisma() + AddValue);     break;
	default: break;
	}
}

/**
 * @brief Sets the given damage resistance as natural resistance enum to the given value
 * @param DamageEnum The enum specifying the damage type
 * @param NewValue The new actual value of the damage resistance
 * @return True if set was successful. False otherwise.
 */
bool UVitalityStatComponent::SetNaturalResistanceValue(EDamageType DamageEnum, int NewValue)
{
	return SetNewDamageResistanceValue(&_BaseStats, DamageEnum, NewValue);
}

/**
 * @brief Sets the given damage resistance as gear resistance enum to the given value
 * @param DamageEnum The enum specifying the damage type
 * @param NewValue The new value to set the gear based resistance to
 * @return True on success, false otherwise
 */
bool UVitalityStatComponent::SetGearResistanceValue(EDamageType DamageEnum, int NewValue)
{
	return SetNewDamageResistanceValue(&_GearStats, DamageEnum, NewValue);
}

/**
* @brief Sets the given damage resistance as magical resistance with the given value
 * @param DamageEnum The enum specifying the damage type
 * @param NewValue The new value to set the magic-based resistance to
 * @return True on success, false otherwise
 */
bool UVitalityStatComponent::SetMagicalResistanceValue(EDamageType DamageEnum, int NewValue)
{
	return SetNewDamageResistanceValue(&_ModifiedStats, DamageEnum, NewValue);
}

/**
* @brief Sets the given damage resistance as other-type resistance with the given value
 * @param DamageEnum The enum specifying the damage type
 * @param NewValue The new value to set the other resistance to
 * @return True on success, false otherwise
 */
bool UVitalityStatComponent::SetOtherResistanceValue(EDamageType DamageEnum, int NewValue)
{
	return SetNewDamageResistanceValue(&_OtherStats, DamageEnum, NewValue);
}

/**
 * @brief 
 * @param DamageEnum The damage enum to get the natural resistance for
 * @return Returns the resistance value, or zero if it does not exist
 */
float UVitalityStatComponent::GetNaturalResistanceValue(EDamageType DamageEnum) const
{
	return GetDamageResistanceValue(&_BaseStats, DamageEnum);
}

/**
 * @brief 
 * @param DamageEnum The damage enum to get the gear resistance for
 * @return Returns the resistance value, or zero if it does not exist
 */
float UVitalityStatComponent::GetGearResistanceValue(EDamageType DamageEnum) const
{
	return GetDamageResistanceValue(&_GearStats, DamageEnum);
}

/**
 * @brief 
 * @param DamageEnum The damage enum to get the magical resistance for
 * @return Returns the resistance value, or zero if it does not exist
 */
float UVitalityStatComponent::GetMagicalResistanceValue(EDamageType DamageEnum) const
{
	return GetDamageResistanceValue(&_ModifiedStats, DamageEnum);
}

/**
 * @brief 
 * @param DamageEnum The damage enum to get the other-type resistance for
 * @return Returns the resistance value, or zero if it does not exist
 */
float UVitalityStatComponent::GetOtherResistanceValue(EDamageType DamageEnum) const
{
	return GetDamageResistanceValue(&_OtherStats, DamageEnum);
}

/**
* @brief Sets the given damage bonus as natural bonus with the given value
 * @param DamageEnum The enum specifying the damage bonus type
 * @param NewValue The new value to set the bonus to
 * @return True on success, false otherwise
 */
bool UVitalityStatComponent::SetNaturalDamageBonusValue(EDamageType DamageEnum, int NewValue)
{
	return SetNewDamageBonusValue(&_BaseStats, DamageEnum, NewValue);
}

/**
* @brief Sets the given damage bonus as gear bonus with the given value
 * @param DamageEnum The enum specifying the damage bonus type
 * @param NewValue The new value to set the bonus to
 * @return True on success, false otherwise
 */
bool UVitalityStatComponent::SetGearDamageBonusValue(EDamageType DamageEnum, int NewValue)
{
	return SetNewDamageBonusValue(&_GearStats, DamageEnum, NewValue);
}


/**
* @brief Sets the given damage bonus as magic bonus with the given value
 * @param DamageEnum The enum specifying the damage bonus type
 * @param NewValue The new value to set the bonus to
 * @return True on success, false otherwise
 */
bool UVitalityStatComponent::SetMagicalDamageBonusValue(EDamageType DamageEnum, int NewValue)
{
	return SetNewDamageBonusValue(&_ModifiedStats, DamageEnum, NewValue);
}


/**
* @brief Sets the given damage bonus as other-type bonus with the given value
 * @param DamageEnum The enum specifying the damage bonus type
 * @param NewValue The new value to set the bonus to
 * @return True on success, false otherwise
 */
bool UVitalityStatComponent::SetOtherDamageBonusValue(EDamageType DamageEnum, int NewValue)
{
	return SetNewDamageBonusValue(&_OtherStats, DamageEnum, NewValue);
}

/**
 * @brief Gets the natural damage bonus value for the requested enum
 * @param DamageEnum The damage enum to get the natural damage bonus for
 * @return Returns the damage bonus value, or zero if it does not exist
 */
float UVitalityStatComponent::GetNaturalDamageBonusValue(EDamageType DamageEnum) const
{
	return GetDamageBonusValue(&_BaseStats, DamageEnum);
}

/**
 * @brief Gets the gear damage bonus value for the requested enum
 * @param DamageEnum The damage enum to get the gear damage bonus for
 * @return Returns the damage bonus value, or zero if it does not exist
 */
float UVitalityStatComponent::GetGearDamageBonusValue(EDamageType DamageEnum) const
{
	return GetDamageBonusValue(&_GearStats, DamageEnum);
}


/**
 * @brief Gets the magic damage bonus value for the requested enum
 * @param DamageEnum The damage enum to get the magic damage bonus for
 * @return Returns the damage bonus value, or zero if it does not exist
 */
float UVitalityStatComponent::GetMagicalDamageBonusValue(EDamageType DamageEnum) const
{
	return GetDamageBonusValue(&_ModifiedStats, DamageEnum);
}


/**
 * @brief Gets the other-type damage bonus value for the requested enum
 * @param DamageEnum The damage enum to get the other-type damage bonus for
 * @return Returns the damage bonus value, or zero if it does not exist
 */
float UVitalityStatComponent::GetOtherDamageBonusValue(EDamageType DamageEnum) const
{
	return GetDamageBonusValue(&_OtherStats, DamageEnum);
}

/**
 * @brief Gets the total damage resistance across natural, gear, magical and other-type
 * @param DamageEnum The damage enum to get the total damage resistance for
 * @return Returns the total damage resistance value across all arrays
 */
float UVitalityStatComponent::GetTotalResistance(EDamageType DamageEnum) const
{
	return (
		  GetNaturalResistanceValue(DamageEnum) + GetMagicalResistanceValue(DamageEnum)
		+ GetGearResistanceValue(DamageEnum) + GetOtherResistanceValue(DamageEnum)
	);
}

/**
 * @brief Gets the total damage bonus across natural, gear, magical and other-type
 * @param DamageEnum The damage enum to get the total damage bonus for
 * @return Returns the total damage bonus value across all arrays
 */
float UVitalityStatComponent::GetTotalDamageBonus(EDamageType DamageEnum) const
{
	return (
		  GetNaturalDamageBonusValue(DamageEnum) + GetMagicalDamageBonusValue(DamageEnum)
		+ GetGearDamageBonusValue(DamageEnum) + GetOtherDamageBonusValue(DamageEnum)
	);
}

/**
 * @brief Sets the value of the natural core stat given
 * @param StatEnum The stat enum to be set
 * @param NewValue The new value to set the natural strength to
 */
bool UVitalityStatComponent::SetNaturalCoreStat(EVitalityStat StatEnum, float NewValue)
{
	return SetNewCoreStatsValue(&_BaseStats, StatEnum, NewValue);
}

/**
 * @brief Sets the value of the gear-based core stat given
 * @param StatEnum The stat enum to be set
 * @param NewValue The new value to set the gear-based strength to
 */
bool UVitalityStatComponent::SetGearCoreStat(EVitalityStat StatEnum, float NewValue)
{
	return SetNewCoreStatsValue(&_GearStats, StatEnum, NewValue);
}

/**
 * @brief Sets the value of the magic-based core stat given
 * @param StatEnum The stat enum to be set
 * @param NewValue The new value to set the magical strength to
 */
bool UVitalityStatComponent::SetMagicalCoreStat(EVitalityStat StatEnum, float NewValue)
{
	return SetNewCoreStatsValue(&_ModifiedStats, StatEnum, NewValue);
}

/**
 * @brief Sets the value of the other-type core stat given
 * @param StatEnum The stat enum to be set
 * @param NewValue The new value to set the other-type strength to
 */
bool UVitalityStatComponent::SetOtherCoreStat(EVitalityStat StatEnum, float NewValue)
{
	return SetNewCoreStatsValue(&_OtherStats, StatEnum, NewValue);
}

/**
 * @brief Returns the total of all stats (natural, gear, magic, other) with the given enum
 * @param StatEnum The vitality stat to get the value for
 * @return The total value, or zero, if the enum is not used.
 */
float UVitalityStatComponent::GetTotalCoreStat(EVitalityStat StatEnum) const
{
	return (
		GetCoreStatValue(&_BaseStats, StatEnum) + GetCoreStatValue(&_GearStats, StatEnum)
	  + GetCoreStatValue(&_ModifiedStats, StatEnum) + GetCoreStatValue(&_OtherStats, StatEnum)
	);
}

/**
 * @brief Returns the total of the given natural core stat enum
 * @param StatEnum The vitality stat to get the value for
 * @return The value of the natural core stat, or zero if not set or used.
 */
float UVitalityStatComponent::GetNaturalCoreStat(EVitalityStat StatEnum) const
{
	return GetCoreStatValue(&_BaseStats, StatEnum);
}

/**
 * @brief Returns the total of the given gear-based core stat enum
 * @param StatEnum The vitality stat to get the value for
 * @return The value of the gear-based core stat, or zero if not set or used.
 */
float UVitalityStatComponent::GetGearCoreStat(EVitalityStat StatEnum) const
{
	return GetCoreStatValue(&_GearStats, StatEnum);
}

/**
 * @brief Returns the total of the given magical core stat enum
 * @param StatEnum The vitality stat to get the value for
 * @return The value of the magical core stat, or zero if not set or used.
 */
float UVitalityStatComponent::GetMagicalCoreStat(EVitalityStat StatEnum) const
{
	return GetCoreStatValue(&_ModifiedStats, StatEnum);
}

/**
 * @brief Returns the total of the given other-type core stat enum
 * @param StatEnum The vitality stat to get the value for
 * @return The value of the other-type core stat, or zero if not set or used.
 */
float UVitalityStatComponent::GetOtherCoreStat(EVitalityStat StatEnum) const
{
	return GetCoreStatValue(&_OtherStats, StatEnum);
}

void UVitalityStatComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UVitalityStatComponent::OnComponentCreated()
{
	Super::OnComponentCreated();
	SetAutoActivate(true);
	RegisterComponent();
}

void UVitalityStatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UVitalityStatComponent, _BaseStats);
	DOREPLIFETIME(UVitalityStatComponent, _GearStats);
	DOREPLIFETIME(UVitalityStatComponent, _ModifiedStats);
	DOREPLIFETIME(UVitalityStatComponent, _OtherStats);
}

/**
 * @brief Finds an existing index in the damage map array that matches the damage enum given
 * @param ArrayReference THe array to be traversed
 * @param DamageEnum The damage enum for the resistance map being sought
 * @return A pointer to the actual damage map
 */
FStVitalityDamageMap* UVitalityStatComponent::FindDamageResistanceMap(
		const FStVitalityStats& ArrayReference, const EDamageType DamageEnum) const
{
	// Iterate the actual resistance map
	for (FStVitalityDamageMap* DamageMap : &ArrayReference.DamageResistances)
	{
		// Return a pointer to the actual resistance map
		if (DamageMap->DamageType == DamageEnum)
			return DamageMap;
	}
	return nullptr;
}

/**
 * @brief Finds an existing index in the damage map array that matches the damage enum given
 * @param ArrayReference THe array to be traversed
 * @param DamageEnum The damage enum for the bonus map being sought
 * @return A pointer to the actual damage map
 */
FStVitalityDamageMap* UVitalityStatComponent::FindDamageBonusMap(
		const FStVitalityStats& ArrayReference, const EDamageType DamageEnum) const
{
	// Iterate the actual bonus map
	for (FStVitalityDamageMap* DamageMap : &ArrayReference.DamageBonuses)
	{
		// Return a pointer to the actual resistance map
		if (DamageMap->DamageType == DamageEnum)
			return DamageMap;
	}
	return nullptr;
}

/**
 * @brief Finds an existing index in the core stat array that matches the stat enum given
 * @param ArrayReference THe array to be traversed
 * @param StatEnum The core stat being sought
 * @return A pointer to the actual stat map
 */
FStVitalityStatMap* UVitalityStatComponent::FindCoreStatsMap(
	const FStVitalityStats& ArrayReference, const EVitalityStat StatEnum) const
{
	// Iterate the actual core stat map
	for (FStVitalityStatMap* CoreStat : &ArrayReference.CoreStats)
	{
		// Return a pointer to the core stat map
		if (CoreStat->StatEnum == StatEnum)
			return CoreStat;
	}
	return nullptr;
}

/**
 * @brief Sets the damage resistance enum to the given value for the specified map
 * @param StatsMap The stats map to be used (natural, gear, magical, other)
 * @param DamageEnum The damage resistance enum to set or modify
 * @param NewValue The new value for the given enum of the given map
 * @return True on success, false otherwise
 */
bool UVitalityStatComponent::SetNewDamageResistanceValue(
	FStVitalityStats* StatsMap, const EDamageType DamageEnum, const int NewValue)
{
	if (StatsMap != nullptr)
	{
		const float OldValue = GetDamageResistanceValue(StatsMap, DamageEnum);
		FStVitalityDamageMap* DamageMap = FindDamageResistanceMap(*StatsMap, DamageEnum);
		if (DamageMap != nullptr)
		{
			DamageMap->MapValue = NewValue;
			StatsMap->OnDamageResistanceUpdated.Broadcast(DamageEnum, OldValue, NewValue);
			return true;
		}
		StatsMap->DamageResistances.Add(FStVitalityDamageMap(DamageEnum, NewValue));
		StatsMap->OnDamageResistanceUpdated.Broadcast(DamageEnum, OldValue, NewValue);
		return true;
	}
	UE_LOG(LogTemp, Error, TEXT("SetNewDamageResistanceValue(): StatsMap passed as INVALID PTR"));
	return false;
}

/**
 * @brief Sets the damage bonus enum to the given value for the specified map
 * @param StatsMap The stats map to be used (natural, gear, magical, other)
 * @param DamageEnum The damage bonus enum to set or modify
 * @param NewValue The new value for the given enum of the given map
 * @return True on success, false otherwise
 */
bool UVitalityStatComponent::SetNewDamageBonusValue(
	FStVitalityStats* StatsMap, const EDamageType DamageEnum, const int NewValue)
{
	if (StatsMap != nullptr)
	{
		const float OldValue = GetDamageBonusValue(StatsMap, DamageEnum);
		FStVitalityDamageMap* DamageMap = FindDamageBonusMap(*StatsMap, DamageEnum);
		if (DamageMap != nullptr)
		{
			DamageMap->MapValue = NewValue;
			StatsMap->OnDamageBonusUpdated.Broadcast(DamageEnum, OldValue, NewValue);
			return true;
		}
		StatsMap->DamageBonuses.Add(FStVitalityDamageMap(DamageEnum, NewValue));
		StatsMap->OnDamageBonusUpdated.Broadcast(DamageEnum, OldValue, NewValue);
		return true;
	}
	UE_LOG(LogTemp, Error, TEXT("SetNewDamageBonusValue(): StatsMap passed as INVALID PTR"));
	return false;
}


/**
 * @brief Sets the core stat to the given value for the specified map
 * @param StatsMap The stats map to be used (natural, gear, magical, other)
 * @param StatEnum The core stat enum to set or modify
 * @param NewValue The new value for the given enum of the given map
 * @return True on success, false otherwise
 */
bool UVitalityStatComponent::SetNewCoreStatsValue(FStVitalityStats* StatsMap,
	const EVitalityStat StatEnum, const int NewValue)
{
	if (StatsMap != nullptr)
	{
		const float OldValue = GetCoreStatValue(StatsMap, StatEnum);
		FStVitalityStatMap* DamageMap = FindCoreStatsMap(*StatsMap, StatEnum);
		if (DamageMap != nullptr)
		{
			DamageMap->MapValue = NewValue;
			StatsMap->OnCoreStatUpdated.Broadcast(StatEnum, OldValue, NewValue);
			return true;
		}
		StatsMap->CoreStats.Add(FStVitalityStatMap(StatEnum, NewValue));
		StatsMap->OnCoreStatUpdated.Broadcast(StatEnum, OldValue, NewValue);
		return true;
	}
	UE_LOG(LogTemp, Error, TEXT("SetNewCoreStatsValue(): StatsMap passed as INVALID PTR"));
	return false;
}

/**
 * @brief Gets the damage resistance value of the referenced stats map
 * @param StatsMap The stats map to be traversed
 * @param DamageEnum The damage enum to look for
 * @return The value of the enum. Returns zero if unused or unset.
 */
float UVitalityStatComponent::GetDamageResistanceValue(
	const FStVitalityStats* StatsMap, const EDamageType DamageEnum) const
{
	const FStVitalityDamageMap* DamageMap = FindDamageResistanceMap(*StatsMap, DamageEnum);
	if (DamageMap != nullptr)
		return DamageMap->MapValue;
	return 0.f;
}

/**
 * @brief Gets the damage bonus value of the referenced stats map
 * @param StatsMap The stats map to be traversed
 * @param DamageEnum The damage bonus enum to look for
 * @return The value of the enum. Returns zero if unused or unset.
 */
float UVitalityStatComponent::GetDamageBonusValue(
	const FStVitalityStats* StatsMap, const EDamageType DamageEnum) const
{
	const FStVitalityDamageMap* DamageMap = FindDamageBonusMap(*StatsMap, DamageEnum);
	if (DamageMap != nullptr)
		return DamageMap->MapValue;
	return 0.f;
}

/**
 * @brief Gets the core stat value of the referenced stats map
 * @param StatsMap The stats map to be traversed
 * @param StatEnum The core stat enum to look for
 * @return The value of the enum. Returns zero if unused or unset.
 */
float UVitalityStatComponent::GetCoreStatValue(
	const FStVitalityStats* StatsMap, const EVitalityStat StatEnum) const
{
	const FStVitalityStatMap* DamageMap = FindCoreStatsMap(*StatsMap, StatEnum);
	if (DamageMap != nullptr)
		return DamageMap->MapValue;
	return 0.f;
}
