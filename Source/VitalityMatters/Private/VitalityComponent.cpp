
#include "VitalityComponent.h"
#include "GameFramework/Character.h"

#include "Net/UnrealNetwork.h"

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

// Sets default values for this component's properties
UVitalityComponent::UVitalityComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	SetAutoActivate(true);
}

void UVitalityComponent::OnComponentCreated()
{
	Super::OnComponentCreated();
	RegisterComponent();
}


float UVitalityComponent::DamageHealth(AActor* DamageActor, float DamageTaken)
{
	const float oldHealth = mHealthValue;
	const float NewDamage = 0.f - abs(DamageTaken); // ensures a negative
	const float newHealth = ModifyVitalityStat(EVitalityCategories::HEALTH, NewDamage);
	if (newHealth < oldHealth)
	{
		if (newHealth <= 0.f && oldHealth > 0.f)
		{
			OnDeath.Broadcast(DamageActor);
			Multicast_VitalityDeath();
		}
		else
		{
			OnDamageTaken.Broadcast(DamageActor, NewDamage);
			Multicast_DamageTaken(NewDamage);
		}
	}
	return newHealth;
}

float UVitalityComponent::GetVitalityStat(EVitalityCategories VitalityStat, float &StatValue, float &StatMax)
{
	
	switch(VitalityStat)
	{
	case EVitalityCategories::HEALTH:
		StatValue = mHealthValue;
		StatMax = mHealthMax;
		return mHealthValue/mHealthMax;
	case EVitalityCategories::STAMINA:
		StatValue = mStaminaValue;
		StatMax = mStaminaMax;
		return mStaminaValue/mStaminaMax;
	case EVitalityCategories::HUNGER:
		StatValue = mCaloriesValue;
		StatMax = mCaloriesMax;
		return mCaloriesValue/mCaloriesMax;
	case EVitalityCategories::THIRST:
		StatValue = mHydrationValue;
		StatMax = mHydrationMax;
		return mHydrationValue/mHydrationMax;
	case EVitalityCategories::MAGIC:
		StatValue = mMagicValue;
		StatMax = mMagicMax;
		return mMagicValue/mMagicMax;
	default:
		break;
	}
	return -0.f;
}

float UVitalityComponent::GetVitalityStat(EVitalityCategories VitalityStat)
{
	switch(VitalityStat)
	{
	case EVitalityCategories::HEALTH:
		return mHealthValue/mHealthMax;
	case EVitalityCategories::STAMINA:
		return mStaminaValue/mStaminaMax;
	case EVitalityCategories::HUNGER:
		return mCaloriesValue/mCaloriesMax;
	case EVitalityCategories::THIRST:
		return mHydrationValue/mHydrationMax;
	case EVitalityCategories::MAGIC:
		return mMagicValue/mMagicMax;
	default:
		break;
	}
	return -0.f;
}

float UVitalityComponent::SetVitalityStat(EVitalityCategories VitalityStat, float NewValue)
{
	switch(VitalityStat)
	{
	case EVitalityCategories::HEALTH:
		
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
		
	case EVitalityCategories::STAMINA:
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
		
	case EVitalityCategories::HUNGER:
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
		
	case EVitalityCategories::THIRST:
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
		
	case EVitalityCategories::MAGIC:
		mMagicValue = NewValue;
		return mMagicValue;
		
	default:
		break;
	}
	return -0.f;
}

float UVitalityComponent::ModifyVitalityStat(EVitalityCategories VitalityStat, float AddValue)
{
	if (AddValue != 0.f)
	{
		float statValue(0.f); float statMax(0.f);
		GetVitalityStat(VitalityStat, statValue, statMax);
		return SetVitalityStat(VitalityStat, statValue + AddValue);
	}
	return 0.f;
}

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

bool UVitalityComponent::RemoveEffectAtIndex(int IndexNumber)
{
	if (mCurrentEffects.IsValidIndex(IndexNumber))
	{
		mEffectsRemoveQueue.Add(mCurrentEffects[IndexNumber].uniqueId);
		return true;
	}
	return false;
}

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
			const float caloriesPercent = GetVitalityStat(EVitalityCategories::HUNGER);

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
				if (!vitalityData.isPersistent)
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

void UVitalityComponent::ReloadFromSaveFile()
{
	UE_LOG(LogTemp, Warning, TEXT("%s(%s): Reload saved settings from file NOT IMPLEMENTED!"),
		*GetName(), GetOwner()->HasAuthority()?TEXT("SERVER"):TEXT("CLIENT"));
}

void UVitalityComponent::InitSubsystems(bool isCharacter)
{

	if (!GetOwner()->HasAuthority()) return;

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

	FTimerDelegate staminaDelegate; staminaDelegate.BindUObject(this, &UVitalityComponent::TickStamina);
	InitializeTimer(mStaminaTimer, staminaDelegate);
	
	FTimerDelegate healthDelegate; healthDelegate.BindUObject(this, &UVitalityComponent::TickHealth);
	InitializeTimer(mHealthTimer, healthDelegate);
	
	FTimerDelegate caloriesDelegate; caloriesDelegate.BindUObject(this, &UVitalityComponent::TickCalories);
	InitializeTimer(mCaloriesTimer, caloriesDelegate);
	
	FTimerDelegate hydrationDelegate; hydrationDelegate.BindUObject(this, &UVitalityComponent::TickHydration);
	InitializeTimer(mHydrationTimer, hydrationDelegate);
	
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
	{
		if (timerHandle.IsValid())
		{
			UE_LOG(LogTemp, Display, TEXT("%s(%s): Timer Handle '%s' has been INVALIDATED"),
				*GetName(), GetOwner()->HasAuthority()?TEXT("SERVER"):TEXT("CLIENT"), *timerHandle.ToString());
			timerHandle.Invalidate();
		}
	}
}

void UVitalityComponent::Multicast_VitalityDeath_Implementation()
{
	OnDeath.Broadcast(nullptr);
}

void UVitalityComponent::Multicast_DamageTaken_Implementation(float DamageTaken)
{
	OnDamageTaken.Broadcast(nullptr, DamageTaken);
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
	
	DOREPLIFETIME_CONDITION(UVitalityComponent, mStaminaValue,		COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UVitalityComponent, mStaminaMax,		COND_OwnerOnly);
	
	DOREPLIFETIME_CONDITION(UVitalityComponent, mHealthValue,		COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UVitalityComponent, mHealthMax,			COND_OwnerOnly);
	
	DOREPLIFETIME_CONDITION(UVitalityComponent, mMagicValue,		COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UVitalityComponent, mMagicMax,			COND_OwnerOnly);
	
	DOREPLIFETIME_CONDITION(UVitalityComponent, mCaloriesValue,		COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UVitalityComponent, mCaloriesMax,		COND_OwnerOnly);
	
	DOREPLIFETIME_CONDITION(UVitalityComponent, mHydrationValue,	COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UVitalityComponent, mHydrationMax,		COND_OwnerOnly);

	// Effects don't need to be multicast.
	// If the effect spawns an actor, it'll be done server-side and synced.
	DOREPLIFETIME_CONDITION(UVitalityComponent, mCurrentEffects,	COND_OwnerOnly);
}