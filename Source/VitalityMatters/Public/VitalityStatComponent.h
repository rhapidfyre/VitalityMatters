// Copyright Take Five Games, LLC 2023 - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Delegates/Delegate.h"
#include "lib/VitalityData.h"

#include "lib/VitalityEnums.h"

#include "VitalityStatComponent.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCoreStatModified,     EVitalityStat, VitalityStat);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamageBonusModified,  EDamageType,   DamageEnum);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamageResistModified, EDamageType,   DamageEnum);


/**
 * Manages all of the Stat-specific members of an actor
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class VITALITYMATTERS_API UVitalityStatComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:

	UVitalityStatComponent();
	
	UFUNCTION(BlueprintPure) float GetTotalResistance(EDamageType DamageEnum = EDamageType::ADMIN);
	UFUNCTION(BlueprintPure) float GetTotalDamageBonus(EDamageType DamageEnum = EDamageType::ADMIN);

	UFUNCTION(BlueprintCallable) bool SetNaturalResistanceValue(EDamageType DamageEnum, int NewValue = 0);
	UFUNCTION(BlueprintCallable) bool SetGearResistanceValue(EDamageType DamageEnum, int NewValue = 0);
	UFUNCTION(BlueprintCallable) bool SetMagicalResistanceValue(EDamageType DamageEnum, int NewValue = 0);
	UFUNCTION(BlueprintCallable) bool SetOtherResistanceValue(EDamageType DamageEnum, int NewValue = 0);
	
	UFUNCTION(BlueprintPure) float GetNaturalResistanceValue(EDamageType DamageEnum);
	UFUNCTION(BlueprintPure) float GetGearResistanceValue(EDamageType DamageEnum);
	UFUNCTION(BlueprintPure) float GetMagicalResistanceValue(EDamageType DamageEnum);
	UFUNCTION(BlueprintPure) float GetOtherResistanceValue(EDamageType DamageEnum);
	
	UFUNCTION(BlueprintCallable) bool SetNaturalDamageBonusValue(EDamageType DamageEnum, int NewValue = 0);
	UFUNCTION(BlueprintCallable) bool SetGearDamageBonusValue(EDamageType DamageEnum, int NewValue = 0);
	UFUNCTION(BlueprintCallable) bool SetMagicalDamageBonusValue(EDamageType DamageEnum, int NewValue = 0);
	UFUNCTION(BlueprintCallable) bool SetOtherDamageBonusValue(EDamageType DamageEnum, int NewValue = 0);
	
	UFUNCTION(BlueprintPure) float GetNaturalDamageBonusValue(EDamageType DamageEnum);
	UFUNCTION(BlueprintPure) float GetGearDamageBonusValue(EDamageType DamageEnum);
	UFUNCTION(BlueprintPure) float GetMagicalDamageBonusValue(EDamageType DamageEnum);
	UFUNCTION(BlueprintPure) float GetOtherDamageBonusValue(EDamageType DamageEnum);
	
	UFUNCTION(BlueprintCallable) bool SetNaturalCoreStat(EVitalityStat StatEnum, float NewValue = 0.f);
	UFUNCTION(BlueprintCallable) bool SetGearCoreStat(EVitalityStat StatEnum, float NewValue = 0.f);
	UFUNCTION(BlueprintCallable) bool SetMagicalCoreStat(EVitalityStat StatEnum, float NewValue = 0.f);
	UFUNCTION(BlueprintCallable) bool SetOtherCoreStat(EVitalityStat StatEnum, float NewValue = 0.f);

	UFUNCTION(BlueprintPure) float GetTotalCoreStat(EVitalityStat StatEnum);
	UFUNCTION(BlueprintPure) float GetNaturalCoreStat(EVitalityStat StatEnum);
	UFUNCTION(BlueprintPure) float GetGearCoreStat(EVitalityStat StatEnum);
	UFUNCTION(BlueprintPure) float GetMagicalCoreStat(EVitalityStat StatEnum);
	UFUNCTION(BlueprintPure) float GetOtherCoreStat(EVitalityStat StatEnum);

	UFUNCTION(BlueprintCallable) void ReloadSettings();

	UFUNCTION(BlueprintPure) FStVitalityStats GetAllNaturalStats() const	{ return _BaseStats; }
	UFUNCTION(BlueprintPure) FStVitalityStats GetAllGearStats() const		{ return _GearStats; }
	UFUNCTION(BlueprintPure) FStVitalityStats GetAllModifiedStats() const	{ return _ModifiedStats; }
	UFUNCTION(BlueprintPure) FStVitalityStats GetAllOtherStats() const		{ return _OtherStats; }
	
protected:

	virtual void BeginPlay() override;

	virtual void OnComponentCreated() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	
	// Helper function for traversing a vitality stats array
	FStVitalityDamageMap* FindDamageResistanceMap(
		FStVitalityStats& ArrayReference, const EDamageType DamageEnum);
	
	// Helper function for traversing a vitality stats array
	FStVitalityDamageMap* FindDamageBonusMap(
		FStVitalityStats& ArrayReference, const EDamageType DamageEnum);

	// Helper function for traversing a core stats array
	FStVitalityStatMap* FindCoreStatsMap(
		FStVitalityStats& ArrayReference, const EVitalityStat StatEnum);
	
	// Helper function for updating damage resistance or adding if it doesn't exist
	bool SetNewDamageResistanceValue(FStVitalityStats& StatsMap,
		const EDamageType DamageEnum, const int NewValue);

	// Helper function for updating damage bonus or adding if it doesn't exist
	bool SetNewDamageBonusValue(FStVitalityStats& StatsMap,
		const EDamageType DamageEnum, const int NewValue);
	
	// Helper function for updating core stats or adding if it doesn't exist
	bool SetNewCoreStatsValue(FStVitalityStats& StatsMap,
		const EVitalityStat StatEnum, const int NewValue);

	
	// Helper function for getting damage resistance
	float GetDamageResistanceValue(const FStVitalityStats& StatsMap,
		const EDamageType DamageEnum) const;
	
	// Helper function for getting damage bonus
	float GetDamageBonusValue(const FStVitalityStats& StatsMap,
		const EDamageType DamageEnum) const;
	
	// Helper function for getting core stats
	float GetCoreStatValue(const FStVitalityStats& StatsMap,
		const EVitalityStat StatEnum) const;

	void NaturalCoreStatUpdated(const EVitalityStat CoreStat);
	void GearCoreStatUpdated(const EVitalityStat CoreStat);
	void MagicCoreStatUpdated(const EVitalityStat CoreStat);
	void OtherCoreStatUpdated(const EVitalityStat CoreStat);
	
	void NaturalDamageBonusUpdated(const EDamageType DamageEnum);
	void GearDamageBonusUpdated(const EDamageType DamageEnum);
	void MagicDamageBonusUpdated(const EDamageType DamageEnum);
	void OtherDamageBonusUpdated(const EDamageType DamageEnum);
	
	void NaturalDamageResistUpdated(const EDamageType DamageEnum);
	void GearDamageResistUpdated(const EDamageType DamageEnum);
	void MagicDamageResistUpdated(const EDamageType DamageEnum);
	void OtherDamageResistUpdated(const EDamageType DamageEnum);
	
	void BindListenerEvents();
	
public:
	
	UPROPERTY(BlueprintAssignable) FOnCoreStatUpdated		OnCoreStatModified;
	UPROPERTY(BlueprintAssignable) FOnDamageBonusUpdated	OnDamageBonusUpdated;
	UPROPERTY(BlueprintAssignable) FOnDamageResistUpdated	OnDamageResistUpdated;

private:

	float _CurrentSpeedSprint	= 0.f;
	float _CurrentSpeedRun		= 0.f;
	float _CurrentSpeedWalk		= 0.f;
	float _CurrentSpeedSwim		= 0.f;
	float _CurrentSpeedCrouch	= 0.f;

	float _MaxSprintSpeed = 0.f;
	float _MaxRunSpeed    = 0.f;
	float _MaxWalkSpeed   = 0.f;
	float _MaxSwimSpeed   = 0.f;
	float _MaxCrouchSpeed = 0.f;

	// The natural value of the actors stats including progression
	UPROPERTY(Replicated) FStVitalityStats _BaseStats;

	// Stats modified by equipment in the player's possession
	UPROPERTY(Replicated) FStVitalityStats _GearStats;

	// Stats modified by magical effects on this actor
	UPROPERTY(Replicated) FStVitalityStats _ModifiedStats;

	// Stats modified by other reasons (environmental, handicaps, etc)
	UPROPERTY(Replicated) FStVitalityStats _OtherStats;

};
