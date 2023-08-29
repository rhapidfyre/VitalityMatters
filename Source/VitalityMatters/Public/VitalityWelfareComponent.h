// Copyright Take Five Games, LLC 2023 - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Delegates/Delegate.h"

#include "lib/VitalityEnums.h"

#include "VitalityWelfareComponent.generated.h"

// Called when the combat state has changed
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnCombatStateChanged,	ECombatState, OldCombatState, ECombatState, NewState);

// Called whenever the actor takes damage, whether it kills them or not.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FOnDamageTaken,		AActor*, DamageTaker, AActor*, DamageInstigator, float, DamageTaken);

// Called whenever the actor is healed, whether dead or not
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnHealed,			AActor*, HealthInstigator, float, HealthRecovered);

// Called when the character is killed, or the object is destroyed
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnDeath,				AActor*, KillingActor);

// Called whenever the current health value has changed, no matter the cause
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnHealthUpdated,		float, OldValue, float, NewValue);
// Called whenever the current stamina value has changed, no matter the cause
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnStaminaUpdated,		float, OldValue, float, NewValue);
// Called whenever the current magic value has changed, no matter the cause
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnMagicUpdated,		float, OldValue, float, NewValue);
// Called whenever the current hydration value has changed, no matter the cause
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnHydrationUpdated,	float, OldValue, float, NewValue);
// Called whenever the current calorie value has changed, no matter the cause
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnCaloriesUpdated,		float, OldValue, float, NewValue);


/**
 * Manages all of the Stat-specific members of an actor
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class VITALITYMATTERS_API UVitalityWelfareComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	
	UVitalityWelfareComponent() {};

	/* Utility & Helper Functions */
	
	UFUNCTION(BlueprintCallable)
	float DamageHealth(AActor* DamageActor = nullptr, float DamageTaken = 0.f);

	UFUNCTION(BlueprintCallable)
	float ConsumeStamina(AActor* DamageActor = nullptr, float DamageTaken = 0.f);

	UFUNCTION(BlueprintCallable)
	float ConsumeMagic(AActor* DamageActor = nullptr, float DamageTaken = 0.f);
	
	/* Getter Functions / Accessors */
	
	UFUNCTION(BlueprintCallable)
	ECombatState GetCombatState() const { return _CombatState; }
	
	/* Setter Functions / Mutators */
	
	UFUNCTION(BlueprintCallable)
	void SetCombatState(ECombatState CombatState);

protected:
	
	virtual void BeginPlay() override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void InitializeTimer(FTimerHandle& timerHandle, FTimerDelegate timerDelegate);

	void CancelTimer(FTimerHandle& timerHandle);
	
	UFUNCTION() void DowngradeCombatState();
	
	// Handles stamina decrease, stamina regen and sprinting logic
	virtual void TickStamina();

	// Handles health regeneration
	virtual void TickHealth();

	// Handles passive consumption of Calories & hunger effect
	virtual void TickCalories();

	// Handles passive consumption of Hydration & thirst effect
	virtual void TickHydration();
	
private:
	
	/* Replication Callbacks */
	
	UFUNCTION(NetMulticast, Reliable)
	void OnRep_CombatStateChanged(ECombatState OldCombatState);
	UFUNCTION(Client, Reliable) void OnRep_HealthValueChanged();
	UFUNCTION(Client, Reliable) void OnRep_MagicValueChanged();
	UFUNCTION(Client, Reliable) void OnRep_StaminaValueChanged();
	UFUNCTION(Client, Reliable) void OnRep_CaloriesValueChanged();
	UFUNCTION(Client, Reliable) void OnRep_HydrationValueChanged();
	
	/** Sent to all clients from server when the DamageHealth() function runs
	 * successfully, but the character survives the damage. Used to trigger clientside events.
	 * May arrive prior to the mHealthValue actually being changed.
	 */
	UFUNCTION(NetMulticast, Unreliable) void Multicast_DamageTaken(
		AActor* DamageTaker = nullptr, AActor* DamageInstigator = nullptr, float DamageTaken = 0.f);
	
	/** Sent to all clients from server when the DamageHealth() function runs
	 * successfully, and the character dies as a result. Used to trigger clientside events.
	 * May arrive prior to the mHealthValue actually being changed.
	 */
	UFUNCTION(Client, Unreliable) void Multicast_VitalityDeath(AActor* DamageActor = nullptr);
	
	
	
public:

	// Called whenever the actor takes damage to their health, regardless of cause
	UPROPERTY(BlueprintAssignable, Category = "Vitality Events")
	FOnDamageTaken OnDamageTaken;

	// Called whenever health is increased up to but not exceeding the maximum
	UPROPERTY(BlueprintAssignable, Category = "Vitality Events")
	FOnHealed OnHealed;

	// Called whenever health reaches zero or less and was not previously dead
	UPROPERTY(BlueprintAssignable, Category = "Vitality Events")
	FOnDeath OnDeath;

	// Called whenever the actors health or max health changes, for any reason.
	UPROPERTY(BlueprintAssignable, Category = "Vitality Events")
	FOnHealthUpdated		OnHealthUpdated;
	
	// Called whenever the actors stamina or max stamina changes, for any reason.
	UPROPERTY(BlueprintAssignable, Category = "Vitality Events")
	FOnStaminaUpdated		OnStaminaUpdated;
	
	// Called whenever the actors magic or max magic changes, for any reason.
	UPROPERTY(BlueprintAssignable, Category = "Vitality Events")
	FOnMagicUpdated			OnMagicUpdated;
	
	// Called whenever the actors hydration or max hydration changes, for any reason.
	UPROPERTY(BlueprintAssignable, Category = "Vitality Events")
	FOnHydrationUpdated		OnHydrationUpdated;
	
	// Called whenever the actors calories or max calories changes, for any reason.
	UPROPERTY(BlueprintAssignable, Category = "Vitality Events")
	FOnCaloriesUpdated		OnCaloriesUpdated;

	// Called anytime the combat state changes, for any reason
	UPROPERTY(BlueprintAssignable, Category = "Vitality Events")
	FOnCombatStateChanged	OnCombatStateChanged;

	
private:

	/* Timers */
	
	// Timers that manage regeneration & resetting values
	UPROPERTY() FTimerHandle _HealthTimer;
	UPROPERTY() FTimerHandle _ManaTimer;
	UPROPERTY() FTimerHandle _StaminaTimer;
	UPROPERTY() FTimerHandle _CaloriesTimer;
	UPROPERTY() FTimerHandle _HydrationTimer;
	UPROPERTY() FTimerHandle _CombatTimer;
	
	// Timers handling cool downs before regen/reset can occur
	UPROPERTY() FTimerHandle _StaminaCooldownTimer;
		
	/* Replicated Members */
	
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_CombatStateChanged)
	ECombatState _CombatState = ECombatState::RELAXED;
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_HealthValueChanged)
	float _HealthCurrent	= 1.f;
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_MagicValueChanged)
	float _MagicCurrent		= 1.f;
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_StaminaValueChanged)
	float _StaminaCurrent	= 1.f;
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_CaloriesValueChanged)
	float _CaloriesCurrent  = 1.f;
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_HydrationValueChanged)
	float _HydrationCurrent = 1.f;
	
	/* Replicated Maximums */
	
	UPROPERTY(Replicated) float _HealthMax		= 1.f;
	UPROPERTY(Replicated) float _MagicMax		= 1.f;  
	UPROPERTY(Replicated) float _StaminaMax		= 1.f;  
	UPROPERTY(Replicated) float _CaloriesMax	= 500.f;
	UPROPERTY(Replicated) float _HydrationMax	= 500.f;

	/* Non-Replicated Members */
	
	float _HealthRegenAtRest	= 1.f;
	float _ManaRegenAtRest		= 1.f;
	float _StaminaRegenAtRest	= 1.f;
	
	float _CaloriesDrainAtRest  = 0.082;
	float _HydrationDrainAtRest = 0.082;
	
};