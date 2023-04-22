// Fill out your copyright notice in the Description page of Project Settings.


#include "VitalityComponent.h"
#include "GameFramework/Character.h"

#include "Net/UnrealNetwork.h"

UDataTable* GetVitalityEffectsTable()
{
	const FSoftObjectPath itemTable = FSoftObjectPath("/VitalityMatters/DataTables/DT_VitalityData.DT_VitalityData");
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

int UVitalityComponent::GetNumActiveBenefit(EEffectsBeneficial BenefitEffect)
{
	if (BenefitEffect == EEffectsBeneficial::NONE)
		return 0;
	int benefitCount(0);
	for (int i = 0; mCurrentEffects.Num(); i++)
	{
		if (mCurrentEffects[i].benefitEffect == BenefitEffect)
			benefitCount++;
	}
	return benefitCount;
}

int UVitalityComponent::GetNumActiveDetriment(EEffectsDetrimental DetrimentEffect)
{
	if (DetrimentEffect == EEffectsDetrimental::NONE)
		return 0;
	int detrimentCount(0);
	for (int i = 0; mCurrentEffects.Num(); i++)
	{
		if (mCurrentEffects[i].detrimentEffect == DetrimentEffect)
			detrimentCount++;
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
	for (int i = 0; i < mCurrentEffects.Num(); i++)
	{
		mEffectsRemoveQueue.Add(UniqueId);
		return true;
	}
	return false;
}

bool UVitalityComponent::ApplyEffect(FName EffectName, int StackCount)
{
	FStVitalityEffects vitalityData = GetVitalityEffect(EffectName);
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
	int removalEntries = RemoveCount < mCurrentEffects.Num() ? RemoveCount : mCurrentEffects.Num();
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
	return (effectsRemoved.Num() > 0);
}

bool UVitalityComponent::ApplyEffectBeneficial(EEffectsBeneficial EffectBeneficial, int StackCount)
{
	// Do nothing if the effect or stack count is invalid
	if (EffectBeneficial == EEffectsBeneficial::NONE || StackCount < 1)
		return false;

	// Obtain the appropriate vitality effect the given StackCount number of times
	FStVitalityEffects vitalityData = GetVitalityEffect(EffectBeneficial);
	vitalityData.uniqueId = GenerateUniqueId();
	if (vitalityData.uniqueId < 1)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to generate unique ID for ApplyEffectBeneficial"));
		return false;
	}
	if (vitalityData.benefitEffect != EEffectsBeneficial::NONE)
	{
		int effectStackCount(1);
		for (int i = 0; i < mCurrentEffects.Num(); i++)
		{
			if (mCurrentEffects[i].benefitEffect == EffectBeneficial)
				effectStackCount += 1;
		}
		mEffectsAddQueue.Add(vitalityData);
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
	FStVitalityEffects vitalityData = GetVitalityEffect(EffectDetrimental);
	vitalityData.uniqueId = GenerateUniqueId();
	if (vitalityData.uniqueId < 1)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to generate unique ID for ApplyEffectBeneficial"));
		return false;
	}
	if (vitalityData.detrimentEffect != EEffectsDetrimental::NONE)
	{
		int effectStackCount(1);
		for (int i = 0; i < mCurrentEffects.Num(); i++)
		{
			if (mCurrentEffects[i].detrimentEffect == EffectDetrimental)
				effectStackCount += 1;
		}
		mEffectsAddQueue.Add(vitalityData);

		// If this effect disables sprinting and sprint is enabled, disable it.
		if (vitalityData.disableSprinting && mCanSprint)
		{
			mCanSprint = false;
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

	// Nothing was removed, the effect wasn't applied
	if (removedUniqueIds.Num() < 1)
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
		for (int i = 0; mCurrentEffects.Num(); i++)
		{
			if (mCurrentEffects[i].uniqueId == UniqueId)
				return mCurrentEffects[i];
		}
	}
	return {};
}

TArray<FStVitalityEffects> UVitalityComponent::GetAllEffectsByDetriment(EEffectsDetrimental DetrimentEffect)
{
	return {};
}

TArray<FStVitalityEffects> UVitalityComponent::GetAllEffectsByBenefit(EEffectsBeneficial BenefitEffect)
{
	return {};
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
			ApplyEffect("tired");
			mStaminaValue = 0;
		}
	}
	else
	{
		if (GetEffe)
		{			
			// If stamina is fully regenerated, kill the timer. It's not needed anymore.
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
			mHydrationValue = 0.f;
			ApplyEffect("thirsty");
			mCanSprint = false;
		}
	}
}

void UVitalityComponent::TickEffects()
{
	if (mEffectsMutex) return;
	mEffectsMutex = true;
	
	// Remove any effects that are pending removal
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
	
	// Perform any logic effects need done per tick
	for (int i = 0; i < mCurrentEffects.Num(); i++)
	{
		if (mCurrentEffects.IsValidIndex(i))
		{
			FStVitalityEffects vitalityData = mCurrentEffects[i];
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
	
	// Add any affects that need to be added
	for (int i = mEffectsAddQueue.Num() - 1; i >= 0; i--)
	{
		mCurrentEffects.Add( mEffectsAddQueue[i] );
		OnEffectModified.Broadcast(mEffectsAddQueue[i].uniqueId, true);
		mEffectsAddQueue.RemoveAt(i);
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

	if (!GetOwner()->HasAuthority()) return;
	
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
		int tempNumber = FMath::RandRange(1,INT_MAX);
		for (int i = 0; i < mCurrentEffects.Num(); i++)
		{
			if (mCurrentEffects[i].uniqueId == tempNumber)
			{
				idExists = true;
				break;
			}
		}
		if (!idExists)
			randomNumber = tempNumber;
	}
	return randomNumber;
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