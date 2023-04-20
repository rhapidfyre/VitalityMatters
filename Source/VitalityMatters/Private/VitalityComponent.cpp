// Fill out your copyright notice in the Description page of Project Settings.


#include "VitalityComponent.h"
#include "GameFramework/Character.h"

#include "Net/UnrealNetwork.h"

UDataTable* GetVitalityEffectsTable()
{
	const FSoftObjectPath itemTable = FSoftObjectPath("/T5GInventorySystem/DataTables/DT_ItemData.DT_ItemData");
	UDataTable* dataTable = Cast<UDataTable>(itemTable.ResolveObject());
	if (IsValid(dataTable)) return dataTable;
	return Cast<UDataTable>(itemTable.TryLoad());
}

FStVitalityEffects UVitalityComponent::GetVitalityEffect(FName EffectName)
{
	UDataTable* vitalityData = GetVitalityEffectsTable();
	if (IsValid(vitalityData))
	{
		const FString errorCaught;
		FStVitalityEffects* vitalityPointer = vitalityData->FindRow<FStVitalityEffects>(EffectName, errorCaught);
		if (vitalityPointer != nullptr)
		{
			return *vitalityPointer;
		}
	}
	return FStVitalityEffects();
}

FStVitalityEffects UVitalityComponent::GetVitalityEffect(EEffectsBeneficial EffectEnum)
{
	if (EffectEnum != EEffectsBeneficial::NONE)
	{
		const FString effectString = UEnum::GetValueAsString(EffectEnum);
		return GetVitalityEffect(*effectString);
	}
	return FStVitalityEffects();
}

FStVitalityEffects UVitalityComponent::GetVitalityEffect(EEffectsDetrimental EffectEnum)
{
	if (EffectEnum != EEffectsDetrimental::NONE)
	{
		const FString effectString = UEnum::GetValueAsString(EffectEnum);
		return GetVitalityEffect(*effectString);
	}
	return FStVitalityEffects();
}

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

bool UVitalityComponent::ApplyEffectBeneficial(EEffectsBeneficial EffectBeneficial, int StackCount)
{
	// Do nothing if the effect or stack count is invalid
	if (EffectBeneficial == EEffectsBeneficial::NONE || StackCount < 1)
		return false;

	// Obtain the appropriate vitality effect the given StackCount number of times
	FStVitalityEffects vitalityData = GetVitalityEffect(EffectBeneficial);
	if (vitalityData.benefitEffect != EEffectsBeneficial::NONE)
	{
		int effectStackCount(1);
		for (int i = 0; i < mCurrentEffects.Num(); i++)
		{
			if (mCurrentEffects[i].benefitEffect == EffectBeneficial)
				effectStackCount += 1;
		}
		mCurrentEffects.Add(vitalityData);
		// Notify listeners
		OnEffectBeneficial.Broadcast(vitalityData.benefitEffect, true);
	}
	return false;
}

bool UVitalityComponent::RevokeEffectBeneficial(EEffectsBeneficial EffectBeneficial, int StackCount)
{
	// Do nothing if the effect or stack count is invalid
	if (EffectBeneficial == EEffectsBeneficial::NONE || StackCount < 1)
		return false;

	// Removes the given effect, StackCount number of times
	int effectsRemoved = 0;
	while (mEffectsMutex);
	mEffectsMutex = true;
	for (int i = mCurrentEffects.Num() - 1; i >= 0; i--)
	{
		if (effectsRemoved >= StackCount)
			break;
		// If this effect matches the benefit to revoke, remove it
		if (mCurrentEffects[i].benefitEffect == EffectBeneficial)
		{
			mCurrentEffects.RemoveAt(i);
			effectsRemoved++;
		}
	}
	mEffectsMutex = false;
	return (effectsRemoved > 0);
}

bool UVitalityComponent::ApplyEffectDetrimental(EEffectsDetrimental EffectDetrimental, int StackCount)
{
	// Do nothing if the effect or stack count is invalid
	if (EffectDetrimental == EEffectsDetrimental::NONE || StackCount < 1)
		return false;

	// Obtain the effect data, and apply it the number of StackCount times
	FStVitalityEffects vitalityData = GetVitalityEffect(EffectDetrimental);
	if (vitalityData.detrimentEffect != EEffectsDetrimental::NONE)
	{
		int effectStackCount(1);
		for (int i = 0; i < mCurrentEffects.Num(); i++)
		{
			if (mCurrentEffects[i].detrimentEffect == EffectDetrimental)
				effectStackCount += 1;
		}
		mCurrentEffects.Add(vitalityData);

		// If this effect disables sprinting and sprint is enabled, disable it.
		if (vitalityData.disableSprinting && mCanSprint)
		{
			mCanSprint = false;
		}

		// Tell all other scripts that are listening
		OnEffectBeneficial.Broadcast(vitalityData.benefitEffect, true);
	}
	return false;
}

bool UVitalityComponent::RevokeEffectDetrimental(EEffectsDetrimental EffectDetrimental, int StackCount)
{
	// Return false if invalid effect or stack count
	if (EffectDetrimental == EEffectsDetrimental::NONE || StackCount < 1)
		return false;

	// Remove the effect the given number of times, or until all occurrences are gone. Whichever occurs first.
	int effectsRemoved = 0;
	while (mEffectsMutex);
	mEffectsMutex = true;
	for (int i = mCurrentEffects.Num() - 1; i >= 0; i--)
	{
		if (effectsRemoved >= StackCount)
			break;
		
		if (mCurrentEffects[i].detrimentEffect == EffectDetrimental)
		{
			mCurrentEffects.RemoveAt(i);
			effectsRemoved++;
		}
	}
	mEffectsMutex = false;

	// Nothing was removed, the effect wasn't applied
	if (effectsRemoved < 1)
		return true;
	
	// If sprinting is disabled, enable sprinting if none of the effects block sprinting
	if (!mCanSprint)
	{
		bool canSprint = true;
		for (int i = 0; i < mCurrentEffects.Num(); i++)
		{
			if (mCurrentEffects[i].disableSprinting)
			{
				canSprint = false;
				break;
			}
		}
		mCanSprint = canSprint;
	}
	
	return true;
}

bool UVitalityComponent::RevokeEffect(int IndexNumber)
{
	if (mCurrentEffects.IsValidIndex(IndexNumber))
	{
		mCurrentEffects.RemoveAt(IndexNumber);
		return true;
	}
	return false;
}

void UVitalityComponent::ToggleSprint(bool DoSprint)
{
	if (!GetOwner()->HasAuthority())
	{
		Server_ToggleSprint();
		return;
	}
	if (DoSprint && mCanSprint)
	{
		StartSprinting();
	}
	else
	{
		mIsSprinting = false;
	}
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
	if (mIsSprinting && mCanSprint)
	{
		mStaminaValue -= mStaminaDrain;
		if (mStaminaValue <= 0)
		{
			StopSprinting();
			ApplyEffectDetrimental(EEffectsDetrimental::TIRED, 1);
		}
	}
	else
	{
		if (mStaminaValue > 0)
		{
			RevokeEffectDetrimental(EEffectsDetrimental::TIRED, 1);

			if (mStaminaValue >= mStaminaMax)
			{
				mStaminaValue = mStaminaMax;
				GetWorld()->GetTimerManager().PauseTimer(mStaminaTimer);	
			}
			else
			{
				// If stamina isn't on refresh/cooldown, regenerate
				if (GetWorld()->GetTimerManager().IsTimerPaused(mStaminaCooldownTimer))
				{
					mStaminaValue += mStaminaRegen;
				}
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
			mHealthValue += mHealthRegen;
		}
		else if (mHealthValue > mHealthMax)
		{
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
			mCaloriesValue = 0.f;
			ApplyEffectDetrimental((EEffectsDetrimental::HUNGER));
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
			mHydrationValue = 0.f;
			ApplyEffectDetrimental((EEffectsDetrimental::THIRST));
			mCanSprint = false;
		}
	}
}

void UVitalityComponent::TickEffects()
{
	while (mEffectsMutex);
	mEffectsMutex = true;
	for (int i = mCurrentEffects.Num(); i >= 0; i--)
	{
		if (mCurrentEffects.IsValidIndex(i))
		{
			FStVitalityEffects vitalityData = mCurrentEffects[i];
			if (!vitalityData.isPersistent)
			{
				mCurrentEffects[i].effectSeconds--;
				if (mCurrentEffects[i].effectSeconds <= 0)
				{
					RevokeEffect(i);
				}
			}
		}
	}
	mEffectsMutex = false;
}

void UVitalityComponent::ReloadFromSaveFile()
{
	UE_LOG(LogTemp, Warning, TEXT("%s(%s): Reload saved settings from file NOT IMPLEMENTED!"),
		*GetName(), GetOwner()->HasAuthority()?TEXT("SERVER"):TEXT("CLIENT"));
}

void UVitalityComponent::TickManager()
{
	//TickStamina(); handled by mStaminaCooldownTimer
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

	GetWorld()->GetTimerManager().SetTimer(
	mStaminaTimer, this, &UVitalityComponent::TickStamina, managerTickRate, true);
	if (mStaminaTimer.IsValid())
		GetWorld()->GetTimerManager().PauseTimer(mStaminaTimer);

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
	if (GetOwner()->HasAuthority())
	{
		mIsSprinting = false;
		if (!mStaminaCooldownTimer.IsValid())
		{
			// Setup the cooldown timer to prevent stamina regen
			const float staminaCooldownRate = StaminaCooldown <= 0.f ? 3.f : StaminaCooldown;
			GetWorld()->GetTimerManager().SetTimer(mStaminaCooldownTimer, this,
				&UVitalityComponent::EndStaminaCooldown, staminaCooldownRate, false);
		}
		OnSprint.Broadcast(mIsSprinting);
	}
}

void UVitalityComponent::StartSprinting()
{
	if (GetOwner()->HasAuthority())
	{
		mIsSprinting = true;
		if (GetWorld()->GetTimerManager().IsTimerPaused(mStaminaTimer))
			GetWorld()->GetTimerManager().UnPauseTimer(mStaminaTimer);
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