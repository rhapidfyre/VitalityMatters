// Copyright Take Five Games, LLC 2023 - All Rights Reserved

#include "VitalityWelfareComponent.h"


/**
 * @brief Call whenever save games are loaded
 */
void UVitalityWelfareComponent::ReloadSettings()
{
	if (_HealthTimer.IsValid())		_HealthTimer.Invalidate();
	if (_MagicTimer.IsValid())		_MagicTimer.Invalidate();
	if (_StaminaTimer.IsValid())	_StaminaTimer.Invalidate();
	if (_CaloriesTimer.IsValid())	_CaloriesTimer.Invalidate();
	if (_HydrationTimer.IsValid())	_HydrationTimer.Invalidate();
	if (_CombatTimer.IsValid())		_CombatTimer.Invalidate();
	
	if (UseHealthSubsystem)
	{
		const bool MaximumValueIsValid	= StartingHealthMaximum	> 0.f;
		const bool CurrentValueIsValid	= StartingHealthCurrent	> 0.f;
		const bool RegenValueIsValid	= PassiveHealthRegen	> 0.f;
		const bool TimerTickRateIsValid	= HealthTimerTickRate	> 0.f;
		_HealthMax				= MaximumValueIsValid	? StartingHealthMaximum	: 1.f;
		_HealthCurrent			= CurrentValueIsValid	? StartingHealthCurrent	: 1.f;
		_HealthRegenAtRest		= RegenValueIsValid		? PassiveHealthRegen	: 1.f;
		_HealthTimerTickRate	= TimerTickRateIsValid	? HealthTimerTickRate	: 0.5;
		if (_HealthCurrent < _HealthMax)
		{
			FTimerDelegate InitDelegate;
			InitDelegate.BindUObject(this, &UVitalityWelfareComponent::TickHealth);
			InitializeTimer(_HealthTimer, InitDelegate);
		}
	}
	else
	{
		_HealthMax = 0.f;
		_HealthCurrent = 0.f;
		_HealthRegenAtRest = 0.f;
		_HealthTimerTickRate = 0.5;
	}
	
	if (UseStaminaSubsystem)
	{
		const bool MaximumValueIsValid	= StartingStaminaMaximum	> 0.f;
		const bool CurrentValueIsValid	= StartingStaminaCurrent	> 0.f;
		const bool RegenValueIsValid	= PassiveStaminaRegen		> 0.f;
		const bool TimerTickRateIsValid	= StaminaTimerTickRate		> 0.f;
		_StaminaMax				= MaximumValueIsValid	? StartingStaminaMaximum	: 1.f;
		_StaminaCurrent			= CurrentValueIsValid	? StartingStaminaCurrent	: 1.f;
		_StaminaRegenAtRest		= RegenValueIsValid		? PassiveStaminaRegen		: 1.f;
		_StaminaTimerTickRate	= TimerTickRateIsValid	? StaminaTimerTickRate		: 0.5;
		if (_StaminaCurrent < _StaminaMax)
		{
			FTimerDelegate InitDelegate;
			InitDelegate.BindUObject(this, &UVitalityWelfareComponent::TickStamina);
			InitializeTimer(_StaminaTimer, InitDelegate);
		}
	}
	else
	{
		_StaminaMax = 0.f;
		_StaminaCurrent = 0.f;
		_StaminaRegenAtRest = 0.f;
		_StaminaTimerTickRate = 0.5;
	}
	
	if (UseMagicSubsystem)
	{
		const bool MaximumValueIsValid	= StartingMagicMaximum	> 0.f;
		const bool CurrentValueIsValid	= StartingMagicCurrent	> 0.f;
		const bool RegenValueIsValid	= PassiveMagicRegen		> 0.f;
		const bool TimerTickRateIsValid	= MagicTimerTickRate	> 0.f;
		_MagicMax			= MaximumValueIsValid	? StartingMagicMaximum	: 1.f;
		_MagicCurrent		= CurrentValueIsValid	? StartingMagicCurrent	: 1.f;
		_MagicRegenAtRest	= RegenValueIsValid		? PassiveMagicRegen		: 1.f;
		_MagicTimerTickRate	= TimerTickRateIsValid	? MagicTimerTickRate	: 0.5;
		if (_MagicCurrent < _MagicMax)
		{
			FTimerDelegate InitDelegate;
			InitDelegate.BindUObject(this, &UVitalityWelfareComponent::TickMagic);
			InitializeTimer(_MagicTimer, InitDelegate);
		}
	}
	else
	{
		_StaminaMax = 0.f;
		_StaminaCurrent = 0.f;
		_StaminaRegenAtRest = 0.f;
		_StaminaTimerTickRate = 0.5;
	}
	
	if (UseSurvivalSubsystem)
	{
		_HydrationMax			= StartingMagicMaximum		> 0.f	? StartingMagicMaximum		: 1.f;
		_HydrationCurrent		= StartingMagicCurrent		> 0.f	? StartingMagicCurrent		: 1.f;
		_HydrationDrainAtRest	= PassiveHydrationDrain		> 0.f	? PassiveHydrationDrain		: 0.082;
		_HydrationTimerTickRate	= HydrationTimerTickRate	> 0.f	? HydrationTimerTickRate	: 0.5;
		if (_HydrationCurrent < _HydrationMax)
		{
			FTimerDelegate InitDelegate;
			InitDelegate.BindUObject(this, &UVitalityWelfareComponent::TickMagic);
			InitializeTimer(_HydrationTimer, InitDelegate);
		}
		
		_CaloriesMax			= StartingHungerMaximum	> 0.f	? StartingHungerMaximum	: 1.f;
		_CaloriesCurrent		= StartingHungerCurrent	> 0.f	? StartingHungerCurrent	: 1.f;
		_CaloriesDrainAtRest	= PassiveHungerDrain	> 0.f	? PassiveHungerDrain	: 0.082;
		_HungerTimerTickRate	= CaloriesTimerTickRate	> 0.f	? CaloriesTimerTickRate	: 0.5;
		if (_CaloriesCurrent < _CaloriesMax)
		{
			FTimerDelegate InitDelegate;
			InitDelegate.BindUObject(this, &UVitalityWelfareComponent::TickMagic);
			InitializeTimer(_CaloriesTimer, InitDelegate);
		}
	}
	else
	{
		_HydrationMax			= 0.f;	_CaloriesMax			= 0.f;
		_HydrationCurrent		= 0.f;	_CaloriesCurrent		= 0.f;
		_HydrationDrainAtRest	= 0.f;	_CaloriesDrainAtRest	= 0.f;
		_HydrationTimerTickRate	= 0.5;	_HungerTimerTickRate	= 0.5;
	}
}

/**
 * @brief Damages the components health value, performing internal logic and firing delegates.
 * @param DamageInstigator The actor who applied the damage. Nullptr indicated environmental damage.
 * @param DamageTaken The amount of damage that is being applied.
 * @return The new value of the components current health value
 */
float UVitalityWelfareComponent::DamageHealth(AActor* DamageInstigator, float DamageTaken)
{
	const float NewDamageValue = abs(DamageTaken);
	if (!FMath::IsNearlyZero(NewDamageValue))
	{
		if (!GetIsDead)
		{
			bool IsNewDamage = true;
			for (FStDamageData DamageData : _DamageHistory)
			{
				if (DamageData.DamagingActor == DamageInstigator)
				{
					// ReSharper disable once CppAssignedValueIsNeverUsed
					DamageData.LastDamageValue   = NewDamageValue;
					// ReSharper disable once CppAssignedValueIsNeverUsed
					DamageData.TotalDamageDealt += NewDamageValue;
					IsNewDamage = false;
				}
			}
			if (IsNewDamage)
				_DamageHistory.Add(FStDamageData(DamageInstigator, NewDamageValue));

			_HealthCurrent -= NewDamageValue;
			if (_HealthCurrent <= 0.f && !GetIsDead())
			{
				_IsDead = true;
				OnDeath.Broadcast(DamageInstigator);
			}
			OnDamageTaken.Broadcast(GetOwner(), DamageInstigator, NewDamageValue);
		}
	}
	return _HealthCurrent;
}

/**
 * @brief Damages the components stamina value, performing internal logic and firing delegates.
 * @param DamageInstigator The actor who applied the damage. Nullptr indicated environmental damage.
 * @param DamageTaken The amount of damage that is being applied.
 * @return The new value of the components stamina value
 */
float UVitalityWelfareComponent::DamageStamina(AActor* DamageInstigator, float DamageTaken)
{
	const float OldValue = _StaminaCurrent;
	const float NewDamageValue = abs(DamageTaken);
	if (_StaminaMax > 0.f)
	{
		_StaminaCurrent -= NewDamageValue;
		if (_HealthCurrent <= 0.f && !GetIsDead())
			_IsDead = true;
		OnStaminaUpdated.Broadcast(OldValue, _StaminaCurrent);
	}
	return 0.f;
}

/**
 * @brief Damages the components magic value, performing internal logic and firing delegates.
 * @param DamageInstigator The actor who applied the damage. Nullptr indicated environmental damage.
 * @param DamageTaken The amount of damage that is being applied.
 * @return The new value of the components magic value
 */
float UVitalityWelfareComponent::DamageMagic(AActor* DamageInstigator, float DamageTaken)
{
	const float OldValue = _MagicCurrent;
	const float NewDamageValue = abs(DamageTaken);
	if (_MagicMax > 0.f)
	{
		_MagicCurrent -= NewDamageValue;
		if (_MagicCurrent <= 0.f && !GetIsDead())
			_IsDead = true;
		OnMagicUpdated.Broadcast(OldValue, _StaminaCurrent);
	}
	return 0.f;
}

/**
 * @return Returns the current health, as a percentage from 0.0 to 1.0
 */
float UVitalityWelfareComponent::GetHealthPercent() const
{
	if (!GetIsDead())
		return FMath::Clamp(_HealthCurrent/_HealthMax, 0.f, 1.f);
	return 0.f;
}

/**
 * @param CurrentValue Pass by Reference for retrieving the CURRENT health
 * @param MaxValue Pass by reference for retrieving the MAX health
 * @return Current health as a percentage from 0.0 to 1.0
 */
float UVitalityWelfareComponent::GetCurrentHealth(float& CurrentValue, float& MaxValue) const
{
	CurrentValue = _HealthCurrent;
	MaxValue     = _HealthMax;
	return GetHealthPercent();
}

/**
 * @return Current stamina, as a percentage from 0.0 to 1.0
 */
float UVitalityWelfareComponent::GetStaminaPercent() const
{
	if (_StaminaMax > 0.f)
		return FMath::Clamp(_StaminaCurrent/_StaminaMax, 0.f, 1.f);
	return 0.f;
}

/**
 * @param CurrentValue Pass by Reference for retrieving the CURRENT stamina
 * @param MaxValue Pass by reference for retrieving the MAX stamina
 * @return Current stamina, as a percentage from 0.0 to 1.0
 */
float UVitalityWelfareComponent::GetCurrentStamina(float& CurrentValue, float& MaxValue) const
{
	CurrentValue = _StaminaCurrent;
	MaxValue     = _StaminaMax;
	return GetStaminaPercent();
}
/**
 * @return Returns the current magic, as a percentage from 0.0 to 1.0
 */
float UVitalityWelfareComponent::GetMagicPercent() const
{
	if (_MagicMax > 0.f)
		return FMath::Clamp(_MagicCurrent/_MagicMax, 0.f, 1.f);
	return 0.f;
}

/**
 * @brief Returns the current magic as a percentage from 0.0 to 1.0
 * @param CurrentValue Pass by Reference for retrieving the CURRENT magic
 * @param MaxValue Pass by reference for retrieving the MAX magic
 * @return 
 */
float UVitalityWelfareComponent::GetCurrentMagic(float& CurrentValue, float& MaxValue) const
{
	CurrentValue = _MagicCurrent;
	MaxValue     = _MagicMax;
	return GetMagicPercent();
}

/**
 * @brief Gets the current percentage of the actors hydration from 0.0 to 1.0
 * @return Returns the current hydration, as a percentage
 */
float UVitalityWelfareComponent::GetHydrationPercent() const
{
	if (_HydrationMax > 0.f)
		return FMath::Clamp(_HydrationCurrent/_HydrationMax, 0.f, 1.f);
	return 0.f;
}

/**
 * @brief Returns the current hydration as a percentage from 0.0 to 1.0
 * @param CurrentValue Pass by Reference for retrieving the CURRENT hydration
 * @param MaxValue Pass by reference for retrieving the MAX hydration
 * @return 
 */
float UVitalityWelfareComponent::GetCurrentHydration(float& CurrentValue, float& MaxValue) const
{
	CurrentValue = _HydrationCurrent;
	MaxValue     = _HydrationMax;
	return GetHydrationPercent();
}

/**
 * @brief Gets the current percentage of the actors hunger from 0.0 to 1.0
 * @return Returns the current hunger, as a percentage
 */
float UVitalityWelfareComponent::GetHungerPercent() const
{
	if (_CaloriesMax > 0.f)
		return FMath::Clamp(_CaloriesCurrent/_CaloriesMax, 0.f, 1.f);
	return 0.f;
}

/**
 * @brief Returns the current hunger as a percentage from 0.0 to 1.0
 * @param CurrentValue Pass by Reference for retrieving the CURRENT hunger
 * @param MaxValue Pass by reference for retrieving the MAX hunger
 * @return 
 */
float UVitalityWelfareComponent::GetCurrentHunger(float& CurrentValue, float& MaxValue) const
{
	CurrentValue = _CaloriesCurrent;
	MaxValue     = _CaloriesMax;
	return GetHungerPercent();
}

/**
 * @brief Increases the Combat State properly, triggering delegates
 */
void UVitalityWelfareComponent::IncreaseCombatState()
{
	if (GetCombatState() == ECombatState::ENGAGED)
		return;
	const ECombatState OldState = _CombatState;
	switch (_CombatState)
	{
	case ECombatState::RECOVERY: _CombatState = ECombatState::RELAXED; break;
	case ECombatState::RELAXED:  _CombatState = ECombatState::ALERT; break;
	case ECombatState::INJURED:  _CombatState = ECombatState::ALERT; break;
	case ECombatState::ALERT:    _CombatState = ECombatState::ENGAGED; break;
	default: // Status is already Engaged or None (highest)
		return;
	}
	OnCombatStateChanged.Broadcast(OldState, _CombatState);
}

/**
 * @brief Decreases the Combat State properly, triggering delegates
 */
void UVitalityWelfareComponent::DecreaseCombatState()
{
	if (GetCombatState() == ECombatState::RELAXED)
		return;
	const ECombatState OldState = _CombatState;
	switch (_CombatState)
	{
	case ECombatState::ENGAGED:  _CombatState = ECombatState::ALERT; break;
	case ECombatState::ALERT:    _CombatState = ECombatState::RELAXED; break;
	case ECombatState::INJURED:  _CombatState = ECombatState::RELAXED; break;
	default:
		return;
	}
	OnCombatStateChanged.Broadcast(OldState, _CombatState);
}

// Resets the Combat State to relaxed, terminating the combat timer
void UVitalityWelfareComponent::ResetCombatState()
{
	if (GetCombatState() == ECombatState::RELAXED)
		return;
	
	if (_CombatTimer.IsValid())
		_CombatTimer.Invalidate();
	
	const ECombatState OldState = _CombatState;
	_CombatState = ECombatState::RELAXED;
	OnCombatStateChanged.Broadcast(OldState,
		ECombatState::RELAXED);
}

// Forces the combat state to be in an engaged state
void UVitalityWelfareComponent::SetCombatEngaged()
{
	
	if (GetCombatState() == ECombatState::ENGAGED)
		return;
	const ECombatState OldState = _CombatState;
	_CombatState = ECombatState::ENGAGED;
	OnCombatStateChanged.Broadcast(OldState,
		ECombatState::ENGAGED);
}

void UVitalityWelfareComponent::BeginPlay()
{
	Super::BeginPlay();
	ReloadSettings();
}

void UVitalityWelfareComponent::InitializeTimer(FTimerHandle& TimerHandle,
		FTimerDelegate TimerDelegate, float TickRate)
{
	if (GetWorld()->GetTimerManager().TimerExists(TimerHandle))
		TimerHandle.Invalidate();

	const float managerTickRate = TickRate <= 0.f ? 1.f : TickRate;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle,	TimerDelegate,
		1 * managerTickRate, true);
}
