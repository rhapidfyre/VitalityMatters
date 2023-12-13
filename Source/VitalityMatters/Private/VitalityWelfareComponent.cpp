// Copyright Take Five Games, LLC 2023 - All Rights Reserved

#include "VitalityWelfareComponent.h"

#include "AsyncTreeDifferences.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void UVitalityWelfareComponent::SetupDefaultValues()
{
	DamageHistory_.Empty();
	HealthCurrent_		= StartingHealthCurrent;
	HealthMax_			= StartingHealthCurrent;
	StaminaCurrent_		= StartingStaminaCurrent;
	StaminaMax_			= StartingStaminaCurrent;
	MagicCurrent_		= StartingMagicCurrent;
	MagicMax_			= StartingMagicCurrent;
	CaloriesCurrent_	= StartingHungerCurrent;
	CaloriesMax_		= StartingHungerCurrent;
	HydrationCurrent_	= StartingHydrationCurrent;
	HydrationMax_		= StartingHydrationCurrent;
	
	HealthRegenAtRest_		= PassiveHealthRegen;
	MagicRegenAtRest_		= PassiveMagicRegen;
	StaminaRegenAtRest_		= PassiveStaminaRegen;
	CaloriesDrainAtRest_	= PassiveHungerDrain;
	HydrationDrainAtRest_	= PassiveHydrationDrain;

	HealthTimerTickRate_		= HealthTimerTickRate;
	StaminaTimerTickRate_		= StaminaTimerTickRate;
	MagicTimerTickRate_			= MagicTimerTickRate;
	HydrationTimerTickRate_		= HydrationTimerTickRate;
	HungerTimerTickRate_		= HungerTimerTickRate_;
	
	CombatState_ = ECombatState::RELAXED;
}

/**
 * @param DamageInstigator The actor who applied the damage. Nullptr indicated environmental damage.
 * @brief Damages the components health value, performing internal logic and firing delegates.
 * @param DamageTaken The amount of damage that is being applied.
 * @return The new value of the components current health value
 */
float UVitalityWelfareComponent::DamageHealth(AActor* DamageInstigator, float DamageTaken)
{
	if (!GetOwner()->HasAuthority())
		return HealthCurrent_;
	
	const float NewDamageValue = abs(DamageTaken);
	if (!FMath::IsNearlyZero(NewDamageValue))
	{
		if (!GetIsDead())
		{
			bool IsNewDamage = true;
			for (FStDamageData DamageData : DamageHistory_)
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
				DamageHistory_.Add(FStDamageData(DamageInstigator, NewDamageValue));

			HealthCurrent_ -= NewDamageValue;
			Multicast_DamageTaken(DamageInstigator, NewDamageValue);
			
			if (HealthCurrent_ <= 0.f)
			{
				if (!GetIsDead())
				{
					IsDead_ = true;
					OnDeath.Broadcast(DamageInstigator);
					
					UAnimMontage* UsingAnimation = nullptr;
					const int NumAnimations = HitAnimations.Num();
					if (HitAnimations.IsValidIndex(0) && NumAnimations > 0)
						UsingAnimation = DeathAnimations[FMath::RandRange(0,NumAnimations-1)];
					
					USoundBase* UsingSound = nullptr;
					const int NumSounds		= HitSounds.Num();
					if (HitSounds.IsValidIndex(0) && NumSounds > 0)
						UsingSound = DeathSounds[FMath::RandRange(0,NumSounds-1)];
					
					Multicast_VitalityDeath(DamageInstigator, UsingAnimation, UsingSound);
				}
			}
			else
			{
				HitByWeapon();
			}
			
		}
	}
	return HealthCurrent_;
}

/**
 * @brief Damages the components stamina value, performing internal logic and firing delegates.
 * @param DamageInstigator The actor who applied the damage. Nullptr indicated environmental damage.
 * @param DamageTaken The amount of damage that is being applied.
 * @return The new value of the components stamina value
 */
float UVitalityWelfareComponent::DamageStamina(AActor* DamageInstigator, float DamageTaken)
{
	if (!GetOwner()->HasAuthority())
		return StaminaCurrent_;
	const float NewDamageValue = abs(DamageTaken);
	if (StaminaMax_ > 0.f)
	{
		StaminaCurrent_ -= NewDamageValue;
		if (StaminaCurrent_ < 0.f)
			StaminaCurrent_ = 0.f;
		OnStaminaUpdated.Broadcast(StaminaCurrent_, StaminaMax_, GetStaminaPercent());
	}
	return StaminaCurrent_;
}

/**
 * @brief Damages the components magic value, performing internal logic and firing delegates.
 * @param DamageInstigator The actor who applied the damage. Nullptr indicated environmental damage.
 * @param DamageTaken The amount of damage that is being applied.
 * @return The new value of the components magic value
 */
float UVitalityWelfareComponent::DamageMagic(AActor* DamageInstigator, float DamageTaken)
{
	if (!GetOwner()->HasAuthority())
		return MagicCurrent_;
	const float NewDamageValue = abs(DamageTaken);
	if (MagicMax_ > 0.f)
	{
		MagicCurrent_ -= NewDamageValue;
		if (MagicCurrent_ < 0.f)
			MagicCurrent_ = 0.f;
		OnMagicUpdated.Broadcast(MagicCurrent_, MagicMax_, GetMagicPercent());
	}
	return MagicCurrent_;
}

bool UVitalityWelfareComponent::StartTimerForCategory(EVitalityCategory VitalityCategory)
{
	FTimerHandle* TimerReference = &HealthTimer_;
	FTimerDelegate TimerDelegate;
	float TimerTickRate = 1.f;
	
	switch(VitalityCategory)
	{
	case EVitalityCategory::STAMINA:
		TimerReference = &StaminaTimer_;
		TimerDelegate.BindUObject(this, &UVitalityWelfareComponent::TickStamina);
		TimerTickRate = StaminaTimerTickRate_;
		break;
	case EVitalityCategory::MAGIC:
		TimerReference = &MagicTimer_;
		TimerDelegate.BindUObject(this, &UVitalityWelfareComponent::TickMagic);
		TimerTickRate = MagicTimerTickRate_;
		break;
	case EVitalityCategory::THIRST:
		TimerReference = &HydrationTimer_;
		TimerDelegate.BindUObject(this, &UVitalityWelfareComponent::TickHydration);
		TimerTickRate = HydrationTimerTickRate_;
		break;
	case EVitalityCategory::HUNGER:
		TimerReference = &CaloriesTimer_;
		TimerDelegate.BindUObject(this, &UVitalityWelfareComponent::TickCalories);
		TimerTickRate = HungerTimerTickRate_;
		break;
	default:
		TimerDelegate.BindUObject(this, &UVitalityWelfareComponent::TickHealth);
		break;
	}
	
	if (TimerReference != nullptr)
		InitializeTimer(*TimerReference, TimerDelegate, TimerTickRate);
	
	return true;
}

bool UVitalityWelfareComponent::CancelTimerForCategory(EVitalityCategory VitalityCategory)
{
	FTimerHandle* TimerReference = &HealthTimer_;
	switch(VitalityCategory)
	{
	case EVitalityCategory::STAMINA:	TimerReference = &StaminaTimer_;	break;
	case EVitalityCategory::MAGIC:		TimerReference = &MagicTimer_;		break;
	case EVitalityCategory::THIRST:		TimerReference = &HydrationTimer_;	break;
	case EVitalityCategory::HUNGER:		TimerReference = &CaloriesTimer_;	break;
	default:
		break;
	}
	CancelTimer(*TimerReference);
	return true;
}

bool UVitalityWelfareComponent::PauseTimerForCategory(EVitalityCategory VitalityCategory, bool PauseTimer)
{
	const FTimerHandle* TimerReference = &HealthTimer_;
	switch(VitalityCategory)
	{
	case EVitalityCategory::STAMINA:	TimerReference = &StaminaTimer_;	break;
	case EVitalityCategory::MAGIC:		TimerReference = &MagicTimer_;		break;
	case EVitalityCategory::THIRST:		TimerReference = &HydrationTimer_;	break;
	case EVitalityCategory::HUNGER:		TimerReference = &CaloriesTimer_;	break;
	default:
		break;
	}
	if (PauseTimer)
		GetWorld()->GetTimerManager().PauseTimer(*TimerReference);
	else
		GetWorld()->GetTimerManager().UnPauseTimer(*TimerReference);
	return true;
}

float UVitalityWelfareComponent::GetVitalityStatData(
	EVitalityCategory VitalityCategory, float& CurrentValue, float& MaxValue) const
{
	switch(VitalityCategory)
	{
	case EVitalityCategory::HEALTH:
		return GetCurrentHealth(CurrentValue, MaxValue);
	case EVitalityCategory::MAGIC:
		return GetCurrentMagic(CurrentValue, MaxValue);
	case EVitalityCategory::STAMINA:
		return GetCurrentStamina(CurrentValue, MaxValue);
	case EVitalityCategory::HUNGER:
		return GetCurrentHunger(CurrentValue, MaxValue);
	case EVitalityCategory::THIRST:
		return GetCurrentHydration(CurrentValue, MaxValue);
	default:
		break;
	}
	CurrentValue = 0.f;
	MaxValue     = 0.f;
	return 0;
}

/**
 * @return Returns the current health, as a percentage from 0.0 to 1.0
 */
float UVitalityWelfareComponent::GetHealthPercent() const
{
	if (!GetIsDead())
		return FMath::Clamp(HealthCurrent_/HealthMax_, 0.f, 1.f);
	return 0.f;
}

/**
 * @param CurrentValue Pass by Reference for retrieving the CURRENT health
 * @param MaxValue Pass by reference for retrieving the MAX health
 * @return Current health as a percentage from 0.0 to 1.0
 */
float UVitalityWelfareComponent::GetCurrentHealth(float& CurrentValue, float& MaxValue) const
{
	CurrentValue = HealthCurrent_;
	MaxValue     = HealthMax_;
	return GetHealthPercent();
}

/**
 * @return Current stamina, as a percentage from 0.0 to 1.0
 */
float UVitalityWelfareComponent::GetStaminaPercent() const
{
	if (StaminaMax_ > 0.f)
		return FMath::Clamp(StaminaCurrent_/StaminaMax_, 0.f, 1.f);
	return 0.f;
}

/**
 * @param CurrentValue Pass by Reference for retrieving the CURRENT stamina
 * @param MaxValue Pass by reference for retrieving the MAX stamina
 * @return Current stamina, as a percentage from 0.0 to 1.0
 */
float UVitalityWelfareComponent::GetCurrentStamina(float& CurrentValue, float& MaxValue) const
{
	CurrentValue = StaminaCurrent_;
	MaxValue     = StaminaMax_;
	return GetStaminaPercent();
}
/**
 * @return Returns the current magic, as a percentage from 0.0 to 1.0
 */
float UVitalityWelfareComponent::GetMagicPercent() const
{
	if (MagicMax_ > 0.f)
		return FMath::Clamp(MagicCurrent_/MagicMax_, 0.f, 1.f);
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
	CurrentValue = MagicCurrent_;
	MaxValue     = MagicMax_;
	return GetMagicPercent();
}

/**
 * @brief Gets the current percentage of the actors hydration from 0.0 to 1.0
 * @return Returns the current hydration, as a percentage
 */
float UVitalityWelfareComponent::GetHydrationPercent() const
{
	if (HydrationMax_ > 0.f)
		return FMath::Clamp(HydrationCurrent_/HydrationMax_, 0.f, 1.f);
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
	CurrentValue = HydrationCurrent_;
	MaxValue     = HydrationMax_;
	return GetHydrationPercent();
}

/**
 * @brief Gets the current percentage of the actors hunger from 0.0 to 1.0
 * @return Returns the current hunger, as a percentage
 */
float UVitalityWelfareComponent::GetHungerPercent() const
{
	if (CaloriesMax_ > 0.f)
		return FMath::Clamp(CaloriesCurrent_/CaloriesMax_, 0.f, 1.f);
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
	CurrentValue = CaloriesCurrent_;
	MaxValue     = CaloriesMax_;
	return GetHungerPercent();
}

// Determines the animation & sound to be played, then uses multicast to send it
void UVitalityWelfareComponent::HitByWeapon()
{
	// The anim & sound needs to be sent by the server, so it's synchronized
	UAnimMontage* UsingAnimation = nullptr;
	const int NumAnimations = HitAnimations.Num();
	if (HitAnimations.IsValidIndex(0) && NumAnimations > 0)
		UsingAnimation = HitAnimations[FMath::RandRange(0,NumAnimations-1)];
	
	USoundBase* UsingSound = nullptr;
	const int NumSounds		= HitSounds.Num();
	if (HitSounds.IsValidIndex(0) && NumSounds > 0)
		UsingSound = HitSounds[FMath::RandRange(0,NumSounds-1)];
	
	Multicast_HitByWeaponEffects(UsingAnimation, UsingSound);
}

/** Sent to all users when an actor gets hit by a weapon
 * @param HitByWeaponAnim The animation to play when executed
 * @param HitByWeaponSound The sound to play when executed
 */
void UVitalityWelfareComponent::Multicast_HitByWeaponEffects_Implementation(UAnimMontage* HitByWeaponAnim,
	USoundBase* HitByWeaponSound)
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitByWeaponSound,
		GetOwner()->GetActorLocation(), FRotator(), 1.0, 1.0, 0.f);
	Cast<ACharacter>(GetOwner())->PlayAnimMontage(HitByWeaponAnim);
	OnHitAnimation.Broadcast(HitByWeaponAnim, HitByWeaponSound);
}

/**
 * @brief Increases the Combat State properly, triggering delegates
 */
void UVitalityWelfareComponent::IncreaseCombatState()
{
	if (GetCombatState() == ECombatState::ENGAGED)
		return;
	const ECombatState OldState = CombatState_;
	switch (CombatState_)
	{
	case ECombatState::RECOVERY: CombatState_ = ECombatState::RELAXED; break;
	case ECombatState::RELAXED:  CombatState_ = ECombatState::ALERT; break;
	case ECombatState::INJURED:  CombatState_ = ECombatState::ALERT; break;
	case ECombatState::ALERT:    CombatState_ = ECombatState::ENGAGED; break;
	default: // Status is already Engaged or None (highest)
		return;
	}
	OnCombatStateChanged.Broadcast(OldState, CombatState_);
}

/**
 * @brief Decreases the Combat State properly, triggering delegates
 */
void UVitalityWelfareComponent::DecreaseCombatState()
{
	if (GetCombatState() == ECombatState::RELAXED)
		return;
	const ECombatState OldState = CombatState_;
	switch (CombatState_)
	{
	case ECombatState::ENGAGED:  CombatState_ = ECombatState::ALERT; break;
	case ECombatState::ALERT:    CombatState_ = ECombatState::RELAXED; break;
	case ECombatState::INJURED:  CombatState_ = ECombatState::RELAXED; break;
	default:
		return;
	}
	OnCombatStateChanged.Broadcast(OldState, CombatState_);
}

// Resets the Combat State to relaxed, terminating the combat timer
void UVitalityWelfareComponent::ResetCombatState()
{
	if (GetCombatState() == ECombatState::RELAXED)
		return;
	
	if (CombatTimer_.IsValid())
		CombatTimer_.Invalidate();
	
	const ECombatState OldState = CombatState_;
	CombatState_ = ECombatState::RELAXED;
	OnCombatStateChanged.Broadcast(OldState,
		ECombatState::RELAXED);
}

// Forces the combat state to be in an engaged state
void UVitalityWelfareComponent::SetCombatEngaged()
{
	if (GetCombatState() == ECombatState::ENGAGED)
		return;
	const ECombatState OldState = CombatState_;
	CombatState_ = ECombatState::ENGAGED;
	OnCombatStateChanged.Broadcast(OldState,
		ECombatState::ENGAGED);
}

void UVitalityWelfareComponent::SetCombatAlert()
{
	const ECombatState OldState = GetCombatState();
	if (OldState == ECombatState::ENGAGED || OldState == ECombatState::ALERT)
		return;
	SetCombatState(ECombatState::ENGAGED);
}

void UVitalityWelfareComponent::InitializeHealthSubsystem(bool UseSubsystem, float NowValue, float MaxValue,
	float RegenRate)
{
	Server_InitializeHealthSubsystem(UseSubsystem, NowValue, MaxValue, RegenRate);
}

void UVitalityWelfareComponent::InitializeStaminaSubsystem(bool UseSubsystem, float NowValue, float MaxValue,
	float RegenRate)
{
	Server_InitializeStaminaSubsystem(UseSubsystem, NowValue, MaxValue, RegenRate);
}

void UVitalityWelfareComponent::InitializeMagicSubsystem(bool UseSubsystem, float NowValue, float MaxValue,
	float RegenRate)
{
	Server_InitializeMagicSubsystem(UseSubsystem, NowValue, MaxValue, RegenRate);
}

void UVitalityWelfareComponent::InitializeSurvivalSubsystem(bool UseSubsystem, float NowHydrationValue,
	float MaxHydrationValue, float HydrationRegenRate, float NowCaloriesValue, float MaxCaloriesValue,
	float CaloriesRegenRate)
{
	if (!GetOwner()->HasAuthority())
	{
		Server_InitializeSurvivalSubsystem(UseSubsystem,
			NowHydrationValue, MaxHydrationValue, HydrationRegenRate,
			NowCaloriesValue, NowCaloriesValue, CaloriesRegenRate);
	}
}

void UVitalityWelfareComponent::InitializeSubsystem(EVitalityCategory VitalityCategory,
	bool UseSubsystem, float NowValue, float MaxValue, float RegenRate)
{
	if (GetOwner()->HasAuthority())
	{
		bool* UseSubsystemPtr  			= &UseHealthSubsystem;
		float* ActualCurrentPtr			= &HealthCurrent_;
		float* CurrentValuePtr 			= &StartingHealthCurrent;
		float* ActualMaximumPtr			= &HealthMax_;
		float* MaximumValuePtr 			= &StartingHealthMaximum;
		float* ActualRegenPtr			= &HealthRegenAtRest_;
		float* RegenValuePtr   			= &PassiveHealthRegen;
		float* ActualTickRatePtr		= &HealthTimerTickRate_;
		float* TimerTickRatePtr			= &HealthTimerTickRate;
		FTimerHandle* SubsystemTimer	= &HealthTimer_;
		FTimerDelegate InitDelegate;

		switch(VitalityCategory)
		{
		case EVitalityCategory::HEALTH:
			InitDelegate.BindUObject(this, &UVitalityWelfareComponent::TickHealth);
			break;
		case EVitalityCategory::STAMINA:
			InitDelegate.BindUObject(this, &UVitalityWelfareComponent::TickStamina);
			UseSubsystemPtr		= &UseStaminaSubsystem;
			ActualCurrentPtr	= &StaminaCurrent_;
			CurrentValuePtr		= &StartingStaminaCurrent;
			ActualMaximumPtr	= &StaminaMax_;
			MaximumValuePtr		= &StartingStaminaMaximum;
			ActualRegenPtr		= &StaminaRegenAtRest_;
			RegenValuePtr		= &PassiveStaminaRegen;
			ActualTickRatePtr	= &HealthTimerTickRate_;
			TimerTickRatePtr	= &StaminaTimerTickRate;
			SubsystemTimer		= &StaminaTimer_;
			break;
		case EVitalityCategory::MAGIC:
			InitDelegate.BindUObject(this, &UVitalityWelfareComponent::TickMagic);
			UseSubsystemPtr		= &UseMagicSubsystem;
			ActualCurrentPtr	= &MagicCurrent_;
			CurrentValuePtr		= &StartingMagicCurrent;
			ActualMaximumPtr	= &MagicMax_;
			MaximumValuePtr		= &StartingMagicMaximum;
			ActualRegenPtr		= &MagicRegenAtRest_;
			RegenValuePtr		= &PassiveMagicRegen;
			ActualTickRatePtr	= &MagicTimerTickRate_;
			TimerTickRatePtr	= &MagicTimerTickRate;
			SubsystemTimer		= &MagicTimer_;
			break;
		default:
			return;
		}
		
		// Set initial values
		if (UseSubsystemPtr != nullptr) *UseSubsystemPtr	= UseSubsystem;
		if (CurrentValuePtr != nullptr) *CurrentValuePtr	= NowValue;
		if (MaximumValuePtr != nullptr) *MaximumValuePtr	= MaxValue;
		if (RegenValuePtr != nullptr)	*RegenValuePtr		= RegenRate;
		
		if (SubsystemTimer->IsValid())
			SubsystemTimer->Invalidate();
		
		if (*UseSubsystemPtr)
		{
			const bool CurrentValueIsValid	= *CurrentValuePtr  > 0.f;
			const bool MaximumValueIsValid	= *MaximumValuePtr  > 0.f; 
			const bool RegenValueIsValid	= *RegenValuePtr	> 0.f;
			const bool TimerTickRateIsValid	= *TimerTickRatePtr	> 0.f;
			
			*ActualCurrentPtr	= CurrentValueIsValid	? *CurrentValuePtr	: 1.f;
			*ActualMaximumPtr	= MaximumValueIsValid	? *MaximumValuePtr	: 1.f;
			*ActualRegenPtr		= RegenValueIsValid		? *ActualRegenPtr	: 1.f;
			*ActualTickRatePtr	= TimerTickRateIsValid	? *TimerTickRatePtr	: 0.5;

			// If the value isn't max, start the regen timer
			if (*ActualCurrentPtr < *ActualMaximumPtr)
			{
				InitializeTimer(*SubsystemTimer, InitDelegate);
				if (VitalityCategory == EVitalityCategory::HEALTH)
				{
					if (HealthCurrent_ <= 0.f && !GetIsDead())
					{
						IsDead_ = true;
						OnDeath.Broadcast(nullptr);
					}
				}
			}
		}
		else
		{
			*ActualCurrentPtr	= 0.f;
			*ActualMaximumPtr	= 0.f;
			*ActualRegenPtr		= 0.f;
			*ActualTickRatePtr	= 0.5;
		}
	}
}


void UVitalityWelfareComponent::Server_InitializeHealthSubsystem_Implementation(bool UseSubsystem, float NowValue,
																				float MaxValue, float RegenRate)
{
	InitializeSubsystem(EVitalityCategory::HEALTH, UseSubsystem, NowValue, MaxValue, RegenRate);
}

void UVitalityWelfareComponent::Server_InitializeStaminaSubsystem_Implementation(bool UseSubsystem, float NowValue,
                                                                                 float MaxValue, float RegenRate)
{
	InitializeSubsystem(EVitalityCategory::STAMINA, UseSubsystem, NowValue, MaxValue, RegenRate);
}

void UVitalityWelfareComponent::Server_InitializeMagicSubsystem_Implementation(bool UseSubsystem, float NowValue,
	float MaxValue, float RegenRate)
{
	InitializeSubsystem(EVitalityCategory::MAGIC, UseSubsystem, NowValue, MaxValue, RegenRate);
}

void UVitalityWelfareComponent::Server_InitializeSurvivalSubsystem_Implementation(bool UseSubsystem,
	float NowHydrationValue, float MaxHydrationValue, float HydrationRegenRate, float NowCaloriesValue,
	float MaxCaloriesValue, float CaloriesRegenRate)
{
	if (UseSubsystem)
	{
		HydrationMax_			= StartingMagicMaximum		> 0.f	? StartingMagicMaximum		: 1.f;
		HydrationCurrent_		= StartingMagicCurrent		> 0.f	? StartingMagicCurrent		: 1.f;
		HydrationDrainAtRest_	= PassiveHydrationDrain		> 0.f	? PassiveHydrationDrain		: 0.082;
		HydrationTimerTickRate_	= HydrationTimerTickRate	> 0.f	? HydrationTimerTickRate	: 0.5;

		if (HydrationCurrent_ > 0.f)
		{
			FTimerDelegate InitDelegate;
			InitDelegate.BindUObject(this, &UVitalityWelfareComponent::TickHydration);
			InitializeTimer(HydrationTimer_, InitDelegate);
		}
		
		CaloriesMax_			= StartingHungerMaximum	> 0.f	? StartingHungerMaximum	: 1.f;
		CaloriesCurrent_		= StartingHungerCurrent	> 0.f	? StartingHungerCurrent	: 1.f;
		CaloriesDrainAtRest_	= PassiveHungerDrain	> 0.f	? PassiveHungerDrain	: 0.082;
		HungerTimerTickRate_	= CaloriesTimerTickRate	> 0.f	? CaloriesTimerTickRate	: 0.5;

		if (CaloriesCurrent_ > 0.f)
		{
			FTimerDelegate InitDelegate;
			InitDelegate.BindUObject(this, &UVitalityWelfareComponent::TickCalories);
			InitializeTimer(CaloriesTimer_, InitDelegate);
		}
		
	}
	else
	{
		HydrationMax_			= 0.f;	CaloriesMax_			= 0.f;
		HydrationCurrent_		= 0.f;	CaloriesCurrent_		= 0.f;
		HydrationDrainAtRest_	= 0.f;	CaloriesDrainAtRest_	= 0.f;
		HydrationTimerTickRate_	= 0.5;	HungerTimerTickRate_	= 0.5;
	}
}

void UVitalityWelfareComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UVitalityWelfareComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UVitalityWelfareComponent, DamageHistory_);
	DOREPLIFETIME(UVitalityWelfareComponent, IsDead_);
	DOREPLIFETIME(UVitalityWelfareComponent, CombatState_);
	
	DOREPLIFETIME(UVitalityWelfareComponent, HealthCurrent_);
	DOREPLIFETIME(UVitalityWelfareComponent, HealthMax_);
	
	DOREPLIFETIME(UVitalityWelfareComponent, MagicCurrent_);
	DOREPLIFETIME(UVitalityWelfareComponent, MagicMax_);
	
	DOREPLIFETIME(UVitalityWelfareComponent, StaminaCurrent_);
	DOREPLIFETIME(UVitalityWelfareComponent, StaminaMax_);
	
	DOREPLIFETIME(UVitalityWelfareComponent, CaloriesCurrent_);
	DOREPLIFETIME(UVitalityWelfareComponent, CaloriesMax_);
	
	DOREPLIFETIME(UVitalityWelfareComponent, HydrationCurrent_);
	DOREPLIFETIME(UVitalityWelfareComponent, HydrationMax_);
}

/**
 * @brief Helper Function used to set the various timers in this class
 * @param TimerHandle The timer handle to be modified
 * @param TimerDelegate The delegate for timer functionality
 * @param TickRate The tick rate for the timer
 */
void UVitalityWelfareComponent::InitializeTimer(FTimerHandle& TimerHandle,
		FTimerDelegate TimerDelegate, float TickRate) const
{
	if (GetWorld()->GetTimerManager().TimerExists(TimerHandle))
		TimerHandle.Invalidate();

	const float managerTickRate = TickRate <= 0.f ? 1.f : TickRate;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle,	TimerDelegate,
		1 * managerTickRate, true);
}

/**
 * @brief Cancels and invalidates the requested timer, if it is valid
 * @param TimerHandle The timer handle to be canceled and invalidated
 */
void UVitalityWelfareComponent::CancelTimer(FTimerHandle& TimerHandle) const
{
	if (GetWorld()->GetTimerManager().TimerExists(TimerHandle))
	{
		if (TimerHandle.IsValid())
			TimerHandle.Invalidate();
	}
}

void UVitalityWelfareComponent::TickStamina()
{
	// If stamina is fully regenerated, kill the timer. It's not needed anymore.
	if (StaminaCurrent_ >= StaminaMax_)
	{
		CancelTimer(StaminaTimer_);
		StaminaCurrent_ = StaminaMax_;
	}
	else
		StaminaCurrent_ += StaminaRegenAtRest_;
}

void UVitalityWelfareComponent::TickHealth()
{
	if (HealthCurrent_ > 0.f)
	{
		if (HealthCurrent_ < HealthMax_)
		{
			// Current calories percent
			const float HungerPercent = GetHungerPercent();

			// If hunger is above 40%, allow full regeneration
			if (HungerPercent >= 0.4)
			{
				HealthCurrent_ += HealthRegenAtRest_;	
			}
			// If it's below 40%, cap regen to hunger value
			else
			{
				// Health can regen up to twice the percentage of calories
				if ( ((HealthCurrent_/HealthMax_)*0.4) < HungerPercent )
				{
					HealthCurrent_ += HealthRegenAtRest_;
				}
			}
		}
		else if (HealthCurrent_ > HealthMax_)
		{
			CancelTimer(HealthTimer_);
			HealthCurrent_ = HealthMax_;
		}
	}
}

void UVitalityWelfareComponent::TickMagic()
{
}

void UVitalityWelfareComponent::TickCalories()
{
	if (CaloriesCurrent_ > 0.f)
	{
		CaloriesCurrent_ -= CaloriesDrainAtRest_;
		if (CaloriesCurrent_ <= 0.f)
		{
			CancelTimer(CaloriesTimer_);
			CaloriesCurrent_ = 0.f;
		}
	}
}

void UVitalityWelfareComponent::TickHydration()
{
	if (HydrationCurrent_ > 0.f)
	{
		HydrationCurrent_ -= HydrationDrainAtRest_;
		if (HydrationCurrent_ <= 0.f)
		{
			HydrationCurrent_ = 0.f;
			CancelTimer(HydrationTimer_);
		}
	}
}

void UVitalityWelfareComponent::SetCombatState(ECombatState CombatState)
{
	const ECombatState OldCombatState = GetCombatState();
	float CombatTimer = 0.f;
		
	// Everytime hostile action is taken, reset the timer
	if (CombatState == ECombatState::ENGAGED || CombatState == ECombatState::ALERT)
	{
		if (CombatTimer_.IsValid())
			CombatTimer_.Invalidate();
		CombatTimer = 10.f;
	}
		
	if (GetCombatState() != CombatState)
	{
		CombatState_ = CombatState;
		if (CombatState != ECombatState::ENGAGED)
		{
			// Player has either acquired or sustained alertness
			if (CombatState == ECombatState::ALERT)
			{
				CombatTimer = 3.f;
			}
			// Player is now engaged in combat
			else if (CombatState == ECombatState::ENGAGED)
			{
				CombatTimer = 10.f;
			}
		}
		else
		{
			// Player has left combat and is coming down from engagement
			if (CombatState == ECombatState::ALERT)
			{
				CombatTimer = 10.f;
			}
		}
		OnCombatStateChanged.Broadcast(OldCombatState, GetCombatState());
			
	}

	if (CombatTimer > 0.f)
	{
		if (CombatTimer_.IsValid())
			CombatTimer_.Invalidate();
			
		GetWorld()->GetTimerManager().SetTimer(CombatTimer_, this,
			&UVitalityWelfareComponent::DecreaseCombatState, CombatTimer, false);
	}
	
}

void UVitalityWelfareComponent::OnRep_IsDeadChanged_Implementation(bool WasDeadBefore)
{
	if (!WasDeadBefore)
	{
		OnDeath.Broadcast(nullptr);
	}
}

void UVitalityWelfareComponent::OnRep_CombatStateChanged_Implementation(ECombatState OldCombatState)
{
	OnCombatStateChanged.Broadcast(OldCombatState, CombatState_);
}

void UVitalityWelfareComponent::OnRep_HealthValueChanged_Implementation(float OldValue)
{
	float CurrentValue = 0, MaximumValue = 0;
	const float ValueAsPercent = GetCurrentHealth(CurrentValue, MaximumValue);
	OnHealthUpdated.Broadcast(HealthCurrent_, HealthMax_, ValueAsPercent);
}
void UVitalityWelfareComponent::OnRep_HealthMaxChanged_Implementation(float OldValue)
{
	float CurrentValue = 0, MaximumValue = 0;
	const float ValueAsPercent = GetCurrentHealth(CurrentValue, MaximumValue);
	OnHealthUpdated.Broadcast(HealthCurrent_, HealthMax_, ValueAsPercent);
}

void UVitalityWelfareComponent::OnRep_MagicValueChanged_Implementation(float OldValue)
{
	float CurrentValue = 0, MaximumValue = 0;
	const float ValueAsPercent = GetCurrentMagic(CurrentValue, MaximumValue);
	OnMagicUpdated.Broadcast(MagicCurrent_, MagicMax_, ValueAsPercent);
}
void UVitalityWelfareComponent::OnRep_MagicMaxChanged_Implementation(float OldValue)
{
	float CurrentValue = 0, MaximumValue = 0;
	const float ValueAsPercent = GetCurrentMagic(CurrentValue, MaximumValue);
	OnMagicUpdated.Broadcast(MagicCurrent_, MagicMax_, ValueAsPercent);
}

void UVitalityWelfareComponent::OnRep_StaminaValueChanged_Implementation(float OldValue)
{
	float CurrentValue = 0, MaximumValue = 0;
	const float ValueAsPercent = GetCurrentStamina(CurrentValue, MaximumValue);
	OnStaminaUpdated.Broadcast(StaminaCurrent_, StaminaMax_, ValueAsPercent);
}
void UVitalityWelfareComponent::OnRep_StaminaMaxChanged_Implementation(float OldValue)
{
	float CurrentValue = 0, MaximumValue = 0;
	const float ValueAsPercent = GetCurrentStamina(CurrentValue, MaximumValue);
	OnStaminaUpdated.Broadcast(StaminaCurrent_, StaminaMax_, ValueAsPercent);
}

void UVitalityWelfareComponent::OnRep_CaloriesValueChanged_Implementation(float OldValue)
{
	float CurrentValue = 0, MaximumValue = 0;
	const float ValueAsPercent = GetCurrentHunger(CurrentValue, MaximumValue);
	OnCaloriesUpdated.Broadcast(CaloriesCurrent_, CaloriesMax_, ValueAsPercent);
}
void UVitalityWelfareComponent::OnRep_CaloriesMaxChanged_Implementation(float OldValue)
{
	float CurrentValue = 0, MaximumValue = 0;
	const float ValueAsPercent = GetCurrentHunger(CurrentValue, MaximumValue);
	OnCaloriesUpdated.Broadcast(CaloriesCurrent_, CaloriesMax_, ValueAsPercent);
}

void UVitalityWelfareComponent::OnRep_HydrationValueChanged_Implementation(float OldValue)
{
	float CurrentValue = 0, MaximumValue = 0;
	const float ValueAsPercent = GetCurrentHydration(CurrentValue, MaximumValue);
	OnHydrationUpdated.Broadcast(HydrationCurrent_, HydrationMax_, ValueAsPercent);
}
void UVitalityWelfareComponent::OnRep_HydrationMaxChanged_Implementation(float OldValue)
{
	float CurrentValue = 0, MaximumValue = 0;
	const float ValueAsPercent = GetCurrentHydration(CurrentValue, MaximumValue);
	OnHydrationUpdated.Broadcast(HydrationCurrent_, HydrationMax_, ValueAsPercent);
}

/**
 * @brief Sent to all clients when damage was taken by this actor.
 * @param DamageInstigator The actor dealing the damage. Nullptr means environmental.
 * @param DamageTaken The actual value of damage taken
 */
void UVitalityWelfareComponent::Multicast_DamageTaken_Implementation(
		AActor* DamageInstigator, float DamageTaken)
{
	OnDamageTaken.Broadcast(DamageInstigator, DamageTaken);
}

/**
* @brief  Sent to the owning client so they can run death-related delegates.
 * @param DamageInstigator The actor who dealt the damage. Nullptr means environmental.
 */
void UVitalityWelfareComponent::Multicast_VitalityDeath_Implementation(
	AActor* DamageInstigator, UAnimMontage* DeathAnim, USoundBase* DeathSound)
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathSound,
		GetOwner()->GetActorLocation(), FRotator(), 1.0, 1.0, 0.f);
	Cast<ACharacter>(GetOwner())->PlayAnimMontage(DeathAnim);
	OnDeath.Broadcast(DamageInstigator);
}