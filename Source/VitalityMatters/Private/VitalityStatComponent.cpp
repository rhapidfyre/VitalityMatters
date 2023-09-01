// Copyright Take Five Games, LLC 2023 - All Rights Reserved


#include "VitalityStatComponent.h"

#include "Net/UnrealNetwork.h"


UVitalityStatComponent::UVitalityStatComponent()
{
	SetIsReplicatedByDefault(true);
}

/**
 * @brief Sets the given damage resistance as natural resistance enum to the given value
 * @param DamageEnum The enum specifying the damage type
 * @param NewValue The new actual value of the damage resistance
 * @return True if set was successful. False otherwise.
 */
bool UVitalityStatComponent::SetNaturalResistanceValue(EDamageType DamageEnum, int NewValue)
{
	return SetNewDamageResistanceValue(_BaseStats, DamageEnum, NewValue);
}

/**
 * @brief Sets the given damage resistance as gear resistance enum to the given value
 * @param DamageEnum The enum specifying the damage type
 * @param NewValue The new value to set the gear based resistance to
 * @return True on success, false otherwise
 */
bool UVitalityStatComponent::SetGearResistanceValue(EDamageType DamageEnum, int NewValue)
{
	return SetNewDamageResistanceValue(_GearStats, DamageEnum, NewValue);
}

/**
* @brief Sets the given damage resistance as magical resistance with the given value
 * @param DamageEnum The enum specifying the damage type
 * @param NewValue The new value to set the magic-based resistance to
 * @return True on success, false otherwise
 */
bool UVitalityStatComponent::SetMagicalResistanceValue(EDamageType DamageEnum, int NewValue)
{
	return SetNewDamageResistanceValue(_ModifiedStats, DamageEnum, NewValue);
}

/**
* @brief Sets the given damage resistance as other-type resistance with the given value
 * @param DamageEnum The enum specifying the damage type
 * @param NewValue The new value to set the other resistance to
 * @return True on success, false otherwise
 */
bool UVitalityStatComponent::SetOtherResistanceValue(EDamageType DamageEnum, int NewValue)
{
	return SetNewDamageResistanceValue(_OtherStats, DamageEnum, NewValue);
}

/**
 * @brief 
 * @param DamageEnum The damage enum to get the natural resistance for
 * @return Returns the resistance value, or zero if it does not exist
 */
float UVitalityStatComponent::GetNaturalResistanceValue(EDamageType DamageEnum)
{
	return GetDamageResistanceValue(_BaseStats, DamageEnum);
}

/**
 * @brief 
 * @param DamageEnum The damage enum to get the gear resistance for
 * @return Returns the resistance value, or zero if it does not exist
 */
float UVitalityStatComponent::GetGearResistanceValue(EDamageType DamageEnum)
{
	return GetDamageResistanceValue(_GearStats, DamageEnum);
}

/**
 * @brief 
 * @param DamageEnum The damage enum to get the magical resistance for
 * @return Returns the resistance value, or zero if it does not exist
 */
float UVitalityStatComponent::GetMagicalResistanceValue(EDamageType DamageEnum)
{
	return GetDamageResistanceValue(_ModifiedStats, DamageEnum);
}

/**
 * @brief 
 * @param DamageEnum The damage enum to get the other-type resistance for
 * @return Returns the resistance value, or zero if it does not exist
 */
float UVitalityStatComponent::GetOtherResistanceValue(EDamageType DamageEnum)
{
	return GetDamageResistanceValue(_OtherStats, DamageEnum);
}

/**
* @brief Sets the given damage bonus as natural bonus with the given value
 * @param DamageEnum The enum specifying the damage bonus type
 * @param NewValue The new value to set the bonus to
 * @return True on success, false otherwise
 */
bool UVitalityStatComponent::SetNaturalDamageBonusValue(EDamageType DamageEnum, int NewValue)
{
	return SetNewDamageBonusValue(_BaseStats, DamageEnum, NewValue);
}

/**
* @brief Sets the given damage bonus as gear bonus with the given value
 * @param DamageEnum The enum specifying the damage bonus type
 * @param NewValue The new value to set the bonus to
 * @return True on success, false otherwise
 */
bool UVitalityStatComponent::SetGearDamageBonusValue(EDamageType DamageEnum, int NewValue)
{
	return SetNewDamageBonusValue(_GearStats, DamageEnum, NewValue);
}


/**
* @brief Sets the given damage bonus as magic bonus with the given value
 * @param DamageEnum The enum specifying the damage bonus type
 * @param NewValue The new value to set the bonus to
 * @return True on success, false otherwise
 */
bool UVitalityStatComponent::SetMagicalDamageBonusValue(EDamageType DamageEnum, int NewValue)
{
	return SetNewDamageBonusValue(_ModifiedStats, DamageEnum, NewValue);
}


/**
* @brief Sets the given damage bonus as other-type bonus with the given value
 * @param DamageEnum The enum specifying the damage bonus type
 * @param NewValue The new value to set the bonus to
 * @return True on success, false otherwise
 */
bool UVitalityStatComponent::SetOtherDamageBonusValue(EDamageType DamageEnum, int NewValue)
{
	return SetNewDamageBonusValue(_OtherStats, DamageEnum, NewValue);
}

/**
 * @brief Gets the natural damage bonus value for the requested enum
 * @param DamageEnum The damage enum to get the natural damage bonus for
 * @return Returns the damage bonus value, or zero if it does not exist
 */
float UVitalityStatComponent::GetNaturalDamageBonusValue(EDamageType DamageEnum)
{
	return GetDamageBonusValue(_BaseStats, DamageEnum);
}

/**
 * @brief Gets the gear damage bonus value for the requested enum
 * @param DamageEnum The damage enum to get the gear damage bonus for
 * @return Returns the damage bonus value, or zero if it does not exist
 */
float UVitalityStatComponent::GetGearDamageBonusValue(EDamageType DamageEnum)
{
	return GetDamageBonusValue(_GearStats, DamageEnum);
}


/**
 * @brief Gets the magic damage bonus value for the requested enum
 * @param DamageEnum The damage enum to get the magic damage bonus for
 * @return Returns the damage bonus value, or zero if it does not exist
 */
float UVitalityStatComponent::GetMagicalDamageBonusValue(EDamageType DamageEnum)
{
	return GetDamageBonusValue(_ModifiedStats, DamageEnum);
}


/**
 * @brief Gets the other-type damage bonus value for the requested enum
 * @param DamageEnum The damage enum to get the other-type damage bonus for
 * @return Returns the damage bonus value, or zero if it does not exist
 */
float UVitalityStatComponent::GetOtherDamageBonusValue(EDamageType DamageEnum)
{
	return GetDamageBonusValue(_OtherStats, DamageEnum);
}

/**
 * @brief Gets the total damage resistance across natural, gear, magical and other-type
 * @param DamageEnum The damage enum to get the total damage resistance for
 * @return Returns the total damage resistance value across all arrays
 */
float UVitalityStatComponent::GetTotalResistance(EDamageType DamageEnum)
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
float UVitalityStatComponent::GetTotalDamageBonus(EDamageType DamageEnum)
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
	return SetNewCoreStatsValue(_BaseStats, StatEnum, NewValue);
}

/**
 * @brief Sets the value of the gear-based core stat given
 * @param StatEnum The stat enum to be set
 * @param NewValue The new value to set the gear-based strength to
 */
bool UVitalityStatComponent::SetGearCoreStat(EVitalityStat StatEnum, float NewValue)
{
	return SetNewCoreStatsValue(_GearStats, StatEnum, NewValue);
}

/**
 * @brief Sets the value of the magic-based core stat given
 * @param StatEnum The stat enum to be set
 * @param NewValue The new value to set the magical strength to
 */
bool UVitalityStatComponent::SetMagicalCoreStat(EVitalityStat StatEnum, float NewValue)
{
	return SetNewCoreStatsValue(_ModifiedStats, StatEnum, NewValue);
}

/**
 * @brief Sets the value of the other-type core stat given
 * @param StatEnum The stat enum to be set
 * @param NewValue The new value to set the other-type strength to
 */
bool UVitalityStatComponent::SetOtherCoreStat(EVitalityStat StatEnum, float NewValue)
{
	return SetNewCoreStatsValue(_OtherStats, StatEnum, NewValue);
}

/**
 * @brief Returns the total of all stats (natural, gear, magic, other) with the given enum
 * @param StatEnum The vitality stat to get the value for
 * @return The total value, or zero, if the enum is not used.
 */
float UVitalityStatComponent::GetTotalCoreStat(EVitalityStat StatEnum)
{
	return (
		GetCoreStatValue(_BaseStats, StatEnum) + GetCoreStatValue(_GearStats, StatEnum)
	  + GetCoreStatValue(_ModifiedStats, StatEnum) + GetCoreStatValue(_OtherStats, StatEnum)
	);
}

/**
 * @brief Returns the total of the given natural core stat enum
 * @param StatEnum The vitality stat to get the value for
 * @return The value of the natural core stat, or zero if not set or used.
 */
float UVitalityStatComponent::GetNaturalCoreStat(EVitalityStat StatEnum)
{
	return GetCoreStatValue(_BaseStats, StatEnum);
}

/**
 * @brief Returns the total of the given gear-based core stat enum
 * @param StatEnum The vitality stat to get the value for
 * @return The value of the gear-based core stat, or zero if not set or used.
 */
float UVitalityStatComponent::GetGearCoreStat(EVitalityStat StatEnum)
{
	return GetCoreStatValue(_GearStats, StatEnum);
}

/**
 * @brief Returns the total of the given magical core stat enum
 * @param StatEnum The vitality stat to get the value for
 * @return The value of the magical core stat, or zero if not set or used.
 */
float UVitalityStatComponent::GetMagicalCoreStat(EVitalityStat StatEnum)
{
	return GetCoreStatValue(_ModifiedStats, StatEnum);
}

/**
 * @brief Returns the total of the given other-type core stat enum
 * @param StatEnum The vitality stat to get the value for
 * @return The value of the other-type core stat, or zero if not set or used.
 */
float UVitalityStatComponent::GetOtherCoreStat(EVitalityStat StatEnum)
{
	return GetCoreStatValue(_OtherStats, StatEnum);
}

void UVitalityStatComponent::BindListenerEvents()
{
	if (!_BaseStats.OnCoreStatUpdated.IsAlreadyBound(this, &UVitalityStatComponent::NaturalCoreStatUpdated))
		 _BaseStats.OnCoreStatUpdated.AddDynamic(this, &UVitalityStatComponent::NaturalCoreStatUpdated);
	if (!_BaseStats.OnDamageBonusUpdated.IsAlreadyBound(this, &UVitalityStatComponent::NaturalDamageBonusUpdated))
		 _BaseStats.OnDamageBonusUpdated.AddDynamic(this, &UVitalityStatComponent::NaturalDamageBonusUpdated);
	if (!_BaseStats.OnDamageResistanceUpdated.IsAlreadyBound(this, &UVitalityStatComponent::NaturalDamageResistUpdated))
		 _BaseStats.OnDamageResistanceUpdated.AddDynamic(this, &UVitalityStatComponent::NaturalDamageResistUpdated);
	
	if (!_GearStats.OnCoreStatUpdated.IsAlreadyBound(this, &UVitalityStatComponent::GearCoreStatUpdated))
		 _GearStats.OnCoreStatUpdated.AddDynamic(this, &UVitalityStatComponent::GearCoreStatUpdated);
	if (!_GearStats.OnDamageBonusUpdated.IsAlreadyBound(this, &UVitalityStatComponent::GearDamageBonusUpdated))
		 _GearStats.OnDamageBonusUpdated.AddDynamic(this, &UVitalityStatComponent::GearDamageBonusUpdated);
	if (!_GearStats.OnDamageResistanceUpdated.IsAlreadyBound(this, &UVitalityStatComponent::GearDamageResistUpdated))
		 _GearStats.OnDamageResistanceUpdated.AddDynamic(this, &UVitalityStatComponent::GearDamageResistUpdated);
	
	if (!_ModifiedStats.OnCoreStatUpdated.IsAlreadyBound(this, &UVitalityStatComponent::MagicCoreStatUpdated))
		 _ModifiedStats.OnCoreStatUpdated.AddDynamic(this, &UVitalityStatComponent::MagicCoreStatUpdated);
	if (!_ModifiedStats.OnDamageBonusUpdated.IsAlreadyBound(this, &UVitalityStatComponent::MagicDamageBonusUpdated))
		 _ModifiedStats.OnDamageBonusUpdated.AddDynamic(this, &UVitalityStatComponent::MagicDamageBonusUpdated);
	if (!_ModifiedStats.OnDamageResistanceUpdated.IsAlreadyBound(this, &UVitalityStatComponent::MagicDamageResistUpdated))
		 _ModifiedStats.OnDamageResistanceUpdated.AddDynamic(this, &UVitalityStatComponent::MagicDamageResistUpdated);
	
	if (!_OtherStats.OnCoreStatUpdated.IsAlreadyBound(this, &UVitalityStatComponent::OtherCoreStatUpdated))
		 _OtherStats.OnCoreStatUpdated.AddDynamic(this, &UVitalityStatComponent::OtherCoreStatUpdated);
	if (!_OtherStats.OnDamageBonusUpdated.IsAlreadyBound(this, &UVitalityStatComponent::OtherDamageBonusUpdated))
		 _OtherStats.OnDamageBonusUpdated.AddDynamic(this, &UVitalityStatComponent::OtherDamageBonusUpdated);
	if (!_OtherStats.OnDamageResistanceUpdated.IsAlreadyBound(this, &UVitalityStatComponent::OtherDamageResistUpdated))
		 _OtherStats.OnDamageResistanceUpdated.AddDynamic(this, &UVitalityStatComponent::OtherDamageResistUpdated);

}

void UVitalityStatComponent::ReloadSettings()
{
	BindListenerEvents();
}

void UVitalityStatComponent::BeginPlay()
{
	Super::BeginPlay();
	ReloadSettings();
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
		FStVitalityStats& ArrayReference, const EDamageType DamageEnum)
{
	// Iterate the actual resistance map
	for (FStVitalityDamageMap& DamageMap : ArrayReference.DamageResistances)
	{
		// Return a pointer to the actual resistance map
		if (DamageMap.DamageType == DamageEnum)
			return &DamageMap;
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
		FStVitalityStats& ArrayReference, const EDamageType DamageEnum)
{
	// Iterate the actual bonus map
	for (FStVitalityDamageMap& DamageMap : ArrayReference.DamageBonuses)
	{
		// Return a pointer to the actual resistance map
		if (DamageMap.DamageType == DamageEnum)
			return &DamageMap;
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
	FStVitalityStats& ArrayReference, const EVitalityStat StatEnum)
{
	// Iterate the actual core stat map
	for (FStVitalityStatMap& CoreStat : ArrayReference.CoreStats)
	{
		// Return a pointer to the core stat map
		if (CoreStat.StatEnum == StatEnum)
			return &CoreStat;
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
	FStVitalityStats& StatsMap, const EDamageType DamageEnum, const int NewValue)
{
	StatsMap.SetDamageResistance(DamageEnum, NewValue);
	return true;
}

/**
 * @brief Sets the damage bonus enum to the given value for the specified map
 * @param StatsMap The stats map to be used (natural, gear, magical, other)
 * @param DamageEnum The damage bonus enum to set or modify
 * @param NewValue The new value for the given enum of the given map
 * @return True on success, false otherwise
 */
bool UVitalityStatComponent::SetNewDamageBonusValue(
	FStVitalityStats& StatsMap, const EDamageType DamageEnum, const int NewValue)
{
	StatsMap.SetDamageBonus(DamageEnum, NewValue);
	return true;
}


/**
 * @brief Sets the core stat to the given value for the specified map
 * @param StatsMap The stats map to be used (natural, gear, magical, other)
 * @param StatEnum The core stat enum to set or modify
 * @param NewValue The new value for the given enum of the given map
 * @return True on success, false otherwise
 */
bool UVitalityStatComponent::SetNewCoreStatsValue(FStVitalityStats& StatsMap,
	const EVitalityStat StatEnum, const int NewValue)
{
	StatsMap.SetCoreStat(StatEnum, NewValue);
	return true;
}

/**
 * @brief Gets the damage resistance value of the referenced stats map
 * @param StatsMap The stats map to be traversed
 * @param DamageEnum The damage enum to look for
 * @return The value of the enum. Returns zero if unused or unset.
 */
float UVitalityStatComponent::GetDamageResistanceValue(
	const FStVitalityStats& StatsMap, const EDamageType DamageEnum) const
{
	return StatsMap.GetDamageResistValue(DamageEnum);
}

/**
 * @brief Gets the damage bonus value of the referenced stats map
 * @param StatsMap The stats map to be traversed
 * @param DamageEnum The damage bonus enum to look for
 * @return The value of the enum. Returns zero if unused or unset.
 */
float UVitalityStatComponent::GetDamageBonusValue(
	const FStVitalityStats& StatsMap, const EDamageType DamageEnum) const
{
	return StatsMap.GetDamageBonusValue(DamageEnum);
}

/**
 * @brief Gets the core stat value of the referenced stats map
 * @param StatsMap The stats map to be traversed
 * @param StatEnum The core stat enum to look for
 * @return The value of the enum. Returns zero if unused or unset.
 */
float UVitalityStatComponent::GetCoreStatValue(
	const FStVitalityStats& StatsMap, const EVitalityStat StatEnum) const
{
	return StatsMap.GetCoreStatValue(StatEnum);
}

void UVitalityStatComponent::NaturalCoreStatUpdated(const EVitalityStat CoreStat)
{
	OnCoreStatModified.Broadcast(CoreStat);
}

void UVitalityStatComponent::GearCoreStatUpdated(const EVitalityStat CoreStat)
{
	OnCoreStatModified.Broadcast(CoreStat);
}

void UVitalityStatComponent::MagicCoreStatUpdated(const EVitalityStat CoreStat)
{
	OnCoreStatModified.Broadcast(CoreStat);
}

void UVitalityStatComponent::OtherCoreStatUpdated(const EVitalityStat CoreStat)
{
	OnCoreStatModified.Broadcast(CoreStat);
}

void UVitalityStatComponent::NaturalDamageBonusUpdated(const EDamageType DamageEnum)
{
	OnDamageBonusUpdated.Broadcast(DamageEnum);
}

void UVitalityStatComponent::GearDamageBonusUpdated(const EDamageType DamageEnum)
{
	OnDamageBonusUpdated.Broadcast(DamageEnum);
}

void UVitalityStatComponent::MagicDamageBonusUpdated(const EDamageType DamageEnum)
{
	OnDamageBonusUpdated.Broadcast(DamageEnum);
}

void UVitalityStatComponent::OtherDamageBonusUpdated(const EDamageType DamageEnum)
{
	OnDamageBonusUpdated.Broadcast(DamageEnum);
}

void UVitalityStatComponent::NaturalDamageResistUpdated(const EDamageType DamageEnum)
{
	OnDamageResistUpdated.Broadcast(DamageEnum);
}

void UVitalityStatComponent::GearDamageResistUpdated(const EDamageType DamageEnum)
{
	OnDamageResistUpdated.Broadcast(DamageEnum);
}

void UVitalityStatComponent::MagicDamageResistUpdated(const EDamageType DamageEnum)
{
	OnDamageResistUpdated.Broadcast(DamageEnum);
}

void UVitalityStatComponent::OtherDamageResistUpdated(const EDamageType DamageEnum)
{
	OnDamageResistUpdated.Broadcast(DamageEnum);
}
