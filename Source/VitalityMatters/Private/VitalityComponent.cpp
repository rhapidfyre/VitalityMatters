// Fill out your copyright notice in the Description page of Project Settings.


#include "VitalityComponent.h"
#include "GameFramework/Character.h"

#include "Net/UnrealNetwork.h"


// Sets default values for this component's properties
UVitalityComponent::UVitalityComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

float UVitalityComponent::GetVitalityStat(EVitalityCategories VitalityStat, bool AsPercentage)
{
	switch(VitalityStat)
	{
	case EVitalityCategories::HEALTH:
		return AsPercentage ? (mHealthValue/mHealthMax) : mHealthValue;
	case EVitalityCategories::STAMINA:
		return AsPercentage ? (mStaminaValue/mStaminaMax) : mStaminaValue;
	case EVitalityCategories::HUNGER:
		return AsPercentage ? (mCaloriesValue/mCaloriesMax) : mHealthValue;
	case EVitalityCategories::THIRST:
		return AsPercentage ? (mHydrationValue/mHydrationMax) : mHealthValue;
	case EVitalityCategories::MAGIC:
		return AsPercentage ? (mHealthValue/mHealthMax) : mHealthValue;
	}
	return -0.f;
}

float UVitalityComponent::SetVitalityStat(EVitalityCategories VitalityStat, float NewValue)
{
	switch(VitalityStat)
	{
	case EVitalityCategories::HEALTH:
		mHealthValue = NewValue;
		return mHealthValue;
	case EVitalityCategories::STAMINA:
		mStaminaValue = NewValue;
		return mStaminaValue;
	case EVitalityCategories::HUNGER:
		mCaloriesValue = NewValue;
		return mCaloriesValue;
	case EVitalityCategories::THIRST:
		mHydrationValue = NewValue;
		return mHydrationValue;
	case EVitalityCategories::MAGIC:
		mMagicValue = NewValue;
		return mMagicValue;
	}
	return -0.f;
}

float UVitalityComponent::ModifyVitalityStat(EVitalityCategories VitalityStat, float AddValue)
{
	if (AddValue != 0.f)
		return SetVitalityStat(VitalityStat, GetVitalityStat(VitalityStat, false) + AddValue);
	return 0.f;
}

bool UVitalityComponent::ApplyEffectBeneficial(EEffectsBeneficial EffectBeneficial)
{
	
	return false;
}

bool UVitalityComponent::RevokeEffectBeneficial(EEffectsBeneficial EffectBeneficial)
{
	return false;
}

bool UVitalityComponent::ApplyEffectDetrimental(EEffectsDetrimental EffectDetrimental)
{
	return false;
}

bool UVitalityComponent::RevokeEffectDetrimental(EEffectsDetrimental EffectDetrimental)
{
	return false;
}

void UVitalityComponent::ToggleSprint(bool DoSprint)
{
	
}

// Called when the game starts
void UVitalityComponent::BeginPlay()
{
	Super::BeginPlay();

	InitSubsystems( IsValid( Cast<ACharacter>(GetOwner()) ) );

	ReloadFromSaveFile();
	
}

void UVitalityComponent::TickStamina()
{
	
}

void UVitalityComponent::TickHealth()
{
	
}

void UVitalityComponent::TickCalories()
{
	
}

void UVitalityComponent::TickHydration()
{
	
}

void UVitalityComponent::TickEffects()
{
	
}

void UVitalityComponent::ReloadFromSaveFile()
{
	UE_LOG(LogTemp, Warning, TEXT("%s(%s): Reload saved settings from file NOT IMPLEMENTED!"),
		*GetName(), GetOwner()->HasAuthority()?TEXT("SERVER"):TEXT("CLIENT"));
}

void UVitalityComponent::TickManager()
{
	TickStamina();
	TickHealth();
	TickCalories();
	TickHydration();
	TickEffects();
}

void UVitalityComponent::InitSubsystems(bool isCharacter)
{

	// Read settings and set members

	const float managerTickRate = VitalityTickRate <= 0.f ? 1.f : VitalityTickRate;
	GetWorld()->GetTimerManager().SetTimer(
		mTickTimer, this, &UVitalityComponent::TickManager, managerTickRate, true);
		
	const float staminaCooldownRate = StaminaCooldown <= 0.f ? 3.f : StaminaCooldown;
	GetWorld()->GetTimerManager().SetTimer(
	mStaminaCooldownTimer, this, &UVitalityComponent::EndStaminaCooldown, staminaCooldownRate, true);
	if (mStaminaCooldownTimer.IsValid())
		GetWorld()->GetTimerManager().PauseTimer(mStaminaCooldownTimer);

	// Character-Only Initialization
	if (isCharacter)
	{
		// Only character-types have stamina
		mSprintSpeed		= SprintSpeedMultiplier >  1.f ? SprintSpeedMultiplier	:		1.2;
		mStaminaDrain		= StaminaDrainRate		>= 0.f ? StaminaDrainRate		:		1.f;
		mStaminaRegen		= StaminaRegenRate		>= 0.f ? StaminaRegenRate		:		2.f;
		mStaminaMax			= StaminaMaximum		 > 0.f ? StaminaMaximum			:	  100.f;
		mStaminaValue		= mStaminaMax;
	}
	// Non-Character Initialization
	else
	{
		mMagicMax			= MagicMaximum			>  0.f ? MagicMaximum			:		1.f;
		mCaloriesDrainRest	= CaloriesAtRest		>= 0.f ? CaloriesAtRest			:	    0.002;
		mHydrationDrainRest = HungerAtRest			>= 0.f ? HungerAtRest			:	    0.002;
		mCaloriesMax		= CaloriesMaximum		>  0.f ? CaloriesMaximum 		:	  100.f;
		mHydrationMax		= HydrationMaximum		>  0.f ? HydrationMaximum		:	  100.f;
	}
	
	// Initialization, All Cases
	
	//						Condition		 		   ?	If True				:	If False
	mHealthRegen		= HealthRegenRate		>= 0.f ? HealthRegenRate		:		0.5;
	mHealthMax			= HealthMaximum			 > 0.f ? HealthMaximum			:	  100.f;
	
	mHealthValue	= mHealthMax;
	mCaloriesValue	= mCaloriesMax;
	mHydrationValue = mHydrationMax;
	mMagicValue		= mMagicMax;
	
}

void UVitalityComponent::StopSprinting()
{
}

void UVitalityComponent::StartSprinting()
{
}


/****************************************
 * REPLICATION
***************************************/

void UVitalityComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Update to owner only.
	// These vars only have value to the actor it affects and no-one else.
	DOREPLIFETIME_CONDITION(UVitalityComponent, mIsSprinting,	COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UVitalityComponent, mStaminaValue,	COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UVitalityComponent, mStaminaMax,	COND_OwnerOnly);

	// Effects don't need to be multicast.
	// If the effect spawns an actor, it'll be done server-side and synced.
	DOREPLIFETIME_CONDITION(UVitalityComponent, mCurrentEffects,	COND_OwnerOnly);
}