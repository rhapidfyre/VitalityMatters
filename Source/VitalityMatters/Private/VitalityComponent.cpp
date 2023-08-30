﻿
#include "VitalityComponent.h"
#include "GameFramework/Character.h"

#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UVitalityComponent::UVitalityComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UVitalityComponent::OnComponentCreated()
{
	Super::OnComponentCreated();
	SetAutoActivate(true);
	RegisterComponent();
}

/** An alternative to 'ModifyVitalityStat(HEALTH)'. Calls appropriate
 * events and handles death events as expected during normal gameplay, whereas
 * ModifyVitalityStat() only manages the internal values without notifiers.
 * @param DamageActor The AActor who dealt the damage. nullptr is treated as world damage.
 * @param DamageTaken The amount of damage taken. Defaults to zero float.
 * @return Returns the new health value. Negative return indicates failure.
 */
float UVitalityComponent::DamageHealth(AActor* DamageActor, float DamageTaken)
{
	if (FMath::IsNearlyZero(DamageTaken,0.001f))
		return mHealthValue;
	
	const float oldHealth = mHealthValue;
	const float NewDamage = 0.f - abs(DamageTaken); // ensures a negative
	const float newHealth = ModifyVitalityStat(EVitalityCategory::HEALTH, NewDamage);
	if (newHealth < oldHealth)
	{
		if (newHealth <= 0.f && oldHealth > 0.f)
		{
			//OnDeath.Broadcast(DamageActor);
			Multicast_VitalityDeath(DamageActor);
		}
		else
		{
			//OnDamageTaken.Broadcast(GetOwner(), DamageActor, NewDamage);
			Multicast_DamageTaken(GetOwner(), DamageActor, NewDamage);
		}
	}
	return newHealth;
}

/** An alternative to 'ModifyVitalityStat(STAMINA)'. Calls appropriate
 * events and handles death events as expected during normal gameplay, whereas
 * ModifyVitalityStat() only manages the internal values without notifiers.
 * @param DamageActor The AActor who dealt the damage. nullptr is treated as world damage.
 * @param DamageTaken The amount of damage taken. Defaults to zero float.
 * @return Returns the new health value. Negative return indicates failure.
 */
float UVitalityComponent::ConsumeStamina(AActor* DamageActor, float DamageTaken)
{
	const float NewDamage = 0.f - abs(DamageTaken); // ensures a negative
	const float newValue = ModifyVitalityStat(EVitalityCategory::STAMINA, NewDamage);
	return newValue;
}

/** An alternative to 'ModifyVitalityStat(HEALTH)'. Calls appropriate
 * events and handles death events as expected during normal gameplay, whereas
 * ModifyVitalityStat() only manages the internal values without notifiers.
 * @param DamageActor The AActor who dealt the damage. nullptr is treated as world damage.
 * @param DamageTaken The amount of damage taken. Defaults to zero float.
 * @return Returns the new health value. Negative return indicates failure.
 */
float UVitalityComponent::ConsumeMagic(AActor* DamageActor, float DamageTaken)
{
	const float NewDamage = 0.f - abs(DamageTaken); // ensures a negative
	const float newValue = ModifyVitalityStat(EVitalityCategory::MAGIC, NewDamage);
	return newValue;
}

void UVitalityComponent::SetNaturalResistanceValue(EDamageType DamageEnum, int NewValue)
{
	for (FStDamageIntMap IntMap : _BaseStats.DamageResists)
	{
		if (IntMap.DamageEnum == DamageEnum)
		{
			IntMap.MapValue = NewValue;
			OnDamageBonusModified.Broadcast(DamageEnum, IntMap.MapValue);
		}
	}
}

void UVitalityComponent::AddNaturalResistance(EDamageType DamageEnum, int AddValue)
{
	for (FStDamageIntMap IntMap : _BaseStats.DamageResists)
	{
		if (IntMap.DamageEnum == DamageEnum)
		{
			const int OldValue = IntMap.MapValue;
			IntMap.MapValue = OldValue + FMath::Abs(AddValue);
			OnDamageBonusModified.Broadcast(DamageEnum, IntMap.MapValue);
		}
	}
}

void UVitalityComponent::RemoveNaturalResistance(EDamageType DamageEnum, int RemoveValue)
{
	for (FStDamageIntMap IntMap : _BaseStats.DamageResists)
	{
		if (IntMap.DamageEnum == DamageEnum)
		{
			const int OldValue = IntMap.MapValue;
			IntMap.MapValue = OldValue - FMath::Abs(RemoveValue);
			OnDamageBonusModified.Broadcast(DamageEnum, IntMap.MapValue);
		}
	}
}

void UVitalityComponent::AddGearResistance(EDamageType DamageEnum, int AddValue)
{
	for (FStDamageIntMap IntMap : _GearStats.DamageResists)
	{
		if (IntMap.DamageEnum == DamageEnum)
		{
			const int OldValue = IntMap.MapValue;
			IntMap.MapValue = OldValue + FMath::Abs(AddValue);
			OnDamageBonusModified.Broadcast(DamageEnum, IntMap.MapValue);
		}
	}
}

void UVitalityComponent::RemoveGearResistance(EDamageType DamageEnum, int RemoveValue)
{
	for (FStDamageIntMap IntMap : _GearStats.DamageResists)
	{
		if (IntMap.DamageEnum == DamageEnum)
		{
			const int OldValue = IntMap.MapValue;
			IntMap.MapValue = OldValue - FMath::Abs(RemoveValue);
			OnDamageBonusModified.Broadcast(DamageEnum, IntMap.MapValue);
		}
	}
}

/**
 * @brief Returns the value of resistance to the given damage type
 * @param DamageEnum The damage enum type to retrieve
 * @return The value of the resistance factor
 */
int UVitalityComponent::GetResistance(EDamageType DamageEnum) const
{
	int TotalValue = 0;
	
	TArray<const FStCharacterStats*> StatCalculations;
	StatCalculations.Add(&_BaseStats);
	StatCalculations.Add(&_GearStats);
	StatCalculations.Add(&_AffectStats);
	
	for (const FStCharacterStats* StatGroup : StatCalculations)
	{
		for (const FStDamageIntMap& IntMap : StatGroup->DamageResists)
		{
			if (IntMap.DamageEnum == DamageEnum)
				TotalValue += IntMap.MapValue;
		}
	}
	
	return TotalValue;
}

void UVitalityComponent::SetNaturalDamageBonus(EDamageType DamageEnum, int NewValue)
{
	for (FStDamageIntMap IntMap : _BaseStats.DamageBonuses)
	{
		if (IntMap.DamageEnum == DamageEnum)
		{
			IntMap.MapValue = NewValue;
			OnDamageBonusModified.Broadcast(DamageEnum, IntMap.MapValue);
		}
	}
}

void UVitalityComponent::AddNaturalDamageBonus(EDamageType DamageEnum, int AddValue)
{
	for (FStDamageIntMap IntMap : _BaseStats.DamageBonuses)
	{
		if (IntMap.DamageEnum == DamageEnum)
		{
			const int OldValue = IntMap.MapValue;
			IntMap.MapValue = OldValue + FMath::Abs(AddValue);
			OnDamageBonusModified.Broadcast(DamageEnum, IntMap.MapValue);
		}
	}
}

void UVitalityComponent::RemoveNaturalDamageBonus(EDamageType DamageEnum, int RemoveValue)
{
	for (FStDamageIntMap IntMap : _BaseStats.DamageBonuses)
	{
		if (IntMap.DamageEnum == DamageEnum)
		{
			const int OldValue = IntMap.MapValue;
			IntMap.MapValue = OldValue - FMath::Abs(RemoveValue);
			OnDamageBonusModified.Broadcast(DamageEnum, IntMap.MapValue);
		}
	}
}

void UVitalityComponent::AddGearDamageBonus(EDamageType DamageEnum, int AddValue)
{
	for (FStDamageIntMap IntMap : _GearStats.DamageBonuses)
	{
		if (IntMap.DamageEnum == DamageEnum)
		{
			const int OldValue = IntMap.MapValue;
			IntMap.MapValue = OldValue + FMath::Abs(AddValue);
			OnDamageBonusModified.Broadcast(DamageEnum, IntMap.MapValue);
		}
	}
}

void UVitalityComponent::RemoveGearDamageBonus(EDamageType DamageEnum, int RemoveValue)
{
	for (FStDamageIntMap IntMap : _GearStats.DamageBonuses)
	{
		if (IntMap.DamageEnum == DamageEnum)
		{
			const int OldValue = IntMap.MapValue;
			IntMap.MapValue = OldValue - FMath::Abs(RemoveValue);
			OnDamageBonusModified.Broadcast(DamageEnum, IntMap.MapValue);
		}
	}
}

// Call when the player's affects change to recalculate stats
void UVitalityComponent::RecalculateAffectedStats()
{
	// For each active effect
	TSet<FName> ProcessedEffectNames;
	
	FStCharacterStats AffectStatsModified = FStCharacterStats();
	
	for (const FStVitalityEffects& ActiveEffect : mCurrentEffects)
	{
		// Check if this effect stacks, if it's been processed already
		if (ProcessedEffectNames.Contains(ActiveEffect.properName))
		{
			if (!ActiveEffect.bEffectStacks)
			{
				// If the effect doesn't stack, skip.
				continue;
			}
		}
		
		// Effect stacks, or hasn't been processed yet
		AffectStatsModified.Strength	+= ActiveEffect.StatsAffected.Strength;
		AffectStatsModified.Agility		+= ActiveEffect.StatsAffected.Agility;
		AffectStatsModified.Fortitude 	+= ActiveEffect.StatsAffected.Fortitude;
		AffectStatsModified.Intellect 	+= ActiveEffect.StatsAffected.Intellect;
		AffectStatsModified.Astuteness	+= ActiveEffect.StatsAffected.Astuteness;
		AffectStatsModified.Charisma	+= ActiveEffect.StatsAffected.Charisma;

		const TArray<FStDamageIntMap> DamageBonuses = ActiveEffect.StatsAffected.DamageBonuses;
		if (!DamageBonuses.IsEmpty())
		{
			for (int i = 0; i < DamageBonuses.Num(); i++)
			{
				bool AffectExists = false;
				for (int j = 0; j < AffectStatsModified.DamageBonuses.Num(); j++)
				{
					const FStDamageIntMap ThisIntMap = AffectStatsModified.DamageBonuses[j]; 
					if (ThisIntMap.IsSameDamageType(DamageBonuses[i].DamageEnum))
					{
						AffectExists = true;
						break;
					}
				}
				
			}
			
		}
		
	}
	_AffectStats = AffectStatsModified;
}

/**
 * @brief Returns the value of damage bonus for the given damage type
 * @param DamageEnum The damage enum type to retrieve
 * @return The value of the damage bonus
 */
int UVitalityComponent::GetDamageBonus(EDamageType DamageEnum) const
{
	int TotalValue = 0;
	
	TArray<const FStCharacterStats*> StatCalculations;
	StatCalculations.Add(&_BaseStats);
	StatCalculations.Add(&_GearStats);
	StatCalculations.Add(&_AffectStats);
	
	for (const FStCharacterStats* StatGroup : StatCalculations)
	{
		for (const FStDamageIntMap& IntMap : StatGroup->DamageBonuses)
		{
			if (IntMap.DamageEnum == DamageEnum)
				TotalValue += IntMap.MapValue;
		}
	}
	
	return TotalValue;
}

/** Client or Server\n Gets the request vitality enum value.
 * @param VitalityStat The stat to be retrieved
 * @param StatValue The value (by ref) of the current stat
 * @param StatMax The maximum value (by ref) of the stat
 * @return The value of health, as a percentage
 */
float UVitalityComponent::GetVitalityStat(EVitalityCategory VitalityStat,
		float &StatValue, float &StatMax) const
{
	
	switch(VitalityStat)
	{
	case EVitalityCategory::HEALTH:
		StatValue = mHealthValue;
		StatMax = mHealthMax;
		return mHealthValue/mHealthMax;
	case EVitalityCategory::STAMINA:
		StatValue = mStaminaValue;
		StatMax = mStaminaMax;
		return mStaminaValue/mStaminaMax;
	case EVitalityCategory::HUNGER:
		StatValue = mCaloriesValue;
		StatMax = mCaloriesMax;
		return mCaloriesValue/mCaloriesMax;
	case EVitalityCategory::THIRST:
		StatValue = mHydrationValue;
		StatMax = mHydrationMax;
		return mHydrationValue/mHydrationMax;
	case EVitalityCategory::MAGIC:
		StatValue = mMagicValue;
		StatMax = mMagicMax;
		return mMagicValue/mMagicMax;
	case EVitalityCategory::STRENGTH:
		StatValue = _BaseStats.Strength;
		StatMax = _BaseStats.Strength;
		return 1.0f;
		
	case EVitalityCategory::AGILITY:
		StatValue = _BaseStats.Agility;
		StatMax = _BaseStats.Agility;
		return 1.0f;
		
	case EVitalityCategory::FORTITUDE:
		StatValue = _BaseStats.Fortitude;
		StatMax = _BaseStats.Fortitude;
		return 1.0f;
		
	case EVitalityCategory::INTELLECT:
		StatValue = _BaseStats.Intellect;
		StatMax = _BaseStats.Intellect;
		return 1.0f;
		
	case EVitalityCategory::ASTUTENESS:
		StatValue = _BaseStats.Astuteness;
		StatMax = _BaseStats.Astuteness;
		return 1.0f;
		
	case EVitalityCategory::CHARISMA:
		StatValue = _BaseStats.Charisma;
		StatMax = _BaseStats.Charisma;
		return 1.0f;
		
	default:
		break;
	}
	return -0.f;
}

/** Client or Server\n C++ Overload - Gets the request vitality enum value.
 * @param VitalityStat The stat to be retrieved
 * @return The value of health, as a percentage
 */
float UVitalityComponent::GetVitalityStat(EVitalityCategory VitalityStat)
{
	switch(VitalityStat)
	{
	case EVitalityCategory::HEALTH:
		return mHealthValue/mHealthMax;
	case EVitalityCategory::STAMINA:
		return mStaminaValue/mStaminaMax;
	case EVitalityCategory::HUNGER:
		return mCaloriesValue/mCaloriesMax;
	case EVitalityCategory::THIRST:
		return mHydrationValue/mHydrationMax;
	case EVitalityCategory::MAGIC:
		return mMagicValue/mMagicMax;
	default:
		break;
	}
	return -0.f;
}

/** Server Only \n Sets the given stat to the given value.
 * Does nothing if run on the client. Straight logic, no math.
 * @param VitalityStat The enum to modify. Defaults to health.
 * @param NewValue The new value of the stat. Defaults to 100.f
 */
float UVitalityComponent::SetVitalityStat(EVitalityCategory VitalityStat, float NewValue)
{
	switch(VitalityStat)
	{
	case EVitalityCategory::HEALTH:
		
		mHealthValue = NewValue;
		
		// Is Character Dead?
		if (mHealthValue <= 0.f)
		{
			if (GetAllEffectsByDetriment(EEffectsDetrimental::DEAD).Num() == 0)
			{
				mEffectsAddQueue.Add( UVitalitySystem::GetVitalityEffect("dead") );
			}
			mHealthValue = 0.f;
			return 0.f;
		}

		// Is player's health below maximum?
		if (mHealthValue < mHealthMax)
		{
			// Regen Timer is inactive/invalid
			if (!mHealthTimer.IsValid())
			{
				FTimerDelegate healthDelegate; healthDelegate.BindUObject(this, &UVitalityComponent::TickHealth);
				InitializeTimer(mHealthTimer, healthDelegate);
			}
		}
		
		return mHealthValue;
		
	case EVitalityCategory::STAMINA:
		mStaminaValue = NewValue;
		if (mStaminaValue <= 0.f)
		{
			if (GetAllEffectsByDetriment(EEffectsDetrimental::TIRED).Num() == 0)
			{
				mEffectsAddQueue.Add( UVitalitySystem::GetVitalityEffect("tired") );
			}
			mStaminaValue = 0.f;
		}
		else if (mStaminaValue < mStaminaMax && !mStaminaTimer.IsValid())
		{
			FTimerDelegate staminaDelegate; staminaDelegate.BindUObject(this, &UVitalityComponent::TickStamina);
			InitializeTimer(mStaminaTimer, staminaDelegate);
		}
		return mStaminaValue;
		
	case EVitalityCategory::MAGIC:
		mMagicValue = NewValue;
		return mMagicValue;
		
		
	case EVitalityCategory::HUNGER:
		mCaloriesValue = NewValue;
		if (mCaloriesValue > (mCaloriesMax * 0.7))
		{
			if (GetAllEffectsByBenefit(EEffectsBeneficial::NOURISHED).Num() == 0)
			{
				mEffectsAddQueue.Add( UVitalitySystem::GetVitalityEffect("nourished") );
			}
		}
		else if (mCaloriesValue < 0)
		{
			if (GetAllEffectsByDetriment(EEffectsDetrimental::HUNGER).Num() == 0)
			{
				mEffectsAddQueue.Add( UVitalitySystem::GetVitalityEffect("hungry") );
			}
		}
		else if (!mCaloriesTimer.IsValid())
		{
			FTimerDelegate caloriesDelegate; caloriesDelegate.BindUObject(this, &UVitalityComponent::TickCalories);
			InitializeTimer(mCaloriesTimer, caloriesDelegate);
		}
		return mCaloriesValue;
		
	case EVitalityCategory::THIRST:
		mHydrationValue = NewValue;
		if (mHydrationValue > (mHydrationMax * 0.7))
		{
			if (GetAllEffectsByBenefit(EEffectsBeneficial::HYDRATED).Num() == 0)
			{
				mEffectsAddQueue.Add( UVitalitySystem::GetVitalityEffect("hydrated") );
			}
		}
		else if (mHydrationValue)
		{
			if (GetAllEffectsByDetriment(EEffectsDetrimental::THIRST).Num() == 0)
			{
				mEffectsAddQueue.Add( UVitalitySystem::GetVitalityEffect("thirst") );
			}
		}
		else if (!mHydrationTimer.IsValid())
		{
			FTimerDelegate hydrationDelegate; hydrationDelegate.BindUObject(this, &UVitalityComponent::TickHydration);
			InitializeTimer(mHydrationTimer, hydrationDelegate);
		}
		return mHydrationValue;
		
	case EVitalityCategory::STRENGTH:
		if (NewValue < 0.f)	_BaseStats.Strength = 0;
		else				_BaseStats.Strength = FMath::RoundToInt(NewValue);
		break;
		
	case EVitalityCategory::AGILITY:
		if (NewValue < 0.f)	_BaseStats.Agility = 0;
		else				_BaseStats.Agility = FMath::RoundToInt(NewValue);
		break;
		
	case EVitalityCategory::FORTITUDE:
		if (NewValue < 0.f)	_BaseStats.Fortitude = 0;
		else				_BaseStats.Fortitude = FMath::RoundToInt(NewValue);
		break;
		
	case EVitalityCategory::INTELLECT:
		if (NewValue < 0.f)	_BaseStats.Intellect = 0;
		else				_BaseStats.Intellect = FMath::RoundToInt(NewValue);
		break;
		
	case EVitalityCategory::ASTUTENESS:
		if (NewValue < 0.f)	_BaseStats.Astuteness = 0;
		else				_BaseStats.Astuteness = FMath::RoundToInt(NewValue);
		break;
		
	case EVitalityCategory::CHARISMA:
		if (NewValue < 0.f)	_BaseStats.Charisma = 0;
		else				_BaseStats.Charisma = FMath::RoundToInt(NewValue);
		break;
		
	default:
		break;
	}
	return -0.f;
}

/** Server Only\n Modifies the current value of the given stat, adding or
 * subtracting value to it, respective of the signed float given. Does NOT trigger
 * events. Use DamageHealth() or other similar functions to notify delegates.
 * @param VitalityStat The enum to modify. Defaults to health.
 * @param AddValue The value to add/remove. Sign sensitive. Defaults to 0.f
 * @return The new stat value (should be input value). Negative indicates error.
 */
float UVitalityComponent::ModifyVitalityStat(EVitalityCategory VitalityStat, float AddValue)
{
	if (!FMath::IsNearlyZero(AddValue,0.001f))
	{
		float statValue(0.f); float statMax(0.f);
		GetVitalityStat(VitalityStat, statValue, statMax);
		return SetVitalityStat(VitalityStat, statValue + AddValue);
	}
	return 0.f;
}

/** Removes the effect with the given Unique ID number.
 * @param UniqueId The unique ID to remove. Defaults to 0. Fails if < 1.
 * @return True on successful removal. False on failure, or if effect did not exist.
 */
bool UVitalityComponent::RemoveEffectByUniqueId(int UniqueId)
{
	if (UniqueId < 1)
		return false;
	// exp scope for good measure
	{
		FRWScopeLock ReadLock(mMutexLock, SLT_Write);
		for (int i = 0; i < mCurrentEffects.Num(); i++)
		{
			mEffectsRemoveQueue.Add(UniqueId);
			return true;
		}
	}
	return false;
}

/** Server Only\n Adds the requested benefit.
 * @param EffectName The table row name to apply.
 * @param StackCount The number of times to apply the effect
 * @return True if the effect was added. False on failure.
 */
bool UVitalityComponent::ApplyEffect(FName EffectName, int StackCount)
{
	FStVitalityEffects vitalityData = UVitalitySystem::GetVitalityEffect(EffectName);
	vitalityData.uniqueId = GenerateUniqueId();
	if (vitalityData.uniqueId < 1)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to generate unique ID for ApplyEffectBeneficial"));
		return false;
	}
	if (!vitalityData.properName.IsNone())
	{
		for (int i = 0; i < StackCount; i++)
		{
			mEffectsAddQueue.Add(vitalityData);
		}
	}
	return false;
}

/** Server Only\n Removes the requested benefit.
 * @param EffectName The table row name to revoke.
 * @param RemoveCount The number of times to remove the effect
 * @return True if the effect was removed at least once. False on failure.
 */
bool UVitalityComponent::RemoveEffect(FName EffectName, int RemoveCount)
{
	TArray<int> effectsRemoved;
	// exp scope for good measure
	{
		FRWScopeLock ReadLock(mMutexLock, SLT_Write);
		for (int i = 0; i < mCurrentEffects.Num(); i++)
		{
			if (effectsRemoved.Num() >= RemoveCount)
				break;
		
			if (mCurrentEffects[i].properName == EffectName)
			{
				effectsRemoved.Add(mCurrentEffects[i].uniqueId);
				mEffectsRemoveQueue.Add(mCurrentEffects[i].uniqueId);
			}
		
		}
	}
	return (effectsRemoved.Num() > 0);
}

/** Server Only\n Adds the requested benefit enum.
 * @param EffectBeneficial The num to apply/revoke.
 * @param StackCount The number of times to apply the effect
 * @return True if the effect was added. False on failure.
 */
bool UVitalityComponent::ApplyEffectBeneficial(EEffectsBeneficial EffectBeneficial, int StackCount)
{
	// Do nothing if the effect or stack count is invalid
	if (EffectBeneficial == EEffectsBeneficial::NONE || StackCount < 1)
		return false;

	// Obtain the appropriate vitality effect the given StackCount number of times
	FStVitalityEffects vitalityData = UVitalitySystem::GetVitalityEffectByBenefit(EffectBeneficial);
	vitalityData.uniqueId = GenerateUniqueId();
	if (vitalityData.uniqueId < 1)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to generate unique ID for ApplyEffectBeneficial"));
		return false;
	}
	if (vitalityData.benefitEffect != EEffectsBeneficial::NONE)
	{
		//exp scope for good measure
		{
			FRWScopeLock ReadLock(mMutexLock, SLT_Write);
			int effectStackCount(1);
			for (int i = 0; i < mCurrentEffects.Num(); i++)
			{
				if (mCurrentEffects[i].benefitEffect == EffectBeneficial)
					effectStackCount += 1;
			}
			for (int i = 0; i < StackCount; i++)
				mEffectsAddQueue.Add(vitalityData);
		}
	}
	return false;
}

/** Server Only\n Removes the requested benefit enum.
 * @param EffectBeneficial The num to apply/revoke.
 * @param StackCount The number of identical effects to remove
 * @return True if the effect was removed. False on failure.
 */
bool UVitalityComponent::RevokeEffectBeneficial(EEffectsBeneficial EffectBeneficial, int StackCount)
{
	// Do nothing if the effect or stack count is invalid
	if (EffectBeneficial == EEffectsBeneficial::NONE || StackCount < 1)
		return false;

	// Removes the given effect, StackCount number of times
	TArray<int> removedUniqueIds;
	
	// Explicit scope for good measure
	{
		FRWScopeLock ReadLock(mMutexLock, SLT_Write);
		for (int i = 0; i < mCurrentEffects.Num(); i++)
		{
			if (removedUniqueIds.Num() >= StackCount)
				break;
			// If this effect matches the benefit to revoke, remove it
			if (mCurrentEffects[i].benefitEffect == EffectBeneficial)
			{
				removedUniqueIds.Add(mCurrentEffects[i].uniqueId);
				mEffectsRemoveQueue.Add(mCurrentEffects[i].uniqueId);
			}
		}
	}
	
	for (int i = 0; i < removedUniqueIds.Num(); i++)
			OnEffectModified.Broadcast(removedUniqueIds[i], false);
	
	return (removedUniqueIds.Num() > 0);
}

/** Server Only\n Adds the requested detriment enum.
 * @param EffectDetrimental The num to apply/revoke.
 * @param StackCount The number of times to apply the effect
 * @return True if the effect was added. False on failure.
 */
bool UVitalityComponent::ApplyEffectDetrimental(EEffectsDetrimental EffectDetrimental, int StackCount)
{
	// Do nothing if the effect or stack count is invalid
	if (EffectDetrimental == EEffectsDetrimental::NONE || StackCount < 1)
		return false;

	// Obtain the effect data, and apply it the number of StackCount times
	FStVitalityEffects vitalityData = UVitalitySystem::GetVitalityEffectByDetriment(EffectDetrimental);
	vitalityData.uniqueId = GenerateUniqueId();
	if (vitalityData.uniqueId < 1)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to generate unique ID for ApplyEffectBeneficial"));
		return false;
	}
	if (vitalityData.detrimentEffect != EEffectsDetrimental::NONE)
	{
		if (mCurrentEffects.Num() > 0)
		{
			int effectStackCount(1);

			//exp scope
			{
				FRWScopeLock ReadLock(mMutexLock, SLT_ReadOnly);
				for (int i = 0; i < mCurrentEffects.Num(); i++)
				{
					if (mCurrentEffects[i].detrimentEffect == EffectDetrimental)
						effectStackCount += 1;
				}
			}

			for (int i = 0; i < StackCount; i++)
				mEffectsAddQueue.Add(vitalityData);

			// If this effect disables sprinting and sprint is enabled, disable it.
			if (vitalityData.disableSprinting && mCanSprint && effectStackCount > 0)
			{
				mCanSprint = false;
			}
		}
	}
	return false;
}

/** Server Only\n Removes the requested detriment enum.
 * @param EffectDetrimental The num to apply/revoke.
 * @param StackCount The number of identical effects to remove
 * @return True if the effect was removed. False on failure.
 */
bool UVitalityComponent::RevokeEffectDetrimental(EEffectsDetrimental EffectDetrimental, int StackCount)
{
	// Return false if invalid effect or stack count
	if (EffectDetrimental == EEffectsDetrimental::NONE || StackCount < 1)
		return false;


	// Remove the effect the given number of times, or until all occurrences are gone. Whichever occurs first.
	TArray<int> removedUniqueIds;
	
	// Explicit Scope for good measure
	{
		FRWScopeLock ReadLock(mMutexLock, SLT_Write);
		
		for (int i = 0; i < mCurrentEffects.Num(); i++)
		{
			if (removedUniqueIds.Num() >= StackCount)
				break;
		
			if (mCurrentEffects[i].detrimentEffect == EffectDetrimental)
			{
				removedUniqueIds.Add(mCurrentEffects[i].uniqueId);
				mEffectsRemoveQueue.Add(mCurrentEffects[i].uniqueId);
			}
		}
	}

	// Nothing was removed, the effect wasn't applied
	if (removedUniqueIds.Num() < 1)
		return true;
	
	// If sprinting is disabled, enable sprinting if none of the effects block sprinting
	if (!mCanSprint)
	{
		bool canSprint = true;
		if (mCurrentEffects.Num() > 0)
		{
			FRWScopeLock ReadLock(mMutexLock, SLT_Write);
			for (int i = 0; i < mCurrentEffects.Num(); i++)
			{
				if (mCurrentEffects[i].disableSprinting)
				{
					canSprint = false;
					break;
				}
			}
		}
		mCanSprint = canSprint;
	}
	
	return true;
}

/** Server Only\n Removes the effect at the given index.
 * @param IndexNumber The index number of the current effect to remove
 * @return True if the effect was removed. False on failure.
 */
bool UVitalityComponent::RemoveEffectAtIndex(int IndexNumber)
{
	if (mCurrentEffects.IsValidIndex(IndexNumber))
	{
		mEffectsRemoveQueue.Add(mCurrentEffects[IndexNumber].uniqueId);
		return true;
	}
	return false;
}

/** Server or Client\n Starts or Stops the Sprinting Mechanic.
 * @param DoSprint If true, attempts to start sprinting. False stops.
 */
void UVitalityComponent::ToggleSprint(bool DoSprint)
{
	if (!GetOwner()->HasAuthority())
	{
		Server_ToggleSprint(DoSprint);
		return;
	}
	if (DoSprint && mCanSprint)
	{
		StartSprinting();
	}
	else
	{
		mIsSprinting = false;
		OnSprint.Broadcast(mIsSprinting);
	}
}

/** Returns the number of active counts of the requested benefit
 * @param BenefitEffect The benefit effect to look for
 * @return The number of times the benefit is in effect (at the time of request)
 */
int UVitalityComponent::GetNumActiveBenefit(EEffectsBeneficial BenefitEffect)
{
	if (BenefitEffect == EEffectsBeneficial::NONE)
		return 0;
	int benefitCount(0);
	// exp scope for good measure
	{
		FRWScopeLock ReadLock(mMutexLock, SLT_Write);
		for (int i = 0; mCurrentEffects.Num(); i++)
		{
			if (mCurrentEffects[i].benefitEffect == BenefitEffect)
				benefitCount++;
		}
	}
	return benefitCount;
}

/** Returns the number of active counts of the requested detrimental effect
 * @param DetrimentEffect The detrimental effect to look for
 * @return The number of times the detrimental is in effect (at the time of request)
 */
int UVitalityComponent::GetNumActiveDetriment(EEffectsDetrimental DetrimentEffect)
{
	if (DetrimentEffect == EEffectsDetrimental::NONE)
		return 0;
	int detrimentCount(0);
	// exp scope for good measure
	{
		FRWScopeLock ReadLock(mMutexLock, SLT_Write);
		for (int i = 0; mCurrentEffects.Num(); i++)
		{
			if (mCurrentEffects[i].detrimentEffect == DetrimentEffect)
				detrimentCount++;
		}
	}
	return detrimentCount;
}

/** Returns a copy of the FStVitalityEffect data by given Unique Id. If there is no effect with the
 * requested UniqueId, or the UniqueId is invalid, this function will return empty table.
 * @return The data object found (or empty object)
 */
FStVitalityEffects UVitalityComponent::GetEffectByUniqueId(int UniqueId)
{
	if (UniqueId > 0)
	{
		FRWScopeLock ReadLock(mMutexLock, SLT_ReadOnly);
		// Simple needle in a haystack search
		for (FStVitalityEffects vEffect : mCurrentEffects)
		{
			if (vEffect.uniqueId == UniqueId)
			{
				return vEffect;
			}
		}
	}
	return {};
}

TArray<FStVitalityEffects> UVitalityComponent::GetAllEffectsByDetriment(EEffectsDetrimental DetrimentEffect)
{
	if (DetrimentEffect != EEffectsDetrimental::NONE)
	{
		FRWScopeLock ReadLock(mMutexLock, SLT_ReadOnly);
		// O(n) needle-in-a-haystack
		TArray<FStVitalityEffects> detrimentalEffects;
		for (FStVitalityEffects vEffect : mCurrentEffects)
		{
			if (vEffect.detrimentEffect != EEffectsDetrimental::NONE)
			{
				detrimentalEffects.Add(vEffect);
			}
		}
		return detrimentalEffects;
	}
	return {};
}

TArray<FStVitalityEffects> UVitalityComponent::GetAllEffectsByBenefit(EEffectsBeneficial BenefitEffect)
{
	if (BenefitEffect != EEffectsBeneficial::NONE)
	{
		FRWScopeLock ReadLock(mMutexLock, SLT_ReadOnly);
		// O(n) needle-in-a-haystack
		TArray<FStVitalityEffects> detrimentalEffects;
		for (FStVitalityEffects vEffect : mCurrentEffects)
		{
			if (vEffect.detrimentEffect != EEffectsDetrimental::NONE)
			{
				detrimentalEffects.Add(vEffect);
			}
		}
		return detrimentalEffects;
	}
	return {};
}

bool UVitalityComponent::IsEffectActive(FName EffectName)
{
	if (mCurrentEffects.Num() < 1)
		return false;
	
	// Does the requested effect exist?
	FStVitalityEffects vData = UVitalitySystem::GetVitalityEffect(EffectName);
	if (!UVitalitySystem::IsVitalityDataValid(vData))
		return false;
	
	// explicit scope
	{
		FRWScopeLock ReadLock(mMutexLock, SLT_ReadOnly);
		for (int i = 0; i < mCurrentEffects.Num(); i++)
		{
			// It was validated before it was added to array. There is no need to validate.
			FStVitalityEffects vTemp = mCurrentEffects[i];
			if (vTemp.properName == EffectName)
			{
				return true;
			}
		}
	}
	
	return false;
}

bool UVitalityComponent::IsEffectActive(EEffectsBeneficial EffectEnum)
{
	// Does the requested effect exist?
	FStVitalityEffects vData = UVitalitySystem::GetVitalityEffectByBenefit(EffectEnum);
	if (!UVitalitySystem::IsVitalityDataValid(vData))
		return false;
	return IsEffectActive(vData.properName);
}

bool UVitalityComponent::IsEffectActive(EEffectsDetrimental EffectEnum)
{
	FStVitalityEffects vData = UVitalitySystem::GetVitalityEffectByDetriment(EffectEnum);
	if (!UVitalitySystem::IsVitalityDataValid(vData))
		return false;
	return IsEffectActive(vData.properName);
}

void UVitalityComponent::SetCombatState(ECombatState CombatState)
{
	if (GetOwner()->HasAuthority())
	{
		float CombatTimer = 0.f;
		
		// Everytime hostile action is taken, reset the timer
		if (CombatState == ECombatState::ENGAGED)
		{
			if (mCombatStateTimer.IsValid())
				mCombatStateTimer.Invalidate();
			CombatTimer = 10.f;
		}
		
		if (_CombatState != CombatState)
		{
			
			const ECombatState OldState = _CombatState;
			_CombatState = CombatState;
			OnCombatStateChanged.Broadcast(OldState, _CombatState);
			
			if (OldState != ECombatState::ENGAGED)
			{
				// Player has either acquired or sustained alertness
				if (_CombatState == ECombatState::ALERT)
				{
					CombatTimer = 3.f;
				}
				// Player is now engaged in combat
				else if (_CombatState == ECombatState::ENGAGED)
				{
					CombatTimer = 10.f;
				}
			}
			else
			{
				// Player has left combat and is coming down from engagement
				if (_CombatState == ECombatState::ALERT)
				{
					CombatTimer = 10.f;
				}
			}
			
		}
		
		if (CombatTimer > 0.f)
		{
			if (mCombatStateTimer.IsValid())
				mCombatStateTimer.Invalidate();
			
			GetWorld()->GetTimerManager().SetTimer(mCombatStateTimer, this,
				&UVitalityComponent::DowngradeCombatState, CombatTimer, false);
		}
	}
}

// Called when the game starts
void UVitalityComponent::BeginPlay()
{
	Super::BeginPlay();
	InitSubsystems( IsValid( Cast<ACharacter>(GetOwner()) ) );
}

void UVitalityComponent::TickStamina()
{
	if (mIsSprinting && mCanSprint)
	{
		if (mStaminaValue <= 0)
		{
			StopSprinting();
			ApplyEffect("tired");
			mStaminaValue = 0;
		}
	}
	else
	{
		if (GetAllEffectsByDetriment(EEffectsDetrimental::TIRED).Num() == 0
		 && GetAllEffectsByDetriment(EEffectsDetrimental::HUNGER).Num() == 0)
		{			
			// If stamina is fully regenerated, kill the timer. It's not needed anymore.
			if (mStaminaValue >= mStaminaMax)
			{
				CancelTimer(mStaminaTimer);
				mStaminaValue = mStaminaMax;
			}
			else
			{
				mStaminaValue += mStaminaRegen;
			}
		}
	}
}

void UVitalityComponent::TickHealth()
{
	if (mHealthValue > 0.f)
	{
		if (mHealthValue < mHealthMax)
		{
			// Current calories percent
			const float caloriesPercent = GetVitalityStat(EVitalityCategory::HUNGER);

			if (caloriesPercent >= 0.5)
			{
				mHealthValue += mHealthRegen;	
			}
			else
			{
				// Health can regen up to twice the percentage of calories
				if ( ((mHealthValue/mHealthMax)*0.5) < caloriesPercent )
				{
					mHealthValue += mHealthRegen;
				}
			}
		}
		else if (mHealthValue > mHealthMax)
		{
			CancelTimer(mHealthTimer);
			mHealthValue = mHealthMax;
		}
	}
}

void UVitalityComponent::TickCalories()
{
	if (mCaloriesValue > 0.f)
	{
		mCaloriesValue -= mCaloriesDrainRest;
		if (mCaloriesValue <= 0.f)
		{
			CancelTimer(mCaloriesTimer);
			mCaloriesValue = 0.f;
			ApplyEffect("hungry");
		}
	}
}

void UVitalityComponent::TickHydration()
{
	if (mHydrationValue > 0.f)
	{
		mHydrationValue -= mHydrationDrainRest;
		if (mHydrationValue <= 0.f)
		{
			mCanSprint = false;
			mHydrationValue = 0.f;
			CancelTimer(mHydrationTimer);
			ApplyEffect("thirsty");
		}
	}
}

void UVitalityComponent::TickEffects()
{
	// Perform any logic effects need done per tick
	if (mCurrentEffects.Num() > 0)
	{
		FRWScopeLock WriteLock(mMutexLock, SLT_ReadOnly);
		for (int i = 0; i < mCurrentEffects.Num(); i++)
		{
			if (mCurrentEffects.IsValidIndex(i))
			{
				const FStVitalityEffects vitalityData = mCurrentEffects[i];
				if (!vitalityData.bIsPersistent)
				{
					mCurrentEffects[i].effectTicks--;
					if (mCurrentEffects[i].effectTicks < 1)
					{
						RemoveEffectAtIndex(i);
					}
				}
			}
		}
	}
	
	// Remove any effects that are pending removal
	if (mEffectsRemoveQueue.Num() > 0)
	{
		FRWScopeLock WriteLock(mMutexLock, SLT_Write);
		for (int i = mEffectsRemoveQueue.Num() - 1; i >= 0; i--)
		{
			for (int j = 0; j < mCurrentEffects.Num(); j++)
			{
				if (mCurrentEffects[j].uniqueId == mEffectsRemoveQueue[i])
				{
					mCurrentEffects.RemoveAt(j);
					OnEffectModified.Broadcast(mEffectsRemoveQueue[i], false);
					break;
				}
			}
			mEffectsRemoveQueue.RemoveAt(i);
		}
	}
	
	// Add any affects that need to be added
	if (mEffectsAddQueue.Num() > 0)
	{
		FRWScopeLock WriteLock(mMutexLock, SLT_Write);
		for (int i = mEffectsAddQueue.Num() - 1; i >= 0; i--)
		{
			mCurrentEffects.Add( mEffectsAddQueue[i] );
			OnEffectModified.Broadcast(mEffectsAddQueue[i].uniqueId, true);
			mEffectsAddQueue.RemoveAt(i);
		}
	}
	
}

void UVitalityComponent::TickManager()
{
}

void UVitalityComponent::InitSubsystems(bool isCharacter)
{
	// Character-Only Initialization
	if (isCharacter)
	{
		// Only character-types have stamina
		mSprintSpeed		= SprintSpeedMultiplier >  1.f ? SprintSpeedMultiplier	:		1.2;
		mStaminaDrain		= StaminaDrainRate		>= 0.f ? StaminaDrainRate		:		1.f;
		mStaminaRegen		= StaminaRegenRate		>= 0.f ? StaminaRegenRate		:		2.f;
		mStaminaMax			= StaminaMaximum		 > 0.f ? StaminaMaximum			:	  100.f;
		mStaminaValue		= mStaminaMax;
		
		mMagicMax			= MagicMaximum			>  0.f ? MagicMaximum			:		1.f;
		mCaloriesDrainRest	= CaloriesAtRest		>= 0.f ? CaloriesAtRest			:	    0.002;
		mHydrationDrainRest = HungerAtRest			>= 0.f ? HungerAtRest			:	    0.002;
		mCaloriesMax		= CaloriesMaximum		>  0.f ? CaloriesMaximum 		:	  100.f;
		mHydrationMax		= HydrationMaximum		>  0.f ? HydrationMaximum		:	  100.f;
		
	}
	// Non-Character Initialization (Crates, Ships, Etc)
	else
	{
		
	}
	
	// Initialization, All Cases
	
	//						Condition		 		   ?	If True				:	If False
	mHealthRegen		= HealthRegenRate		>= 0.f ? HealthRegenRate		:		0.5;
	mHealthMax			= HealthMaximum			 > 0.f ? HealthMaximum			:	  100.f;
	
	mHealthValue	= mHealthMax;
	mCaloriesValue	= mCaloriesMax;
	mHydrationValue = mHydrationMax;
	mMagicValue		= mMagicMax;

	// Set up timers

	if (mStaminaTimer.IsValid()) mStaminaTimer.Invalidate();
	FTimerDelegate staminaDelegate; staminaDelegate.BindUObject(this, &UVitalityComponent::TickStamina);
	InitializeTimer(mStaminaTimer, staminaDelegate);
	
	if (mHealthTimer.IsValid()) mHealthTimer.Invalidate();
	FTimerDelegate healthDelegate; healthDelegate.BindUObject(this, &UVitalityComponent::TickHealth);
	InitializeTimer(mHealthTimer, healthDelegate);
	
	if (mCaloriesTimer.IsValid()) mCaloriesTimer.Invalidate();
	FTimerDelegate caloriesDelegate; caloriesDelegate.BindUObject(this, &UVitalityComponent::TickCalories);
	InitializeTimer(mCaloriesTimer, caloriesDelegate);
	
	if (mHydrationTimer.IsValid()) mHydrationTimer.Invalidate();
	FTimerDelegate hydrationDelegate; hydrationDelegate.BindUObject(this, &UVitalityComponent::TickHydration);
	InitializeTimer(mHydrationTimer, hydrationDelegate);
	
	if (mEffectsTimer.IsValid()) mEffectsTimer.Invalidate();
	FTimerDelegate effectsDelegate; effectsDelegate.BindUObject(this, &UVitalityComponent::TickEffects);
	InitializeTimer(mEffectsTimer, effectsDelegate);
	
}

void UVitalityComponent::StopSprinting()
{
	if (GetOwner()->HasAuthority())
	{
		mIsSprinting = false;
		OnSprint.Broadcast(mIsSprinting);
	}
}

void UVitalityComponent::StartSprinting()
{
	if (GetOwner()->HasAuthority())
	{
		mIsSprinting = true;
		OnSprint.Broadcast(mIsSprinting);
	}
}

void UVitalityComponent::EndStaminaCooldown()
{
	if(mStaminaCooldownTimer.IsValid())
	{
		mStaminaCooldownTimer.Invalidate();
	}
}

void UVitalityComponent::Server_ToggleSprint_Implementation(bool DoSprint)
{
	ToggleSprint(DoSprint);
}

void UVitalityComponent::DowngradeCombatState()
{
	switch(_CombatState)
	{
	// Downgrade ENGAGED to ALERT
	case ECombatState::ENGAGED:
		SetCombatState(ECombatState::ALERT);
		break;
	// Downgrade ALERT to RELAXED
	case ECombatState::ALERT:
		SetCombatState(ECombatState::RELAXED);
		break;
	// No other states downgrade
	default:
		break;
	}
}

int UVitalityComponent::GenerateUniqueId()
{
	int randomNumber(-1);
	
	while (randomNumber < 1)
	{
		bool idExists = false;
		const int tempNumber = FMath::RandRange(1,INT_MAX);
		FRWScopeLock ReadLock(mMutexLock, SLT_ReadOnly);
		for (const FStVitalityEffects vEffect : mCurrentEffects)
		{
			if (vEffect.uniqueId == tempNumber)
			{
				idExists = true;
				break;
			}
		}
		// If unique, assign and break loop
		if (!idExists)
		{
			randomNumber = tempNumber;
			break;
		}
	}
	return randomNumber;
}

void UVitalityComponent::InitializeTimer(FTimerHandle& timerHandle, FTimerDelegate timerDelegate)
{
	if (!GetWorld()->GetTimerManager().TimerExists(timerHandle))
	{
		const float managerTickRate = VitalityTickRate <= 0.f ? 1.f : VitalityTickRate;
		GetWorld()->GetTimerManager().SetTimer(timerHandle,	timerDelegate, 1*managerTickRate, true);
		UE_LOG(LogTemp, Display, TEXT("%s(%s): Timer Handle '%s' has been created"),
			*GetName(), GetOwner()->HasAuthority()?TEXT("SERVER"):TEXT("CLIENT"), *timerHandle.ToString());
	}
}

void UVitalityComponent::CancelTimer(FTimerHandle& timerHandle)
{
	if (GetWorld()->GetTimerManager().TimerExists(timerHandle))
	if (GetWorld()->GetTimerManager().TimerExists(timerHandle))
	{
		if (timerHandle.IsValid())
		{
			UE_LOG(LogTemp, Display, TEXT("%s(%s): Timer Handle '%s' has been INVALIDATED"),
				*GetName(), GetOwner()->HasAuthority()?TEXT("SERVER"):TEXT("CLIENT"), *timerHandle.ToString());
			timerHandle.Invalidate();
		}
	}
}

void UVitalityComponent::OnRep_HydrationValue_Implementation()
{
	OnHydrationUpdated.Broadcast();
}

void UVitalityComponent::OnRep_CaloriesValue_Implementation()
{
	OnCaloriesUpdated.Broadcast();
}

void UVitalityComponent::OnRep_StaminaValue_Implementation()
{
	OnStaminaUpdated.Broadcast();
}

void UVitalityComponent::OnRep_HealthValue_Implementation()
{
	OnHealthUpdated.Broadcast();
}

void UVitalityComponent::OnRep_MagicValue_Implementation()
{
	OnMagicUpdated.Broadcast();
}

void UVitalityComponent::OnRep_CombatState_Implementation(ECombatState OldCombatState)
{
	OnCombatStateChanged.Broadcast(OldCombatState, _CombatState);
}

void UVitalityComponent::Multicast_VitalityDeath_Implementation(AActor* DamageActor)
{
	OnDeath.Broadcast(DamageActor);
}

void UVitalityComponent::Multicast_DamageTaken_Implementation(AActor* DamageTaker, AActor* DamageInstigator, float DamageTaken)
{
	OnDamageTaken.Broadcast(DamageTaker, DamageInstigator, DamageTaken);
}

void UVitalityComponent::OnRep_CurrentEffects_Implementation()
{
	OnEffectModified.Broadcast(0, true);
}


/****************************************
 * REPLICATION
***************************************/

void UVitalityComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Update to owner only.
	// These vars only have value to the actor it affects and no-one else.
	DOREPLIFETIME_CONDITION(UVitalityComponent, mIsSprinting,		COND_OwnerOnly);
	
	DOREPLIFETIME(UVitalityComponent, _CombatState);
	
	DOREPLIFETIME(UVitalityComponent, mStaminaValue);
	DOREPLIFETIME(UVitalityComponent, mStaminaMax);
	
	DOREPLIFETIME(UVitalityComponent, mHealthValue);
	DOREPLIFETIME(UVitalityComponent, mHealthMax);
	
	DOREPLIFETIME(UVitalityComponent, mMagicValue);
	DOREPLIFETIME(UVitalityComponent, mMagicMax);
	
	DOREPLIFETIME_CONDITION(UVitalityComponent, mCaloriesValue,		COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UVitalityComponent, mCaloriesMax,		COND_OwnerOnly);
	
	DOREPLIFETIME_CONDITION(UVitalityComponent, mHydrationValue,	COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UVitalityComponent, mHydrationMax,		COND_OwnerOnly);

	// Effects don't need to be multicast.
	// If the effect spawns an actor, it'll be done server-side and synced.
	DOREPLIFETIME_CONDITION(UVitalityComponent, mCurrentEffects,	COND_OwnerOnly);
}