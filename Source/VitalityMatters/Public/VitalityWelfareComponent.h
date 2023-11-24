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
	FOnCaloriesUpdated,		float, CurrentValue, float, NewValue, float, ValueAsPercent);


/**
 * Manages all of the Stat-specific members of an actor
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class VITALITYMATTERS_API UVitalityWelfareComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	
	UVitalityWelfareComponent() { SetIsReplicatedByDefault(true); };

	/* Utility & Helper Functions */
	
	UFUNCTION(BlueprintCallable) float DamageHealth(AActor* DamageInstigator = nullptr, float DamageTaken = 0.f);
	UFUNCTION(BlueprintCallable) float DamageStamina(AActor* DamageInstigator = nullptr, float DamageTaken = 0.f);
	UFUNCTION(BlueprintCallable) float DamageMagic(AActor* DamageInstigator = nullptr, float DamageTaken = 0.f);

	UFUNCTION(BlueprintCallable) bool StartTimerForCategory(EVitalityCategory VitalityCategory);
	UFUNCTION(BlueprintCallable) bool CancelTimerForCategory(EVitalityCategory VitalityCategory);
	UFUNCTION(BlueprintCallable) bool PauseTimerForCategory(EVitalityCategory VitalityCategory, bool PauseTimer = true);

	/* Getter Functions / Accessors */

	UFUNCTION(BlueprintPure) bool GetIsDead() const { return _IsDead; }
	UFUNCTION(BlueprintPure) ECombatState GetCombatState() const { return _CombatState; };
	UFUNCTION(BlueprintPure) TArray<FStDamageData> GetDamageHistory() const { return _DamageHistory; }

	UFUNCTION(BlueprintPure) float GetHealthPercent() const;
	UFUNCTION(BlueprintPure) float GetHealthValue() const { return _HealthCurrent; }
	UFUNCTION(BlueprintPure) float GetCurrentHealth(float& CurrentValue, float& MaxValue) const;
	
	UFUNCTION(BlueprintPure) float GetStaminaPercent() const;
	UFUNCTION(BlueprintPure) float GetStaminaValue() const { return _StaminaCurrent; }
	UFUNCTION(BlueprintPure) float GetCurrentStamina(float& CurrentValue, float& MaxValue) const;
	
	UFUNCTION(BlueprintPure) float GetMagicPercent() const;
	UFUNCTION(BlueprintPure) float GetMagicValue() const { return _MagicCurrent; }
	UFUNCTION(BlueprintPure) float GetCurrentMagic(float& CurrentValue, float& MaxValue) const;
	
	UFUNCTION(BlueprintPure) float GetHydrationPercent() const;
	UFUNCTION(BlueprintPure) float GetHydrationValue() const { return _HydrationCurrent; }
	UFUNCTION(BlueprintPure) float GetCurrentHydration(float& CurrentValue, float& MaxValue) const;
	
	UFUNCTION(BlueprintPure) float GetHungerPercent() const;
	UFUNCTION(BlueprintPure) float GetHungerValue() const { return _CaloriesCurrent; }
	UFUNCTION(BlueprintPure) float GetCurrentHunger(float& CurrentValue, float& MaxValue) const;
	
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
	void Multicast_VitalityDeath(AActor* DamageInstigator = nullptr);
	
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UAnimMontage*> DeathAnimations;
	
	// If FALSE, the welfare component will NOT have health-related functionality
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Settings")
	bool UseHealthSubsystem = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Settings")
	float StartingHealthCurrent = 0.f;
	// The value that the maximum health should be when LoadSettings() is called
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Settings")
	float StartingHealthMaximum = 0.f;
	// The value that the passive health regen should be when LoadSettings() is called
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Settings")
	float PassiveHealthRegen = 1.f;
	// The rate of the health tick timer when LoadSettings() is called
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Settings")
	float HealthTimerTickRate = 0.5;

	// If FALSE, the welfare component will NOT have stamina-related functionality
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Settings")
	bool UseStaminaSubsystem = true;
	// The value that the current stamina should be when LoadSettings() is called
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina Settings")
	float StartingStaminaCurrent = 0.f;
	// The value that the maximum stamina should be when LoadSettings() is called
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina Settings")
	float StartingStaminaMaximum = 0.f;
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
	float StartingMagicCurrent = 0.f;
	// The value that the maximum magic should be when LoadSettings() is called
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic Settings")
	float StartingMagicMaximum = 0.f;
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
	float StartingHydrationCurrent = 0.f;
	// The value that the current magic should be when LoadSettings() is called
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival Settings")
	float StartingHungerCurrent = 0.f;
	// The value that the maximum magic should be when LoadSettings() is called
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival Settings")
	float StartingHydrationMaximum = 0.f;
	// The value that the maximum magic should be when LoadSettings() is called
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival Settings")
	float StartingHungerMaximum = 0.f;
	// The value that the passive magic drain should be when LoadSettings() is called
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival Settings")
	float PassiveHydrationDrain = 0.082;
	// The value that the passive magic drain should be when LoadSettings() is called
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival Settings")
	float PassiveHungerDrain = 0.082;
	// The rate of the magic tick timer when LoadSettings() is called
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival Settings")
	float HydrationTimerTickRate = 0.5;
	// The rate of the magic tick timer when LoadSettings() is called
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival Settings")
	float CaloriesTimerTickRate = 0.5;
	
private:

	/* Timers */
	
	// Timers that manage regeneration & resetting values
	UPROPERTY() FTimerHandle _HealthTimer;
	UPROPERTY() FTimerHandle _MagicTimer;
	UPROPERTY() FTimerHandle _StaminaTimer;
	UPROPERTY() FTimerHandle _CaloriesTimer;
	UPROPERTY() FTimerHandle _HydrationTimer;
	UPROPERTY() FTimerHandle _CombatTimer;
		
	/* Replicated Members */

	UPROPERTY(Replicated) TArray<FStDamageData> _DamageHistory;
	
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_IsDeadChanged)
	bool  _IsDead        = false;
	
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_CombatStateChanged)
	ECombatState _CombatState = ECombatState::RELAXED;
	
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_HealthValueChanged)
	float _HealthCurrent	= 1.f;
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_HealthMaxChanged)
	float _HealthMax		= 1.f;
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_MagicValueChanged)
	float _MagicCurrent		= 1.f;
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_MagicMaxChanged)
	float _MagicMax			= 1.f;
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_StaminaValueChanged)
	float _StaminaCurrent	= 1.f;
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_StaminaMaxChanged)
	float _StaminaMax		= 1.f;
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_HydrationValueChanged)
	float _HydrationCurrent = 1.f;
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_HydrationMaxChanged)
	float _HydrationMax		= 500.f;
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_CaloriesValueChanged)
	float _CaloriesCurrent  = 1.f;	
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_CaloriesMaxChanged)
	float _CaloriesMax		= 500.f;

	/* Non-Replicated Members */
	
	float _HealthRegenAtRest	= 1.f;
	float _MagicRegenAtRest		= 1.f;
	float _StaminaRegenAtRest	= 1.f;
	float _CaloriesDrainAtRest  = 0.082;
	float _HydrationDrainAtRest = 0.082;

	float _HealthTimerTickRate		= 1.f;
	float _StaminaTimerTickRate		= 1.f;
	float _MagicTimerTickRate		= 1.f;
	float _HydrationTimerTickRate	= 1.f;
	float _HungerTimerTickRate		= 1.f;
	
};