// Copyright Take Five Games, LLC 2023 - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Delegates/Delegate.h"
#include "lib/VitalityData.h"

#include "lib/VitalityEnums.h"

#include "VitalityStatComponent.generated.h"


/**
 * Manages all of the Stat-specific members of an actor
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class VITALITYMATTERS_API UVitalityStatComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:

	UVitalityStatComponent();
	
	TMulticastDelegate<void(bool)> OnInventorySaved;

	bool LoadStatsFromSave(FString& ResponseString, FString SaveSlotName, bool isAsync);
	
	UFUNCTION(BlueprintCallable) void Reinitialize();
	
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

	void InitializeCoreStats(float StrengthValue, float AgilityValue, float FortitudeValue,
		float IntellectValue, float AstutenessValue, float CharismaValue);
	void InitializeNaturalDamageBonuses(const TArray<float>& DamageMap);
	void InitializeNaturalDamageResists(const TArray<float>& DamageMap);

	UFUNCTION(Server, Reliable)
	void Server_InitializeCoreStats(
		float StrengthValue, float AgilityValue, float FortitudeValue,
		float IntellectValue, float AstutenessValue, float CharismaValue);
	UFUNCTION(Server, Reliable)
	void Server_InitializeNaturalDamageBonuses(const TArray<float>& DamageMap);
	UFUNCTION(Server, Reliable)
	void Server_InitializeNaturalDamageResists(const TArray<float>& DamageMap);
	
	UFUNCTION(BlueprintPure) FStVitalityStats GetAllNaturalStats() const	{ return BaseStats_; }
	UFUNCTION(BlueprintPure) FStVitalityStats GetAllGearStats() const		{ return GearStats_; }
	UFUNCTION(BlueprintPure) FStVitalityStats GetAllModifiedStats() const	{ return ModifiedStats_; }
	UFUNCTION(BlueprintPure) FStVitalityStats GetAllOtherStats() const		{ return OtherStats_; }
	
protected:

	virtual void BeginPlay() override;

	virtual void OnComponentCreated() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:

	UFUNCTION()
	void LoadDataDelegate(const FString& SaveSlotName, int32 UserIndex, USaveGame* SaveData);
	
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
	float GetDamageResistanceValue(
		const FStVitalityStats& StatsMap, const EDamageType DamageEnum) const;
	
	// Helper function for getting damage bonus
	float GetDamageBonusValue(
		const FStVitalityStats& StatsMap, const EDamageType DamageEnum) const;
	
	// Helper function for getting core stats
	float GetCoreStatValue(
		const FStVitalityStats& StatsMap, const EVitalityStat StatEnum) const;

	UFUNCTION() void NaturalCoreStatUpdated(const EVitalityStat CoreStat);
	UFUNCTION() void GearCoreStatUpdated(const EVitalityStat CoreStat);
	UFUNCTION() void MagicCoreStatUpdated(const EVitalityStat CoreStat);
	UFUNCTION() void OtherCoreStatUpdated(const EVitalityStat CoreStat);
	
	UFUNCTION() void NaturalDamageBonusUpdated(const EDamageType DamageEnum);
	UFUNCTION() void GearDamageBonusUpdated(const EDamageType DamageEnum);
	UFUNCTION() void MagicDamageBonusUpdated(const EDamageType DamageEnum);
	UFUNCTION() void OtherDamageBonusUpdated(const EDamageType DamageEnum);
	
	UFUNCTION() void NaturalDamageResistUpdated(const EDamageType DamageEnum);
	UFUNCTION() void GearDamageResistUpdated(const EDamageType DamageEnum);
	UFUNCTION() void MagicDamageResistUpdated(const EDamageType DamageEnum);
	UFUNCTION() void OtherDamageResistUpdated(const EDamageType DamageEnum);
	
	void BindListenerEvents();
	
public:
	
	UPROPERTY(BlueprintAssignable) FOnCoreStatUpdated		OnCoreStatModified;
	UPROPERTY(BlueprintAssignable) FOnDamageBonusUpdated	OnDamageBonusUpdated;
	UPROPERTY(BlueprintAssignable) FOnDamageResistUpdated	OnDamageResistUpdated;

	UPROPERTY(BlueprintReadWrite, EditAnywhere) FStVitalityStats StartingStats;

private:

	bool bStatsSystemReady = false;
	bool bDamageBonusesReady = false;
	bool bDamageResistsReady = false;
	
	float CurrentSpeedSprint_	= 0.f;
	float CurrentSpeedRun_		= 0.f;
	float CurrentSpeedWalk_		= 0.f;
	float CurrentSpeedSwim_		= 0.f;
	float CurrentSpeedCrouch_	= 0.f;

	float MaxSprintSpeed_ = 0.f;
	float MaxRunSpeed_    = 0.f;
	float MaxWalkSpeed_   = 0.f;
	float MaxSwimSpeed_   = 0.f;
	float MaxCrouchSpeed_ = 0.f;

	bool bSavesOnServerOnly = false;
	FString StatsSaveName_ = "";
	int32 StatsSaveUserIndex_ = 0;

	void StatsEventTrigger(	const FStVitalityStats* OldStats,
							const FStVitalityStats* NewStats);

	UFUNCTION(Client, Reliable)
	void OnRep_BaseStatsChanged(FStVitalityStats OldBaseStats);
	// The natural value of the actors stats including progression
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_BaseStatsChanged)
	FStVitalityStats BaseStats_;

	UFUNCTION(Client, Reliable)
	void OnRep_GearStatsChanged(FStVitalityStats OldGearStats);
	// Stats modified by equipment in the player's possession
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_GearStatsChanged)
	FStVitalityStats GearStats_;

	UFUNCTION(Client, Reliable)
	void OnRep_ModifiedStatsChanged(FStVitalityStats OldModifiedStats);
	// Stats modified by magical effects on this actor
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_ModifiedStatsChanged)
	FStVitalityStats ModifiedStats_;

	UFUNCTION(Client, Reliable)
	void OnRep_OtherStatsChanged(FStVitalityStats OldOtherStats);
	// Stats modified by other reasons (environmental, handicaps, etc)
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_OtherStatsChanged)
	FStVitalityStats OtherStats_;

};
