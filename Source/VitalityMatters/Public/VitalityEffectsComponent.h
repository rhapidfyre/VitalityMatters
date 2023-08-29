// Copyright Take Five Games, LLC 2023 - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Delegates/Delegate.h"
#include "lib/VitalityData.h"

#include "VitalityEffectsComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnEffectDetrimentalApplied,	int, UniqueId, FName, EffectName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnEffectBeneficialApplied,		int, UniqueId, FName, EffectName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnEffectDetrimentalExpired,	int, UniqueId, FName, EffectName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnEffectBeneficialExpired,		int, UniqueId, FName, EffectName);


/**
 * Manages all of the Stat-specific members of an actor
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class VITALITYMATTERS_API UVitalityEffectsComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:

	UVitalityEffectsComponent();
	
	UFUNCTION(BlueprintCallable) bool ApplyEffect(FName EffectName, int StackCount = 1);
	
	UFUNCTION(BlueprintPure) FStVitalityEffects GetEffectByUniqueId(int UniqueId) const;
	
	UFUNCTION(Blueprintcallable) bool RemoveEffect(FName EffectName, int RemoveCount);
	UFUNCTION(BlueprintCallable) bool RemoveEffectByUniqueId(int UniqueId = 0);
	UFUNCTION(BlueprintCallable) bool RemoveEffectAtIndex(int IndexNumber = 0);

	UFUNCTION(BlueprintCallable) bool ApplyEffectBeneficial(EEffectsBeneficial EffectBeneficial, int StackCount = 1);
	UFUNCTION(BlueprintCallable) bool RevokeEffectBeneficial(EEffectsBeneficial EffectBeneficial, int StackCount = 1);

	UFUNCTION(BlueprintCallable) bool ApplyEffectDetrimental(EEffectsDetrimental EffectDetrimental, int StackCount = 1);
	UFUNCTION(BlueprintCallable) bool RevokeEffectDetrimental(EEffectsDetrimental EffectDetrimental, int StackCount = 1);

	UFUNCTION(BlueprintCallable) int GetNumberOfActiveBenefits(EEffectsBeneficial BenefitEffect);
	UFUNCTION(BlueprintCallable) int GetNumberOfActiveDetriments(EEffectsDetrimental DetrimentEffect);
	UFUNCTION(BlueprintPure) int GetNumberOfActiveEffects() const { return _CurrentEffects.Num(); }

	UFUNCTION(BlueprintPure) TArray<FStVitalityEffects> GetAllActiveEffects() const { return _CurrentEffects; }
	UFUNCTION(BlueprintCallable) TArray<FStVitalityEffects> GetAllEffectsByDetriment(EEffectsDetrimental DetrimentEffect) const;
	UFUNCTION(BlueprintCallable) TArray<FStVitalityEffects> GetAllEffectsByBenefit(EEffectsBeneficial BenefitEffect) const;

	UFUNCTION(BlueprintPure) bool IsEffectActive(FName EffectName) const;
	UFUNCTION(BlueprintPure) bool IsEffectIdActive(int UniqueId) const;
	UFUNCTION(BlueprintPure) bool IsEffectBeneficialActive(EEffectsBeneficial EffectEnum) const;
	UFUNCTION(BlueprintPure) bool IsEffectDetrimentalActive(EEffectsDetrimental EffectEnum) const;

protected:
	
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	// Handles effects wearing off
	virtual void TickEffects();

private:
	
	int GenerateUniqueId();

	UFUNCTION(Client, Reliable)
	void OnRep_CurrentEffects();
	
public:
	
	UPROPERTY(BlueprintAssignable, Category = "Vitality Events")
	FOnEffectDetrimentalApplied OnEffectDetrimentalApplied;
	
	UPROPERTY(BlueprintAssignable, Category = "Vitality Events")
	FOnEffectBeneficialApplied OnEffectBeneficialApplied;
	
	UPROPERTY(BlueprintAssignable, Category = "Vitality Events")
	FOnEffectDetrimentalExpired OnEffectDetrimentalExpired;
	
	UPROPERTY(BlueprintAssignable, Category = "Vitality Events")
	FOnEffectBeneficialExpired OnEffectBeneficialExpired;
	
private:
	
	FRWLock mMutexLock;
	
	UPROPERTY() FTimerHandle _EffectsTimer;

	UPROPERTY(Replicated, ReplicatedUsing=OnRep_CurrentEffects)
	TArray<FStVitalityEffects> _CurrentEffects;

	
	TArray<FStVitalityEffects> mEffectsAddQueue;	// Thread safe add queue
	TArray<int> mEffectsRemoveQueue;				// Thread safe remove queue
	
	
};