﻿// Copyright Take Five Games, LLC 2023 - All Rights Reserved


#include "VitalityEffectsComponent.h"

#include "Net/UnrealNetwork.h"


UVitalityEffectsComponent::UVitalityEffectsComponent()
{
	SetIsReplicatedByDefault(true);
}

void UVitalityEffectsComponent::InitializeEffects(const TArray<FStVitalityEffects>& SavedEffects)
{
	Server_InitializeEffects(SavedEffects);
}

void UVitalityEffectsComponent::Server_InitializeEffects_Implementation(const TArray<FStVitalityEffects>& SavedEffects)
{
	if (GetOwner()->HasAuthority() && !bHasInitialized)
	{
		bHasInitialized = true;
		FRWScopeLock ReadLock(_EffectsLock, SLT_Write);
		_CurrentEffects.Empty();
		_CurrentEffects = SavedEffects;
	}
}


/** Adds the requested effect by data table name.
 * @param EffectName The table row name to apply.
 * @param StackCount The number of times to apply the effect
 * @return True if the effect was added. False on failure.
 */
bool UVitalityEffectsComponent::ApplyEffect(FName EffectName, int StackCount)
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
			_AddQueue.Add(vitalityData);
		}
	}
	return false;
}


/** Adds the requested beneficial effect by enum.
 * @param EffectBeneficial The num to apply/revoke.
 * @param StackCount The number of times to apply the effect
 * @return True if the effect was added. False on failure.
 */
bool UVitalityEffectsComponent::ApplyEffectBeneficial(EEffectsBeneficial EffectBeneficial, int StackCount)
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
			FRWScopeLock ReadLock(_EffectsLock, SLT_Write);
			int effectStackCount(1);
			for (int i = 0; i < _CurrentEffects.Num(); i++)
			{
				if (_CurrentEffects[i].benefitEffect == EffectBeneficial)
					effectStackCount += 1;
			}
			for (int i = 0; i < StackCount; i++)
				_CurrentEffects.Add(vitalityData);
		}
	}
	return false;
}

/** Adds the requested detriment enum.
 * @param EffectDetrimental The num to apply/revoke.
 * @param StackCount The number of times to apply the effect
 * @return True if the effect was added. False on failure.
 */
bool UVitalityEffectsComponent::ApplyEffectDetrimental(EEffectsDetrimental EffectDetrimental, int StackCount)
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
		for (int i = 0; i < StackCount; i++)
			_AddQueue.Add(vitalityData);
	}
	
	return false;
}

/**
 * @brief Retrieves a COPY of the effect found with the given unique ID
 * @param UniqueId The Effect Unique ID to retrieve
 * @return A copy of the effect, avoiding memory access violations
 */
FStVitalityEffects UVitalityEffectsComponent::GetEffectByUniqueId(int UniqueId) 
{
	FRWScopeLock ReadLock(_EffectsLock, SLT_Write);
	for (int i = 0; i < _CurrentEffects.Num(); i++)
	{
		if (_CurrentEffects[i].uniqueId == UniqueId)
			return _CurrentEffects[i];
	}
	return {};
}

/**
 * @brief Removes n counts of the effect name (from the data tables) given
 * @param EffectName The effect proper name to remove
 * @param RemoveCount The number of stacks to remove
 * @return True on success, false otherwise
 */
bool UVitalityEffectsComponent::RemoveEffect(FName EffectName, int RemoveCount)
{
	TArray<int> effectsRemoved;
	// exp scope for good measure
	{
		FRWScopeLock ReadLock(_EffectsLock, SLT_Write);
		for (int i = 0; i < _CurrentEffects.Num(); i++)
		{
			if (effectsRemoved.Num() >= RemoveCount)
				break;
		
			if (_CurrentEffects[i].properName == EffectName)
			{
				effectsRemoved.Add(_CurrentEffects[i].uniqueId);
				_RemoveQueue.Add(_CurrentEffects[i].uniqueId);
			}
		
		}
	}
	return (effectsRemoved.Num() > 0);
}

/**
 * @brief Removes the effect by the given unique id
 * @param UniqueId The Unique Id to find
 * @return True on success, false otherwise
 */
bool UVitalityEffectsComponent::RemoveEffectByUniqueId(int UniqueId)
{
	FRWScopeLock ReadLock(_EffectsLock, SLT_Write);
	for (int i = 0; i < _CurrentEffects.Num(); i++)
	{		
		if (_CurrentEffects[i].uniqueId == UniqueId)
		{
			_RemoveQueue.Add(_CurrentEffects[i].uniqueId);
			return true;
		}
	}
	return false;
}

/**
 * @brief Removes an effect by the index in the array
 * @param IndexNumber The array index to remove
 * @return True on success, false otherwise
 */
bool UVitalityEffectsComponent::RemoveEffectAtIndex(int IndexNumber)
{
	if (!_CurrentEffects.IsValidIndex(IndexNumber))
		return false;
	if (_EffectsLock.TryReadLock())
	{
		UE_LOG(LogTemp, Error
			, TEXT("RemoveEffectAtIndex(): Tried to remove index %d, but reading is not locked!")
			, IndexNumber);
		return false;
	}
	const int UniqueId		= _CurrentEffects[IndexNumber].uniqueId;
	const FName EffectName	= _CurrentEffects[IndexNumber].properName;
	_CurrentEffects.RemoveAt(IndexNumber);
	OnEffectDetrimentalExpired.Broadcast(UniqueId, EffectName);
	return true;
}

/**
 * @brief Removes n counts of the beneficial effect given
 * @param EffectBeneficial The beneficial effect enum to find
 * @param StackCount The number of stacks to remove
 * @return True on success, false otherwise
 */
bool UVitalityEffectsComponent::RemoveEffectBeneficial(EEffectsBeneficial EffectBeneficial, int StackCount)
{
	// Return false if invalid effect or stack count
	if (EffectBeneficial == EEffectsBeneficial::NONE || StackCount < 1)
		return false;


	// Remove the effect the given number of times, or until all occurrences are gone. Whichever occurs first.
	TMap<int, FName> removedUniqueIds;

	FRWScopeLock ReadLock(_EffectsLock, SLT_Write);
	
	for (int i = 0; i < _CurrentEffects.Num(); i++)
	{
		// Only remove up to the requested amount of stacks
		if (removedUniqueIds.Num() >= StackCount)
			break;
	
		if (_CurrentEffects[i].benefitEffect == EffectBeneficial)
		{
			// Add the effect to the remove queue for safe removal
			removedUniqueIds.Add(_CurrentEffects[i].uniqueId, _CurrentEffects[i].properName);
			_RemoveQueue.Add(_CurrentEffects[i].uniqueId);
		}
	}
	
	return true;
}


/**
 * @brief Removes n counts of the detrimental effect given
 * @param EffectDetrimental The detrimental effect enum to find
 * @param StackCount The number of stacks to remove
 * @return True on success, false otherwise
 */
bool UVitalityEffectsComponent::RemoveEffectDetrimental(EEffectsDetrimental EffectDetrimental, int StackCount)
{
	// Return false if invalid effect or stack count
	if (EffectDetrimental == EEffectsDetrimental::NONE || StackCount < 1)
		return false;

	// Remove the effect the given number of times, or until all occurrences are gone. Whichever occurs first.
	TMap<int, FName> removedUniqueIds;
	
	FRWScopeLock ReadLock(_EffectsLock, SLT_Write);
	for (int i = 0; i < _CurrentEffects.Num(); i++)
	{
		// Only remove up to the requested amount of stacks
		if (removedUniqueIds.Num() >= StackCount)
			break;
	
		if (_CurrentEffects[i].detrimentEffect == EffectDetrimental)
		{
			// Add the effect to the remove queue for safe removal
			removedUniqueIds.Add(_CurrentEffects[i].uniqueId, _CurrentEffects[i].properName);
			_RemoveQueue.Add(_CurrentEffects[i].uniqueId);
		}
	}
	return true;
}

/**
 * @brief Returns how many instances of the requested beneficial effect are active
 * @param BenefitEffect The beneficial effect enum to find
 * @return The number of active beneficial effects
 */
int UVitalityEffectsComponent::GetNumberOfActiveBenefits(EEffectsBeneficial BenefitEffect)
{
	if (BenefitEffect == EEffectsBeneficial::NONE) return 0;
	int NumEffectsActive(0);
	FRWScopeLock WriteLock(_EffectsLock, SLT_ReadOnly);
	for (const FStVitalityEffects& CurrentEffect : _CurrentEffects)
	{
		if (CurrentEffect.benefitEffect == BenefitEffect)
			NumEffectsActive += 1;
	}
	return NumEffectsActive;
}


/**
 * @brief Returns how many instances of the requested detrimental effect are active
 * @param DetrimentEffect The detrimental effect enum to find
 * @return The number of active detrimental effects
 */
int UVitalityEffectsComponent::GetNumberOfActiveDetriments(EEffectsDetrimental DetrimentEffect)
{
	if (DetrimentEffect == EEffectsDetrimental::NONE) return 0;
	int NumEffectsActive(0);
	FRWScopeLock WriteLock(_EffectsLock, SLT_ReadOnly);
	for (const FStVitalityEffects& CurrentEffect : _CurrentEffects)
	{
		if (CurrentEffect.detrimentEffect == DetrimentEffect)
			NumEffectsActive += 1;
	}
	return NumEffectsActive;
}


/**
 * @brief Returns an array containing copies of all of active benefits
 * @param BenefitEffect The benefit effect to find
 * @return An array with copies of all active benefit effects
 */
TArray<FStVitalityEffects> UVitalityEffectsComponent::GetAllEffectsByBenefit(
	EEffectsBeneficial BenefitEffect)
{
	if (BenefitEffect == EEffectsBeneficial::NONE) return {};
	TArray<FStVitalityEffects> EffectCopies;
	FRWScopeLock WriteLock(_EffectsLock, SLT_ReadOnly);
	for (const FStVitalityEffects& CurrentEffect : _CurrentEffects)
	{
		if (CurrentEffect.benefitEffect == BenefitEffect)
			EffectCopies.Add(CurrentEffect);
	}
	return EffectCopies;
}

/**
 * @brief Returns an array containing copies of all of active detriments
 * @param DetrimentEffect The detrimental effect to find
 * @return An array with copies of all active detrimental effects
 */
TArray<FStVitalityEffects> UVitalityEffectsComponent::GetAllEffectsByDetriment(
	EEffectsDetrimental DetrimentEffect)
{
	if (DetrimentEffect == EEffectsDetrimental::NONE) return {};
	TArray<FStVitalityEffects> EffectCopies;
	FRWScopeLock WriteLock(_EffectsLock, SLT_ReadOnly);
	for (const FStVitalityEffects& CurrentEffect : _CurrentEffects)
	{
		if (CurrentEffect.detrimentEffect == DetrimentEffect)
			EffectCopies.Add(CurrentEffect);
	}
	return EffectCopies;
}

/**
 * @brief Checks if the requested effect name is active
 * @param EffectName The effect name to locate
 * @return True if the effect is active, false otherwise
 */
bool UVitalityEffectsComponent::IsEffectActive(FName EffectName) const
{
	for (const FStVitalityEffects CurrentEffect : _CurrentEffects)
	{
		if (CurrentEffect.properName == EffectName)
			return true;
	}
	return false;
}

/**
 * @brief Checks if the requested id is active
 * @param UniqueId The effect id to locate
 * @return True if the effect is active, false otherwise
 */
bool UVitalityEffectsComponent::IsEffectIdActive(int UniqueId) const
{
	for (const FStVitalityEffects CurrentEffect : _CurrentEffects)
	{
		if (CurrentEffect.uniqueId == UniqueId)
			return true;
	}
	return false;
}

/**
 * @brief Checks if the requested beneficial enum effect is active
 * @param EffectEnum The effect enum to locate
 * @return True if the effect is active, false otherwise
 */
bool UVitalityEffectsComponent::IsEffectBeneficialActive(EEffectsBeneficial EffectEnum) const
{
	for (const FStVitalityEffects CurrentEffect : _CurrentEffects)
	{
		if (CurrentEffect.benefitEffect == EffectEnum)
			return true;
	}
	return false;
}

/**
 * @brief Checks if the requested detrimental enum effect is active
 * @param EffectEnum The effect enum to locate
 * @return True if the effect is active, false otherwise
 */
bool UVitalityEffectsComponent::IsEffectDetrimentalActive(EEffectsDetrimental EffectEnum) const
{
	for (const FStVitalityEffects CurrentEffect : _CurrentEffects)
	{
		if (CurrentEffect.detrimentEffect == EffectEnum)
			return true;
	}
	return false;
}

void UVitalityEffectsComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UVitalityEffectsComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UVitalityEffectsComponent, _CurrentEffects, COND_OwnerOnly);
}

// Runs the tick timer, evaluating each active effect per tick
void UVitalityEffectsComponent::TickEffects()
{
	// Perform any logic effects need done per tick
	if (_CurrentEffects.Num() > 0)
	{
		FRWScopeLock WriteLock(_EffectsLock, SLT_ReadOnly);
		for (int i = 0; i < _CurrentEffects.Num(); i++)
		{
			if (_CurrentEffects.IsValidIndex(i))
			{
				const FStVitalityEffects vitalityData = _CurrentEffects[i];
				if (!vitalityData.bIsPersistent)
				{
					_CurrentEffects[i].effectTicks--;
					if (_CurrentEffects[i].effectTicks < 1)
					{
						RemoveEffectAtIndex(i);
					}
				}
			}
		}
	}
	
	// Remove any effects that are pending removal
	if (_RemoveQueue.Num() > 0)
	{
		// Lock against any other reading or writing until finished
		FRWScopeLock WriteLock(_EffectsLock, SLT_Write);
		
		for (int i = _RemoveQueue.Num() - 1; i >= 0; i--)
		{
			for (int j = 0; j < _CurrentEffects.Num(); j++)
			{
				if (_CurrentEffects[j].uniqueId == _RemoveQueue[i])
				{
					_CurrentEffects.RemoveAt(j);
					break;
				}
			}
			_RemoveQueue.RemoveAt(i);
		}
	}
	
	// Add any affects that need to be added
	if (_AddQueue.Num() > 0)
	{
		FRWScopeLock WriteLock(_EffectsLock, SLT_Write);
		for (int i = _AddQueue.Num() - 1; i >= 0; i--)
		{
			_CurrentEffects.Add( _AddQueue[i] );
			_AddQueue.RemoveAt(i);
		}
	}
}

/**
 * @brief Helper Function used to set the various timers in this class
 * @param TimerHandle The timer handle to be modified
 * @param TimerDelegate The delegate for timer functionality
 * @param TickRate The tick rate for the timer
 */
void UVitalityEffectsComponent::InitializeTimer(FTimerHandle& TimerHandle,
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
void UVitalityEffectsComponent::CancelTimer(FTimerHandle& TimerHandle) const
{
	if (GetWorld()->GetTimerManager().TimerExists(TimerHandle))
	{
		if (TimerHandle.IsValid())
			TimerHandle.Invalidate();
	}
}

/**
 * @brief Creates a unique ID that does not exist in the active effects array
 * @return The unique ID generated, or zero on failure
 */
int UVitalityEffectsComponent::GenerateUniqueId()
{
	bool idExists = false;
	FRWScopeLock ReadLock(_EffectsLock, SLT_ReadOnly);
	while (idExists)
	{
		const int randomNumber = FMath::RandRange(1,INT_MAX);
		for (const FStVitalityEffects& vEffect : _CurrentEffects)
		{
			if (vEffect.uniqueId == randomNumber)
			{
				idExists = true;
				break;
			}
		}
		if (!idExists)
			return randomNumber;
	}
	return 0;
}

/**
 * @brief Ruins on the owning client, triggering the proper delegates
 * @param OldArray The effects that were active before the update occurred
 */
void UVitalityEffectsComponent::OnRep_CurrentEffectsChanged_Implementation(
	const TArray<FStVitalityEffects>& OldArray)
{
	// Arrays that track effects by Unique ID (KEY) and EffectName (VALUE)
	TMap<int, FName> AddedBenefitEffects;
	TMap<int, FName> AddedDetrimentEffects;
	TMap<int, FName> RemovedBenefitEffects;
	TMap<int, FName> RemovedDetrimentEffects;

	// Add all preexisting entries
	for (const FStVitalityEffects& OldEntry : OldArray)
	{
		if (OldEntry.benefitEffect != EEffectsBeneficial::NONE)
			RemovedBenefitEffects.Add(OldEntry.uniqueId, OldEntry.properName);
		else
			RemovedDetrimentEffects.Add(OldEntry.uniqueId, OldEntry.properName);
	}

	// Remove any entries that still exist after the server update
	// and also add all current effects
	{
		// Mutex locks are required on current effects array
		FRWScopeLock ReadLock(_EffectsLock, SLT_ReadOnly);
		for (const FStVitalityEffects& CurrentEntry : _CurrentEffects)
		{
			if (CurrentEntry.benefitEffect != EEffectsBeneficial::NONE)
			{
				RemovedBenefitEffects.Add(CurrentEntry.uniqueId, CurrentEntry.properName);
				AddedBenefitEffects.Add(CurrentEntry.uniqueId, CurrentEntry.properName);
			}
			else
			{
				RemovedDetrimentEffects.Add(CurrentEntry.uniqueId, CurrentEntry.properName);
				AddedDetrimentEffects.Add(CurrentEntry.uniqueId, CurrentEntry.properName);
			}
		}
	}
	
	// Remove any current effects that existed prior to the update
	for (const FStVitalityEffects& OldEntry : OldArray)
	{
		if (OldEntry.benefitEffect != EEffectsBeneficial::NONE)
			AddedBenefitEffects.Remove(OldEntry.uniqueId);
		else
			AddedDetrimentEffects.Remove(OldEntry.uniqueId);
	}

	// Trigger Delegates	
	for (const TPair<int, FName>& RemovedEffect : AddedBenefitEffects)
		OnEffectBeneficialApplied.Broadcast(RemovedEffect.Key, RemovedEffect.Value);
	
	for (const TPair<int, FName>& RemovedEffect : AddedDetrimentEffects)
		OnEffectBeneficialApplied.Broadcast(RemovedEffect.Key, RemovedEffect.Value);
	
	for (const TPair<int, FName>& RemovedEffect : RemovedBenefitEffects)
		OnEffectBeneficialExpired.Broadcast(RemovedEffect.Key, RemovedEffect.Value);
	
	for (const TPair<int, FName>& RemovedEffect : RemovedDetrimentEffects)
		OnEffectDetrimentalExpired.Broadcast(RemovedEffect.Key, RemovedEffect.Value);
}

