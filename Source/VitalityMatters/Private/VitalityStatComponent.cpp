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
 * @brief Sets the natural resistance value of the actor.
 *			If it does not exist, it will be added.
 * @param DamageEnum The enum specifying the damage type
 * @param NewValue The new actual value of the damage resistance
 * @return True if set was successful. False otherwise.
 */
bool UVitalityStatComponent::SetNaturalResistanceValue(EDamageType DamageEnum, int NewValue)
{
	return SetNewDamageResistanceValue(&_BaseStats, DamageEnum, NewValue);
}

bool UVitalityStatComponent::SetGearResistanceValue(EDamageType DamageEnum, int NewValue)
{
	return SetNewDamageResistanceValue(&_GearStats, DamageEnum, NewValue);
}

bool UVitalityStatComponent::SetMagicalResistanceValue(EDamageType DamageEnum, int NewValue)
{
	return SetNewDamageResistanceValue(&_ModifiedStats, DamageEnum, NewValue);
}

bool UVitalityStatComponent::SetOtherResistanceValue(EDamageType DamageEnum, int NewValue)
{
	return SetNewDamageResistanceValue(&_OtherStats, DamageEnum, NewValue);
}

float UVitalityStatComponent::GetNaturalResistanceValue(EDamageType DamageEnum) const
{
	return GetDamageResistanceValue(&_BaseStats, DamageEnum);
}

float UVitalityStatComponent::GetGearResistanceValue(EDamageType DamageEnum) const
{
	return GetDamageResistanceValue(&_GearStats, DamageEnum);
}

float UVitalityStatComponent::GetMagicalResistanceValue(EDamageType DamageEnum) const
{
	return GetDamageResistanceValue(&_ModifiedStats, DamageEnum);
}

float UVitalityStatComponent::GetOtherResistanceValue(EDamageType DamageEnum) const
{
	return GetDamageResistanceValue(&_OtherStats, DamageEnum);
}

bool UVitalityStatComponent::SetNaturalDamageBonusValue(EDamageType DamageEnum, int NewValue)
{
	return SetNewDamageBonusValue(&_BaseStats, DamageEnum, NewValue);
}

bool UVitalityStatComponent::SetGearDamageBonusValue(EDamageType DamageEnum, int NewValue)
{
	return SetNewDamageBonusValue(&_GearStats, DamageEnum, NewValue);
}

bool UVitalityStatComponent::SetMagicalDamageBonusValue(EDamageType DamageEnum, int NewValue)
{
	return SetNewDamageBonusValue(&_ModifiedStats, DamageEnum, NewValue);
}

bool UVitalityStatComponent::SetOtherDamageBonusValue(EDamageType DamageEnum, int NewValue)
{
	return SetNewDamageBonusValue(&_OtherStats, DamageEnum, NewValue);
}

float UVitalityStatComponent::GetNaturalDamageBonusValue(EDamageType DamageEnum) const
{
	return GetDamageBonusValue(&_BaseStats, DamageEnum);
}

float UVitalityStatComponent::GetGearDamageBonusValue(EDamageType DamageEnum) const
{
	return GetDamageBonusValue(&_GearStats, DamageEnum);
}

float UVitalityStatComponent::GetMagicalDamageBonusValue(EDamageType DamageEnum) const
{
	return GetDamageBonusValue(&_ModifiedStats, DamageEnum);
}

float UVitalityStatComponent::GetOtherDamageBonusValue(EDamageType DamageEnum) const
{
	return GetDamageBonusValue(&_OtherStats, DamageEnum);
}

float UVitalityStatComponent::GetTotalResistance(EDamageType DamageEnum) const
{
	return (
		  GetNaturalResistanceValue(DamageEnum) + GetMagicalResistanceValue(DamageEnum)
		+ GetGearResistanceValue(DamageEnum) + GetOtherResistanceValue(DamageEnum)
	);
}

float UVitalityStatComponent::GetTotalDamageBonus(EDamageType DamageEnum) const
{
	return (
		  GetNaturalDamageBonusValue(DamageEnum) + GetMagicalDamageBonusValue(DamageEnum)
		+ GetGearDamageBonusValue(DamageEnum) + GetOtherDamageBonusValue(DamageEnum)
	);
}

void UVitalityStatComponent::SetStrength(float NewValue)
{
	SetNewCoreStatsValue(&_BaseStats, EVitalityStat::STRENGTH, NewValue);
}

float UVitalityStatComponent::IncreaseStrength(float ValueAffected)
{
	return SetNewCoreStatsValue(&_BaseStats, EVitalityStat::STRENGTH,
		abs(ValueAffected) + GetCoreStatValue(&_BaseStats, EVitalityStat::STRENGTH));
}

float UVitalityStatComponent::DecreaseStrength(float ValueAffected)
{
	return SetNewCoreStatsValue(&_BaseStats, EVitalityStat::STRENGTH,
		abs(ValueAffected) - GetCoreStatValue(&_BaseStats, EVitalityStat::STRENGTH));
}

float UVitalityStatComponent::GetStrength() const
{
	return (
		GetCoreStatValue(&_BaseStats,		EVitalityStat::STRENGTH)
	  + GetCoreStatValue(&_GearStats,		EVitalityStat::STRENGTH)
	  + GetCoreStatValue(&_ModifiedStats,	EVitalityStat::STRENGTH)
	  + GetCoreStatValue(&_OtherStats,		EVitalityStat::STRENGTH)
	);
}


void UVitalityStatComponent::SetAgility(float NewValue)
{
	SetNewCoreStatsValue(&_BaseStats, EVitalityStat::AGILITY, NewValue);
}

float UVitalityStatComponent::IncreaseAgility(float ValueAffected)
{
	return SetNewCoreStatsValue(&_BaseStats, EVitalityStat::AGILITY,
		abs(ValueAffected) + GetCoreStatValue(&_BaseStats, EVitalityStat::AGILITY));
}

float UVitalityStatComponent::DecreaseAgility(float ValueAffected)
{
	return SetNewCoreStatsValue(&_BaseStats, EVitalityStat::AGILITY,
		abs(ValueAffected) - GetCoreStatValue(&_BaseStats, EVitalityStat::AGILITY));
}

float UVitalityStatComponent::GetAgility() const
{
	return (
		GetCoreStatValue(&_BaseStats,		EVitalityStat::AGILITY)
	  + GetCoreStatValue(&_GearStats,		EVitalityStat::AGILITY)
	  + GetCoreStatValue(&_ModifiedStats,	EVitalityStat::AGILITY)
	  + GetCoreStatValue(&_OtherStats,		EVitalityStat::AGILITY)
	);
}


bool UVitalityStatComponent::SetFortitude(float NewValue)
{
	return SetNewCoreStatsValue(&_BaseStats, EVitalityStat::AGILITY, NewValue);
}

float UVitalityStatComponent::IncreaseFortitude(float ValueAffected)
{
	return SetNewCoreStatsValue(&_BaseStats, EVitalityStat::AGILITY,
		abs(ValueAffected) + GetCoreStatValue(&_BaseStats, EVitalityStat::AGILITY));
}

float UVitalityStatComponent::DecreaseFortitude(float ValueAffected)
{
	return SetNewCoreStatsValue(&_BaseStats, EVitalityStat::AGILITY,
		abs(ValueAffected) - GetCoreStatValue(&_BaseStats, EVitalityStat::AGILITY));
}

float UVitalityStatComponent::GetFortitude() const
{
	return (
		GetCoreStatValue(&_BaseStats,		EVitalityStat::FORTITUDE)
	  + GetCoreStatValue(&_GearStats,		EVitalityStat::FORTITUDE)
	  + GetCoreStatValue(&_ModifiedStats,	EVitalityStat::FORTITUDE)
	  + GetCoreStatValue(&_OtherStats,		EVitalityStat::FORTITUDE)
	);
}


bool UVitalityStatComponent::SetIntellect(float NewValue)
{
	
}

float UVitalityStatComponent::IncreaseIntellect(float ValueAffected)
{
	
}

float UVitalityStatComponent::DecreaseIntellect(float ValueAffected)
{
	
}

float UVitalityStatComponent::GetIntellect() const
{
	return (
		GetCoreStatValue(&_BaseStats,		EVitalityStat::INTELLECT)
	  + GetCoreStatValue(&_GearStats,		EVitalityStat::INTELLECT)
	  + GetCoreStatValue(&_ModifiedStats,	EVitalityStat::INTELLECT)
	  + GetCoreStatValue(&_OtherStats,		EVitalityStat::INTELLECT)
	);
}


bool UVitalityStatComponent::SetAstuteness(float NewValue)
{
	const float OldValue = _Astuteness;
	_Astuteness = NewValue;
	OnStatModified.Broadcast(EVitalityStat::ASTUTENESS, OldValue, _Astuteness);
}

float UVitalityStatComponent::IncreaseAstuteness(float ValueAffected)
{
	const float OldValue = _Astuteness;
	_Astuteness += abs(ValueAffected);
	OnStatModified.Broadcast(EVitalityStat::ASTUTENESS, OldValue, _Astuteness);
	return _Astuteness;
}

float UVitalityStatComponent::DecreaseAstuteness(float ValueAffected)
{
	const float OldValue = _Astuteness;
	_Astuteness -= abs(ValueAffected);
	OnStatModified.Broadcast(EVitalityStat::ASTUTENESS, OldValue, _Astuteness);
	return _Astuteness;
}

float UVitalityStatComponent::GetAstuteness() const
{
	return (
		GetCoreStatValue(&_BaseStats,		EVitalityStat::ASTUTENESS)
	  + GetCoreStatValue(&_GearStats,		EVitalityStat::ASTUTENESS)
	  + GetCoreStatValue(&_ModifiedStats,	EVitalityStat::ASTUTENESS)
	  + GetCoreStatValue(&_OtherStats,		EVitalityStat::ASTUTENESS)
	);
}


bool UVitalityStatComponent::SetCharisma(float NewValue)
{
}

float UVitalityStatComponent::IncreaseCharisma(float ValueAffected)
{
}

float UVitalityStatComponent::DecreaseCharisma(float ValueAffected)
{
}

float UVitalityStatComponent::GetCharisma() const
{
	return (
	    GetCoreStatValue(&_BaseStats,		EVitalityStat::CHARISMA)
	  + GetCoreStatValue(&_GearStats,		EVitalityStat::CHARISMA)
	  + GetCoreStatValue(&_ModifiedStats,	EVitalityStat::CHARISMA)
	  + GetCoreStatValue(&_OtherStats,		EVitalityStat::CHARISMA)
	);
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

bool UVitalityStatComponent::SetNewCoreStatsValue(FStVitalityStats* StatsMap, const EVitalityStat StatEnum,
	const int NewValue)
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

float UVitalityStatComponent::GetDamageResistanceValue(
	const FStVitalityStats* StatsMap, const EDamageType DamageEnum) const
{
	const FStVitalityDamageMap* DamageMap = FindDamageResistanceMap(*StatsMap, DamageEnum);
	if (DamageMap != nullptr)
		return DamageMap->MapValue;
	return 0.f;
}

float UVitalityStatComponent::GetDamageBonusValue(
	const FStVitalityStats* StatsMap, const EDamageType DamageEnum) const
{
	const FStVitalityDamageMap* DamageMap = FindDamageBonusMap(*StatsMap, DamageEnum);
	if (DamageMap != nullptr)
		return DamageMap->MapValue;
	return 0.f;
}

float UVitalityStatComponent::GetCoreStatValue(const FStVitalityStats* StatsMap, const EVitalityStat StatEnum) const
{
	const FStVitalityStatMap* DamageMap = FindCoreStatsMap(*StatsMap, StatEnum);
	if (DamageMap != nullptr)
		return DamageMap->MapValue;
	return 0.f;
}
