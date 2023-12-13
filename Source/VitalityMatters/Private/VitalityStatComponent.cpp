// Copyright Take Five Games, LLC 2023 - All Rights Reserved


#include "VitalityStatComponent.h"

#include "Kismet/GameplayStatics.h"
#include "lib/SaveStats.h"
#include "Logging/StructuredLog.h"
#include "Net/UnrealNetwork.h"


UVitalityStatComponent::UVitalityStatComponent()
{
	SetIsReplicatedByDefault(true);
}

bool UVitalityStatComponent::LoadStatsFromSave(
		FString& ResponseString, FString SaveSlotName, bool isAsync)
{
	/*
	ResponseString = "Failed to Save (Inventory Has Not Initialized)";
	if (bSavesOnServerOnly)
	{
		if (GetNetMode() == NM_Client)
		{
			ResponseString = "Saving Only Allowed on Authority";
			return false;
		}
	}
	
	if (bStatsSystemReady)
	{
		if (!UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
		{
			ResponseString = "No SaveSlotName Exists";
			return false;
		}
		StatsSaveName_ = SaveSlotName;

		if (isAsync)
		{
			FAsyncLoadGameFromSlotDelegate LoadDelegate;
			LoadDelegate.BindUObject(this, &UVitalityStatComponent::LoadDataDelegate);
			UGameplayStatics::AsyncLoadGameFromSlot(StatsSaveName_, StatsSaveUserIndex_, LoadDelegate);
			ResponseString = "Sent Async Save Request";
			return true;
		}

		const USaveGame* SaveData = UGameplayStatics::LoadGameFromSlot(
											StatsSaveName_, StatsSaveUserIndex_);
		
		if (IsValid(SaveData))
		{
			const USaveVitalityStat* SavedStats = Cast<USaveVitalityStat>( SaveData );
			if (IsValid(SavedStats))
			{
				SavedStats(InventorySave->InventorySlots_,
								 InventorySave->EquipmentSlots_);
				ResponseString = "Successful Synchronous Load";
				return true;	
			}
		}
	}
	*/
	return false;
}

void UVitalityStatComponent::Reinitialize()
{
	UE_LOGFMT(LogTemp, Display, "{cName}({Sv}): Reinitialize()", *GetName(), GetOwner()->HasAuthority()?"S":"C");
	// Update base stats to match the starting stats
	for (int i = 0; i < UVitalitySystem::GetNumberOfCoreStats(); i++)
	{
		BaseStats_.CoreStats[i] = StartingStats.CoreStats[i];
		OnCoreStatModified.Broadcast(UVitalitySystem::GetCoreStatFromInt(i));
	}
	
	for (int i = 0; i < UVitalitySystem::GetNumberOfDamageTypes(); i++)
	{
		BaseStats_.DamageBonuses[i] = StartingStats.DamageBonuses[i];
		BaseStats_.DamageResists[i] = StartingStats.DamageBonuses[i];
		OnDamageBonusUpdated.Broadcast(UVitalitySystem::GetDamageTypeFromInt(i));
		OnDamageResistUpdated.Broadcast(UVitalitySystem::GetDamageTypeFromInt(i));
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
	return SetNewDamageResistanceValue(BaseStats_, DamageEnum, NewValue);
}

/**
 * @brief Sets the given damage resistance as gear resistance enum to the given value
 * @param DamageEnum The enum specifying the damage type
 * @param NewValue The new value to set the gear based resistance to
 * @return True on success, false otherwise
 */
bool UVitalityStatComponent::SetGearResistanceValue(EDamageType DamageEnum, int NewValue)
{
	return SetNewDamageResistanceValue(GearStats_, DamageEnum, NewValue);
}

/**
* @brief Sets the given damage resistance as magical resistance with the given value
 * @param DamageEnum The enum specifying the damage type
 * @param NewValue The new value to set the magic-based resistance to
 * @return True on success, false otherwise
 */
bool UVitalityStatComponent::SetMagicalResistanceValue(EDamageType DamageEnum, int NewValue)
{
	return SetNewDamageResistanceValue(ModifiedStats_, DamageEnum, NewValue);
}

/**
* @brief Sets the given damage resistance as other-type resistance with the given value
 * @param DamageEnum The enum specifying the damage type
 * @param NewValue The new value to set the other resistance to
 * @return True on success, false otherwise
 */
bool UVitalityStatComponent::SetOtherResistanceValue(EDamageType DamageEnum, int NewValue)
{
	return SetNewDamageResistanceValue(OtherStats_, DamageEnum, NewValue);
}

/**
 * @brief 
 * @param DamageEnum The damage enum to get the natural resistance for
 * @return Returns the resistance value, or zero if it does not exist
 */
float UVitalityStatComponent::GetNaturalResistanceValue(EDamageType DamageEnum)
{
	return GetDamageResistanceValue(BaseStats_, DamageEnum);
}

/**
 * @brief 
 * @param DamageEnum The damage enum to get the gear resistance for
 * @return Returns the resistance value, or zero if it does not exist
 */
float UVitalityStatComponent::GetGearResistanceValue(EDamageType DamageEnum)
{
	return GetDamageResistanceValue(GearStats_, DamageEnum);
}

/**
 * @brief 
 * @param DamageEnum The damage enum to get the magical resistance for
 * @return Returns the resistance value, or zero if it does not exist
 */
float UVitalityStatComponent::GetMagicalResistanceValue(EDamageType DamageEnum)
{
	return GetDamageResistanceValue(ModifiedStats_, DamageEnum);
}

/**
 * @brief 
 * @param DamageEnum The damage enum to get the other-type resistance for
 * @return Returns the resistance value, or zero if it does not exist
 */
float UVitalityStatComponent::GetOtherResistanceValue(EDamageType DamageEnum)
{
	return GetDamageResistanceValue(OtherStats_, DamageEnum);
}

/**
* @brief Sets the given damage bonus as natural bonus with the given value
 * @param DamageEnum The enum specifying the damage bonus type
 * @param NewValue The new value to set the bonus to
 * @return True on success, false otherwise
 */
bool UVitalityStatComponent::SetNaturalDamageBonusValue(EDamageType DamageEnum, int NewValue)
{
	return SetNewDamageBonusValue(BaseStats_, DamageEnum, NewValue);
}

/**
* @brief Sets the given damage bonus as gear bonus with the given value
 * @param DamageEnum The enum specifying the damage bonus type
 * @param NewValue The new value to set the bonus to
 * @return True on success, false otherwise
 */
bool UVitalityStatComponent::SetGearDamageBonusValue(EDamageType DamageEnum, int NewValue)
{
	return SetNewDamageBonusValue(GearStats_, DamageEnum, NewValue);
}


/**
* @brief Sets the given damage bonus as magic bonus with the given value
 * @param DamageEnum The enum specifying the damage bonus type
 * @param NewValue The new value to set the bonus to
 * @return True on success, false otherwise
 */
bool UVitalityStatComponent::SetMagicalDamageBonusValue(EDamageType DamageEnum, int NewValue)
{
	return SetNewDamageBonusValue(ModifiedStats_, DamageEnum, NewValue);
}


/**
* @brief Sets the given damage bonus as other-type bonus with the given value
 * @param DamageEnum The enum specifying the damage bonus type
 * @param NewValue The new value to set the bonus to
 * @return True on success, false otherwise
 */
bool UVitalityStatComponent::SetOtherDamageBonusValue(EDamageType DamageEnum, int NewValue)
{
	return SetNewDamageBonusValue(OtherStats_, DamageEnum, NewValue);
}

/**
 * @brief Gets the natural damage bonus value for the requested enum
 * @param DamageEnum The damage enum to get the natural damage bonus for
 * @return Returns the damage bonus value, or zero if it does not exist
 */
float UVitalityStatComponent::GetNaturalDamageBonusValue(EDamageType DamageEnum)
{
	return GetDamageBonusValue(BaseStats_, DamageEnum);
}

/**
 * @brief Gets the gear damage bonus value for the requested enum
 * @param DamageEnum The damage enum to get the gear damage bonus for
 * @return Returns the damage bonus value, or zero if it does not exist
 */
float UVitalityStatComponent::GetGearDamageBonusValue(EDamageType DamageEnum)
{
	return GetDamageBonusValue(GearStats_, DamageEnum);
}


/**
 * @brief Gets the magic damage bonus value for the requested enum
 * @param DamageEnum The damage enum to get the magic damage bonus for
 * @return Returns the damage bonus value, or zero if it does not exist
 */
float UVitalityStatComponent::GetMagicalDamageBonusValue(EDamageType DamageEnum)
{
	return GetDamageBonusValue(ModifiedStats_, DamageEnum);
}


/**
 * @brief Gets the other-type damage bonus value for the requested enum
 * @param DamageEnum The damage enum to get the other-type damage bonus for
 * @return Returns the damage bonus value, or zero if it does not exist
 */
float UVitalityStatComponent::GetOtherDamageBonusValue(EDamageType DamageEnum)
{
	return GetDamageBonusValue(OtherStats_, DamageEnum);
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
	return SetNewCoreStatsValue(BaseStats_, StatEnum, NewValue);
}

/**
 * @brief Sets the value of the gear-based core stat given
 * @param StatEnum The stat enum to be set
 * @param NewValue The new value to set the gear-based strength to
 */
bool UVitalityStatComponent::SetGearCoreStat(EVitalityStat StatEnum, float NewValue)
{
	return SetNewCoreStatsValue(GearStats_, StatEnum, NewValue);
}

/**
 * @brief Sets the value of the magic-based core stat given
 * @param StatEnum The stat enum to be set
 * @param NewValue The new value to set the magical strength to
 */
bool UVitalityStatComponent::SetMagicalCoreStat(EVitalityStat StatEnum, float NewValue)
{
	return SetNewCoreStatsValue(ModifiedStats_, StatEnum, NewValue);
}

/**
 * @brief Sets the value of the other-type core stat given
 * @param StatEnum The stat enum to be set
 * @param NewValue The new value to set the other-type strength to
 */
bool UVitalityStatComponent::SetOtherCoreStat(EVitalityStat StatEnum, float NewValue)
{
	return SetNewCoreStatsValue(OtherStats_, StatEnum, NewValue);
}

/**
 * @brief Returns the total of all stats (natural, gear, magic, other) with the given enum
 * @param StatEnum The vitality stat to get the value for
 * @return The total value, or zero, if the enum is not used.
 */
float UVitalityStatComponent::GetTotalCoreStat(EVitalityStat StatEnum)
{
	return (
		GetCoreStatValue(BaseStats_, StatEnum) + GetCoreStatValue(GearStats_, StatEnum)
	  + GetCoreStatValue(ModifiedStats_, StatEnum) + GetCoreStatValue(OtherStats_, StatEnum)
	);
}

/**
 * @brief Returns the total of the given natural core stat enum
 * @param StatEnum The vitality stat to get the value for
 * @return The value of the natural core stat, or zero if not set or used.
 */
float UVitalityStatComponent::GetNaturalCoreStat(EVitalityStat StatEnum)
{
	return GetCoreStatValue(BaseStats_, StatEnum);
}

/**
 * @brief Returns the total of the given gear-based core stat enum
 * @param StatEnum The vitality stat to get the value for
 * @return The value of the gear-based core stat, or zero if not set or used.
 */
float UVitalityStatComponent::GetGearCoreStat(EVitalityStat StatEnum)
{
	return GetCoreStatValue(GearStats_, StatEnum);
}

/**
 * @brief Returns the total of the given magical core stat enum
 * @param StatEnum The vitality stat to get the value for
 * @return The value of the magical core stat, or zero if not set or used.
 */
float UVitalityStatComponent::GetMagicalCoreStat(EVitalityStat StatEnum)
{
	return GetCoreStatValue(ModifiedStats_, StatEnum);
}

/**
 * @brief Returns the total of the given other-type core stat enum
 * @param StatEnum The vitality stat to get the value for
 * @return The value of the other-type core stat, or zero if not set or used.
 */
float UVitalityStatComponent::GetOtherCoreStat(EVitalityStat StatEnum)
{
	return GetCoreStatValue(OtherStats_, StatEnum);
}

void UVitalityStatComponent::InitializeCoreStats(float StrengthValue, float AgilityValue, float FortitudeValue,
	float IntellectValue, float AstutenessValue, float CharismaValue)
{
	Server_InitializeCoreStats(StrengthValue, AgilityValue, FortitudeValue,
			IntellectValue, AstutenessValue, CharismaValue);
}

void UVitalityStatComponent::InitializeNaturalDamageBonuses(const TArray<float>& DamageMap)
{
	Server_InitializeNaturalDamageBonuses(DamageMap);
}

void UVitalityStatComponent::InitializeNaturalDamageResists(const TArray<float>& DamageMap)
{
	Server_InitializeNaturalDamageResists(DamageMap);
}

void UVitalityStatComponent::Server_InitializeCoreStats_Implementation(float StrengthValue, float AgilityValue,
                                                                       float FortitudeValue, float IntellectValue, float AstutenessValue, float CharismaValue)
{
	if (GetNetMode() < NM_Client && !bStatsSystemReady)
	{
		bStatsSystemReady = true;
		SetNaturalCoreStat(EVitalityStat::STRENGTH,		StrengthValue);
		SetNaturalCoreStat(EVitalityStat::AGILITY,		AgilityValue);
		SetNaturalCoreStat(EVitalityStat::FORTITUDE,	FortitudeValue);
		SetNaturalCoreStat(EVitalityStat::INTELLECT,	IntellectValue);
		SetNaturalCoreStat(EVitalityStat::ASTUTENESS,	AstutenessValue);
		SetNaturalCoreStat(EVitalityStat::CHARISMA,		CharismaValue);
	}
}

void UVitalityStatComponent::Server_InitializeNaturalDamageBonuses_Implementation(const TArray<float>& DamageMap)
{
	if (GetNetMode() < NM_Client && !bDamageBonusesReady)
	{
		bDamageBonusesReady = true;
		for (int i = 0; i < DamageMap.Num(); i++)
			SetNaturalDamageBonusValue(static_cast<EDamageType>(i), DamageMap[i]);
	}
}

void UVitalityStatComponent::Server_InitializeNaturalDamageResists_Implementation(const TArray<float>& DamageMap)
{
	if (GetNetMode() < NM_Client && !bDamageResistsReady)
	{
		bDamageResistsReady = true;
		for (int i = 0; i < DamageMap.Num(); i++)
			SetNaturalResistanceValue(static_cast<EDamageType>(i), DamageMap[i]);
	}
}

void UVitalityStatComponent::BindListenerEvents()
{
	if (!BaseStats_.OnCoreStatUpdated.IsAlreadyBound(this, &UVitalityStatComponent::NaturalCoreStatUpdated))
		 BaseStats_.OnCoreStatUpdated.AddDynamic(this, &UVitalityStatComponent::NaturalCoreStatUpdated);
	if (!BaseStats_.OnDamageBonusUpdated.IsAlreadyBound(this, &UVitalityStatComponent::NaturalDamageBonusUpdated))
		 BaseStats_.OnDamageBonusUpdated.AddDynamic(this, &UVitalityStatComponent::NaturalDamageBonusUpdated);
	if (!BaseStats_.OnDamageResistanceUpdated.IsAlreadyBound(this, &UVitalityStatComponent::NaturalDamageResistUpdated))
		 BaseStats_.OnDamageResistanceUpdated.AddDynamic(this, &UVitalityStatComponent::NaturalDamageResistUpdated);
	
	if (!GearStats_.OnCoreStatUpdated.IsAlreadyBound(this, &UVitalityStatComponent::GearCoreStatUpdated))
		 GearStats_.OnCoreStatUpdated.AddDynamic(this, &UVitalityStatComponent::GearCoreStatUpdated);
	if (!GearStats_.OnDamageBonusUpdated.IsAlreadyBound(this, &UVitalityStatComponent::GearDamageBonusUpdated))
		 GearStats_.OnDamageBonusUpdated.AddDynamic(this, &UVitalityStatComponent::GearDamageBonusUpdated);
	if (!GearStats_.OnDamageResistanceUpdated.IsAlreadyBound(this, &UVitalityStatComponent::GearDamageResistUpdated))
		 GearStats_.OnDamageResistanceUpdated.AddDynamic(this, &UVitalityStatComponent::GearDamageResistUpdated);
	
	if (!ModifiedStats_.OnCoreStatUpdated.IsAlreadyBound(this, &UVitalityStatComponent::MagicCoreStatUpdated))
		 ModifiedStats_.OnCoreStatUpdated.AddDynamic(this, &UVitalityStatComponent::MagicCoreStatUpdated);
	if (!ModifiedStats_.OnDamageBonusUpdated.IsAlreadyBound(this, &UVitalityStatComponent::MagicDamageBonusUpdated))
		 ModifiedStats_.OnDamageBonusUpdated.AddDynamic(this, &UVitalityStatComponent::MagicDamageBonusUpdated);
	if (!ModifiedStats_.OnDamageResistanceUpdated.IsAlreadyBound(this, &UVitalityStatComponent::MagicDamageResistUpdated))
		 ModifiedStats_.OnDamageResistanceUpdated.AddDynamic(this, &UVitalityStatComponent::MagicDamageResistUpdated);
	
	if (!OtherStats_.OnCoreStatUpdated.IsAlreadyBound(this, &UVitalityStatComponent::OtherCoreStatUpdated))
		 OtherStats_.OnCoreStatUpdated.AddDynamic(this, &UVitalityStatComponent::OtherCoreStatUpdated);
	if (!OtherStats_.OnDamageBonusUpdated.IsAlreadyBound(this, &UVitalityStatComponent::OtherDamageBonusUpdated))
		 OtherStats_.OnDamageBonusUpdated.AddDynamic(this, &UVitalityStatComponent::OtherDamageBonusUpdated);
	if (!OtherStats_.OnDamageResistanceUpdated.IsAlreadyBound(this, &UVitalityStatComponent::OtherDamageResistUpdated))
		 OtherStats_.OnDamageResistanceUpdated.AddDynamic(this, &UVitalityStatComponent::OtherDamageResistUpdated);

}

void UVitalityStatComponent::StatsEventTrigger(
	const FStVitalityStats* OldStats, const FStVitalityStats* NewStats)
{
	for (int i = 0; i < static_cast<int>(EVitalityStat::MAX); i++)
	{
		if (NewStats->CoreStats.IsValidIndex(i) && OldStats->CoreStats.IsValidIndex(i))
		{
			if (NewStats->CoreStats[i] != NewStats->CoreStats[i])
				OnCoreStatModified.Broadcast(static_cast<EVitalityStat>(i));	
		}
	}
	for (int i = 0; i < static_cast<int>(EDamageType::MAX); i++)
	{
		if (NewStats->DamageBonuses.IsValidIndex(i) && OldStats->DamageBonuses.IsValidIndex(i))
		{
			if (NewStats->DamageBonuses[i] != OldStats->DamageBonuses[i])
				OnDamageBonusUpdated.Broadcast(static_cast<EDamageType>(i));	
		}
		if (NewStats->DamageResists.IsValidIndex(i) && OldStats->DamageResists.IsValidIndex(i))
		{
			if (NewStats->DamageResists[i] != OldStats->DamageResists[i])
				OnDamageResistUpdated.Broadcast(static_cast<EDamageType>(i));	
		}
	}
}

void UVitalityStatComponent::OnRep_BaseStatsChanged_Implementation(FStVitalityStats OldBaseStats)
{
	UE_LOGFMT(LogTemp, Display, "{cName}({Sv}): OnRep_BaseStatsChanged()", *GetName(), GetOwner()->HasAuthority()?"S":"C");
	StatsEventTrigger(&OldBaseStats, &BaseStats_);
}

void UVitalityStatComponent::OnRep_GearStatsChanged_Implementation(FStVitalityStats OldGearStats)
{
	StatsEventTrigger(&OldGearStats, &GearStats_);
}

void UVitalityStatComponent::OnRep_ModifiedStatsChanged_Implementation(FStVitalityStats OldModifiedStats)
{
	StatsEventTrigger(&OldModifiedStats, &ModifiedStats_);
}

void UVitalityStatComponent::OnRep_OtherStatsChanged_Implementation(FStVitalityStats OldOtherStats)
{
	StatsEventTrigger(&OldOtherStats, &OtherStats_);
}

void UVitalityStatComponent::BeginPlay()
{
	Super::BeginPlay();
	BindListenerEvents();
	if (GetNetMode() < NM_Client)
	{
		Reinitialize();
	}
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
	DOREPLIFETIME_CONDITION(UVitalityStatComponent, BaseStats_, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UVitalityStatComponent, GearStats_, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UVitalityStatComponent, ModifiedStats_, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UVitalityStatComponent, OtherStats_, COND_OwnerOnly);
}

void UVitalityStatComponent::LoadDataDelegate(const FString& SaveSlotName, int32 UserIndex, USaveGame* SaveData)
{
	/*
	if (!IsValid(SaveData))
	{
		if (UGameplayStatics::CreateSaveGameObject( UInventorySave::StaticClass() ))
		{
			InventorySaveSlotName_ = SaveSlotName;
			OnInventoryRestored.Broadcast(true);
			return;
		}
		OnInventoryRestored.Broadcast(false);
		return;
	}
	const UInventorySave* InventorySave = Cast<UInventorySave>( SaveData );
	
	if (!IsValid(InventorySave))
		return;
	
	RestoreInventory(InventorySave->InventorySlots_, InventorySave->EquipmentSlots_);
	OnInventoryRestored.Broadcast(true);
	*/
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
	const FStVitalityStats& StatsMap, const EDamageType DamageEnum)  const
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
