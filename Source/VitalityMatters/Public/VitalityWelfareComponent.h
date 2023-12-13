// Copyright Take Five Games, LLC 2023 - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Delegates/Delegate.h"

#include "lib/VitalityData.h"
#include "lib/VitalityEnums.h"

#include "VitalityWelfareComponent.generated.h"

class UVitalityEffectsComponent;
class UVitalityStatComponent;


// Called when the combat state has changed
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnCombatStateChanged,	ECombatState, OldCombatState, ECombatState, NewState);

// Called whenever the actor takes damage, whether it kills them or not.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnDamageTaken,			AActor*, DamageInstigator, float, DamageTaken);

// Called whenever the actor is healed, whether dead or not
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnHealed,				AActor*, HealthInstigator, float, HealthRecovered);

// Called when the character is killed, or the object is destroyed
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnDeath,				AActor*, KillingActor);

// Called when an actor has taken a hit, via multicast (runs on both Svr and Cli)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnHitAnimation,		UAnimMontage*, AnimMontage, USoundBase*, SoundBase);

// Called whenever the current health value has changed, no matter the cause
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FOnHealthUpdated,		float, CurrentValue, float, MaxValue, float, ValueAsPercent);
// Called whenever the current stamina value has changed, no matter the cause
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FOnStaminaUpdated,		float, CurrentValue, float, MaxValue, float, ValueAsPercent);
// Called whenever the current magic value has changed, no matter the cause
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FOnMagicUpdated,		float, CurrentValue, float, MaxValue, float, ValueAsPercent);
// Called whenever the current hydration value has changed, no matter the cause
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FOnHydrationUpdated,	float, CurrentValue, float, MaxValue, float, ValueAsPercent);
// Called whenever the current calorie value has changed, no matter the cause
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
FOnCaloriesUpdated,			float, CurrentValue, float, NewValue, float, ValueAsPercent);
// Called whenever the current calorie value has changed, no matter the cause


/**
 * Manages all of the Stat-specific members of an actor
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class VITALITYMATTERS_API UVitalityWelfareComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	
	UVitalityWelfareComponent()
	{
		SetIsReplicatedByDefault(true);
		SetupDefaultValues();
	};

	/* Utility & Helper Functions */

	void SetupDefaultValues();
	
	UFUNCTION(BlueprintCallable) float DamageHealth(AActor* DamageInstigator = nullptr, float DamageTaken = 0.f);
	UFUNCTION(BlueprintCallable) float DamageStamina(AActor* DamageInstigator = nullptr, float DamageTaken = 0.f);
	UFUNCTION(BlueprintCallable) float DamageMagic(AActor* DamageInstigator = nullptr, float DamageTaken = 0.f);

	UFUNCTION(BlueprintCallable) bool StartTimerForCategory(EVitalityCategory VitalityCategory);
	UFUNCTION(BlueprintCallable) bool CancelTimerForCategory(EVitalityCategory VitalityCategory);
	UFUNCTION(BlueprintCallable) bool PauseTimerForCategory(EVitalityCategory VitalityCategory, bool PauseTimer = true);

	/* Getter Functions / Accessors */

	UFUNCTION(BlueprintPure) bool GetIsDead() const { return IsDead_; }
	UFUNCTION(BlueprintPure) ECombatState GetCombatState() const { return CombatState_; };
	UFUNCTION(BlueprintPure) TArray<FStDamageData> GetDamageHistory() const { return DamageHistory_; }

	UFUNCTION(BlueprintPure) float GetVitalityStatData(EVitalityCategory VitalityCategory, float& CurrentValue, float& MaxValue) const;
	
	UFUNCTION(BlueprintPure) float GetHealthPercent() const;
	UFUNCTION(BlueprintPure) float GetHealthValue() const { return HealthCurrent_; }
	UFUNCTION(BlueprintPure) float GetCurrentHealth(float& CurrentValue, float& MaxValue) const;
	
	UFUNCTION(BlueprintPure) float GetStaminaPercent() const;
	UFUNCTION(BlueprintPure) float GetStaminaValue() const { return StaminaCurrent_; }
	UFUNCTION(BlueprintPure) float GetCurrentStamina(float& CurrentValue, float& MaxValue) const;
	
	UFUNCTION(BlueprintPure) float GetMagicPercent() const;
	UFUNCTION(BlueprintPure) float GetMagicValue() const { return MagicCurrent_; }
	UFUNCTION(BlueprintPure) float GetCurrentMagic(float& CurrentValue, float& MaxValue) const;
	
	UFUNCTION(BlueprintPure) float GetHydrationPercent() const;
	UFUNCTION(BlueprintPure) float GetHydrationValue() const { return HydrationCurrent_; }
	UFUNCTION(BlueprintPure) float GetCurrentHydration(float& CurrentValue, float& MaxValue) const;
	
	UFUNCTION(BlueprintPure) float GetHungerPercent() const;
	UFUNCTION(BlueprintPure) float GetHungerValue() const { return CaloriesCurrent_; }
	UFUNCTION(BlueprintPure) float GetCurrentHunger(float& CurrentValue, float& MaxValue) const;

	UFUNCTION(BlueprintCallable) void HitByWeapon();
	
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_HitByWeaponEffects(
		UAnimMontage* HitByWeaponAnim, USoundBase* HitByWeaponSound);
	
	/* Setter Functions / Mutators */
	
	UFUNCTION(BlueprintCallable) void IncreaseCombatState();
	UFUNCTION(BlueprintCallable) void DecreaseCombatState();
	UFUNCTION(BlueprintCallable) void ResetCombatState();
	UFUNCTION(BlueprintCallable) void SetCombatEngaged();
	UFUNCTION(BlueprintCallable) void SetCombatAlert();

	void InitializeHealthSubsystem(bool UseSubsystem = false, float NowValue = 0.f, float MaxValue = 0.f, float RegenRate = 0.f);
	void InitializeStaminaSubsystem(bool UseSubsystem = false, float NowValue = 0.f, float MaxValue = 0.f, float RegenRate = 0.f);
	void InitializeMagicSubsystem(bool UseSubsystem = false, float NowValue = 0.f, float MaxValue = 0.f, float RegenRate = 0.f);

	void InitializeSurvivalSubsystem(bool UseSubsystem = false,
		float NowHydrationValue = 0.f, float MaxHydrationValue = 0.f, float HydrationRegenRate = 0.f,
		float NowCaloriesValue = 0.f, float MaxCaloriesValue = 0.f, float CaloriesRegenRate = 0.f);

	void InitializeSubsystem(EVitalityCategory VitalityCategory,
		bool UseSubsystem = false, float NowValue = 0.f, float MaxValue = 0.f, float RegenRate = 0.f);
	UFUNCTION(Server, Reliable)	void Server_InitializeHealthSubsystem(bool UseSubsystem = false, float NowValue = 0.f, float MaxValue = 0.f, float RegenRate = 0.f);
	UFUNCTION(Server, Reliable)	void Server_InitializeStaminaSubsystem(bool UseSubsystem = false, float NowValue = 0.f, float MaxValue = 0.f, float RegenRate = 0.f);
	UFUNCTION(Server, Reliable)	void Server_InitializeMagicSubsystem(bool UseSubsystem = false, float NowValue = 0.f, float MaxValue = 0.f, float RegenRate = 0.f);
	UFUNCTION(Server, Reliable)	void Server_InitializeSurvivalSubsystem(bool UseSubsystem = false,
		float NowHydrationValue = 0.f, float MaxHydrationValue = 0.f, float HydrationRegenRate = 0.f,
		float NowCaloriesValue = 0.f, float MaxCaloriesValue = 0.f, float CaloriesRegenRate = 0.f);
	
	
protected:
	
	virtual void BeginPlay() override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void InitializeTimer(FTimerHandle& TimerHandle,
			FTimerDelegate TimerDelegate, float TickRate = 0.5) const;

	void CancelTimer(FTimerHandle& TimerHandle) const;
	
	// Handles stamina decrease, stamina regen and sprinting logic
	virtual void TickStamina();

	// Handles health regeneration
	virtual void TickHealth();

	// Handles magic regeneration
	virtual void TickMagic();

	// Handles passive consumption of Calories & hunger effect
	virtual void TickCalories();

	// Handles passive consumption of Hydration & thirst effect
	virtual void TickHydration();
	
private:
	
	/* Replication Callbacks */
	
	UFUNCTION(NetMulticast, Reliable)	void OnRep_IsDeadChanged(bool WasDeadBefore);
	UFUNCTION(NetMulticast, Reliable)	void OnRep_CombatStateChanged(ECombatState OldCombatState);
	UFUNCTION(NetMulticast, Reliable)	void OnRep_HealthValueChanged(float OldValue);
	UFUNCTION(NetMulticast, Reliable)	void OnRep_HealthMaxChanged(float OldValue);
	UFUNCTION(NetMulticast, Reliable)	void OnRep_MagicValueChanged(float OldValue);
	UFUNCTION(NetMulticast, Reliable)	void OnRep_MagicMaxChanged(float OldValue);
	UFUNCTION(NetMulticast, Reliable)	void OnRep_StaminaValueChanged(float OldValue);
	UFUNCTION(NetMulticast, Reliable)	void OnRep_StaminaMaxChanged(float OldValue);
	UFUNCTION(Client, Reliable)			void OnRep_CaloriesValueChanged(float OldValue);
	UFUNCTION(Client, Reliable)			void OnRep_CaloriesMaxChanged(float OldValue);
	UFUNCTION(Client, Reliable)			void OnRep_HydrationValueChanged(float OldValue);
	UFUNCTION(Client, Reliable)			void OnRep_HydrationMaxChanged(float OldValue);
	
	/** Sent to all clients from server when the DamageHealth() function runs
	 * successfully, but the character survives the damage. Used to trigger clientside events.
	 * May arrive prior to the mHealthValue actually being changed.
	 */
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_DamageTaken(AActor* DamageInstigator, float DamageTaken = 0.f);
	
	UFUNCTION(Client, Unreliable)
	void Multicast_VitalityDeath(AActor* DamageInstigator, UAnimMontage* DeathAnim, USoundBase* DeathSound);
	
	UFUNCTION(BlueprintCallable)
	void SetCombatState(ECombatState CombatState);
	
public:

	// Called whenever the actor takes damage to their health, including death
	UPROPERTY(BlueprintAssignable, Category = "Vitality Events")
	FOnDamageTaken			OnDamageTaken;

	// Called whenever health is increased up to but not exceeding the maximum
	UPROPERTY(BlueprintAssignable, Category = "Vitality Events")
	FOnHealed				OnHealed;

	// Called whenever health reaches zero or less and was not previously dead
	UPROPERTY(BlueprintAssignable, Category = "Vitality Events")
	FOnDeath				OnDeath;

	// Called whenever the actors health changes, for any reason.
	UPROPERTY(BlueprintAssignable, Category = "Vitality Events")
	FOnHealthUpdated		OnHealthUpdated;
	
	// Called whenever the actors stamina changes, for any reason.
	UPROPERTY(BlueprintAssignable, Category = "Vitality Events")
	FOnStaminaUpdated		OnStaminaUpdated;
	
	// Called whenever the actors magic changes, for any reason.
	UPROPERTY(BlueprintAssignable, Category = "Vitality Events")
	FOnMagicUpdated			OnMagicUpdated;
	
	// Called whenever the actors hydration changes, for any reason.
	UPROPERTY(BlueprintAssignable, Category = "Vitality Events")
	FOnHydrationUpdated		OnHydrationUpdated;
	
	// Called whenever the actors calories changes, for any reason.
	UPROPERTY(BlueprintAssignable, Category = "Vitality Events")
	FOnCaloriesUpdated		OnCaloriesUpdated;

	// Called anytime the combat state changes, for any reason
	UPROPERTY(BlueprintAssignable, Category = "Vitality Events")
	FOnCombatStateChanged	OnCombatStateChanged;

	// Called anytime the vitality system takes a hit
	UPROPERTY(BlueprintAssignable, Category = "Vitality Events")
	FOnHitAnimation OnHitAnimation;

	// An array of animations played when actor dies, chosen at random
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UAnimMontage*> DeathAnimations;

	// An array of sounds played when actor dies, chosen at random
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<USoundBase*> DeathSounds;

	// An array of animations played when actor gets hit, chosen at random
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UAnimMontage*> HitAnimations;

	// An array of sounds played when actor gets hit, chosen at random
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<USoundBase*> HitSounds;
	
	// If FALSE, the welfare component will NOT have health-related functionality
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Settings")
	bool UseHealthSubsystem = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Settings")
	float StartingHealthCurrent = 100.f;
	// The value that the maximum health should be when LoadSettings() is called
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Settings")
	float StartingHealthMaximum = 100.f;
	// The value that the passive health regen should be when LoadSettings() is called
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Settings")
	float PassiveHealthRegen = 0.25;
	// The rate of the health tick timer when LoadSettings() is called
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Settings")
	float HealthTimerTickRate = 0.5;

	// If FALSE, the welfare component will NOT have stamina-related functionality
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Settings")
	bool UseStaminaSubsystem = true;
	// The value that the current stamina should be when LoadSettings() is called
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina Settings")
	float StartingStaminaCurrent = 100.f;
	// The value that the maximum stamina should be when LoadSettings() is called
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina Settings")
	float StartingStaminaMaximum = 100.f;
	// The value that the passive stamina regen should be when LoadSettings() is called
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina Settings")
	float PassiveStaminaRegen = 0.082;
	// The rate of the stamina tick timer when LoadSettings() is called
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina Settings")
	float StaminaTimerTickRate = 0.5;

	// If FALSE, the welfare component will NOT have magic-related functionality
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Settings")
	bool UseMagicSubsystem = true;
	// The value that the current magic should be when LoadSettings() is called
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic Settings")
	float StartingMagicCurrent = 100.f;
	// The value that the maximum magic should be when LoadSettings() is called
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic Settings")
	float StartingMagicMaximum = 100.f;
	// The value that the passive magic regen should be when LoadSettings() is called
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic Settings")
	float PassiveMagicRegen = 0.082;
	// The rate of the magic tick timer when LoadSettings() is called
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic Settings")
	float MagicTimerTickRate = 0.5;

	// If FALSE, the welfare component will NOT have stamina-related functionality
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Settings")
	bool UseSurvivalSubsystem = false;
	// The value that the current magic should be when LoadSettings() is called
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival Settings")
	float StartingHydrationCurrent = 1000.f;
	// The value that the current magic should be when LoadSettings() is called
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival Settings")
	float StartingHungerCurrent = 1000.f;
	// The value that the maximum magic should be when LoadSettings() is called
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival Settings")
	float StartingHydrationMaximum = 10000.f;
	// The value that the maximum magic should be when LoadSettings() is called
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival Settings")
	float StartingHungerMaximum = 1000.f;
	// The value that the passive magic drain should be when LoadSettings() is called
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival Settings")
	float PassiveHydrationDrain = 0.082;
	// The value that the passive magic drain should be when LoadSettings() is called
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival Settings")
	float PassiveHungerDrain = 0.037;
	// The rate of the magic tick timer when LoadSettings() is called
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival Settings")
	float HydrationTimerTickRate = 0.5;
	// The rate of the magic tick timer when LoadSettings() is called
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival Settings")
	float CaloriesTimerTickRate = 0.5;
	
private:

	/* Timers */
	
	// Timers that manage regeneration & resetting values
	UPROPERTY() FTimerHandle HealthTimer_;
	UPROPERTY() FTimerHandle MagicTimer_;
	UPROPERTY() FTimerHandle StaminaTimer_;
	UPROPERTY() FTimerHandle CaloriesTimer_;
	UPROPERTY() FTimerHandle HydrationTimer_;
	UPROPERTY() FTimerHandle CombatTimer_;
		
	/* Replicated Members */

	UPROPERTY(Replicated) TArray<FStDamageData> DamageHistory_;
	
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_IsDeadChanged)
	bool  IsDead_        = false;
	
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_CombatStateChanged)
	ECombatState CombatState_ = ECombatState::RELAXED;
	
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_HealthValueChanged)
	float HealthCurrent_	= 1.f;
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_HealthMaxChanged)
	float HealthMax_		= 1.f;
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_MagicValueChanged)
	float MagicCurrent_		= 1.f;
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_MagicMaxChanged)
	float MagicMax_			= 1.f;
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_StaminaValueChanged)
	float StaminaCurrent_	= 1.f;
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_StaminaMaxChanged)
	float StaminaMax_		= 1.f;
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_HydrationValueChanged)
	float HydrationCurrent_ = 1.f;
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_HydrationMaxChanged)
	float HydrationMax_		= 500.f;
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_CaloriesValueChanged)
	float CaloriesCurrent_  = 1.f;	
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_CaloriesMaxChanged)
	float CaloriesMax_		= 500.f;

	/* Non-Replicated Members */
	
	float HealthRegenAtRest_	= 1.f;
	float MagicRegenAtRest_		= 1.f;
	float StaminaRegenAtRest_	= 1.f;
	float CaloriesDrainAtRest_  = 0.082;
	float HydrationDrainAtRest_ = 0.082;

	float HealthTimerTickRate_		= 1.f;
	float StaminaTimerTickRate_		= 1.f;
	float MagicTimerTickRate_		= 1.f;
	float HydrationTimerTickRate_	= 1.f;
	float HungerTimerTickRate_		= 1.f;
	
};