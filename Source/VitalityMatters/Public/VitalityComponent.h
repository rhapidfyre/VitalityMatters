
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "lib/StatusEffects.h"
#include "lib/VitalityData.h"
#include "lib/VitalityEnums.h"

#include "VitalityComponent.generated.h"

// Called when the combat state has changed
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnCombatStateChanged,	ECombatState, OldCombatState, ECombatState, NewState);

// Called when the character is killed, or the object is destroyed
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnDeath,				AActor*, KillingActor);

// Called whenever the actor starts or stops sprinting
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnSprint,				bool,  IsSprinting);

// Called whenever the actor takes damage, whether it kills them or not.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FOnDamageTaken,			AActor*, DamageTaker, AActor*, DamageInstigator, float, DamageTaken);

// Called whenever the actor is healed, whether dead or not
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnHealed,				AActor*, HealthInstigator, float, HealthRecovered);

// Called whenever the actor has a beneficial effect applied or removed
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnEffectModified,		int, UniqueId, bool, EffectActive);

// Called whenever any stat changes
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FOnStatModified,		EVitalityCategories, VitalityStat,
	float, OldValue,		float, NewValue);

// Called whenever a damage bonus has changed
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnDamageBonusModified,	EDamageType, DamageBonusType, int, NewTotal);

// Called whenever a damage resistance has changed
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnResistanceModified,	EDamageType, ResistanceType, int, NewTotal);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealthUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStaminaUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMagicUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHydrationUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCaloriesUpdated);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class VITALITYMATTERS_API UVitalityComponent : public UActorComponent
{
	GENERATED_BODY()

public: // public functions
	
	UVitalityComponent();
	virtual void OnComponentCreated() override;
	
	// Initializes the system, based on whether it's a character (true)
	// or an inanimate object (false).
	void InitSubsystems(bool isCharacter = true);

	// SERVER - Called whenever a stat's value is changed
	UPROPERTY(BlueprintAssignable, Category = "Vitality Events") FOnStatModified OnStatModified;

	// SERVER - Called whenever damage is taken
	UPROPERTY(BlueprintAssignable, Category = "Vitality Events") FOnDamageTaken OnDamageTaken;

	// SERVER - Called whenever health is replenished by another Actor
	UPROPERTY(BlueprintAssignable, Category = "Vitality Events") FOnHealed OnHealed;

	// SERVER - Called whenever the player dies
	UPROPERTY(BlueprintAssignable, Category = "Vitality Events") FOnDeath OnDeath;

	// SERVER - Called whenever sprinting is toggled
	UPROPERTY(BlueprintAssignable, Category = "Vitality Events") FOnSprint OnSprint;

	// SERVER - Called whenever an effect is added or removed.
	// CLIENT ONLY - Always called with Unique ID zero when the effects array is replicated
	UPROPERTY(BlueprintAssignable, Category = "Vitality Events") FOnEffectModified OnEffectModified;
	
	UPROPERTY(BlueprintAssignable, Category = "Vitality Events") FOnDamageBonusModified OnDamageBonusModified;

	UPROPERTY(BlueprintAssignable, Category = "Vitality Events") FOnResistanceModified OnResistanceModified;

	UPROPERTY(BlueprintAssignable, Category = "Vitality Events") FOnCombatStateChanged OnCombatStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "Vitality Events") FOnHealthUpdated		OnHealthUpdated;
	UPROPERTY(BlueprintAssignable, Category = "Vitality Events") FOnStaminaUpdated		OnStaminaUpdated;
	UPROPERTY(BlueprintAssignable, Category = "Vitality Events") FOnMagicUpdated		OnMagicUpdated;
	UPROPERTY(BlueprintAssignable, Category = "Vitality Events") FOnHydrationUpdated	OnHydrationUpdated;
	UPROPERTY(BlueprintAssignable, Category = "Vitality Events") FOnCaloriesUpdated		OnCaloriesUpdated;
	
	UFUNCTION(BlueprintCallable) float DamageHealth(AActor* DamageActor = nullptr, float DamageTaken = 0.f);
	UFUNCTION(BlueprintCallable) float ConsumeStamina(AActor* DamageActor = nullptr, float DamageTaken = 0.f);
	UFUNCTION(BlueprintCallable) float ConsumeMagic(AActor* DamageActor = nullptr, float DamageTaken = 0.f);
	
	/** Client or Server\n Checks if mIsSprinting is true (sprint mechanic on)
	 * @return True if sprinting is active, false if it is not.
	 */
	UFUNCTION(BlueprintPure)
	bool IsSprinting() const { return mIsSprinting; }

	UFUNCTION(BlueprintPure) int GetStrength() const { return _BaseStats.Strength; } // Quick Accessor
	UFUNCTION(BlueprintPure) int GetAgility() const { return _BaseStats.Agility; } // Quick Accessor
	UFUNCTION(BlueprintPure) int GetFortitude() const { return _BaseStats.Fortitude; } // Quick Accessor
	UFUNCTION(BlueprintPure) int GetIntellect() const { return _BaseStats.Intellect; } // Quick Accessor
	UFUNCTION(BlueprintPure) int GetAstuteness() const { return _BaseStats.Astuteness; } // Quick Accessor
	UFUNCTION(BlueprintPure) int GetCharisma() const { return _BaseStats.Charisma; } // Quick Accessor

	UFUNCTION(BlueprintCallable) void SetTotalResistanceValue(EDamageType DamageEnum, int NewValue = 0);
	UFUNCTION(BlueprintCallable) void AddNaturalResistance(EDamageType DamageEnum, int AddValue = 0);
	UFUNCTION(BlueprintCallable) void RemoveNaturalResistance(EDamageType DamageEnum, int RemoveValue = 0);
	UFUNCTION(BlueprintCallable) void AddGearResistance(EDamageType DamageEnum, int AddValue = 0);
	UFUNCTION(BlueprintCallable) void RemoveGearResistance(EDamageType DamageEnum, int RemoveValue = 0);
	UFUNCTION(BlueprintCallable) int CalculateAffectDamageResists() const;
	UFUNCTION(BlueprintPure) int GetResistance(EDamageType DamageEnum = EDamageType::ADMIN) const;

	UFUNCTION(BlueprintCallable) void SetTotalDamageBonus(EDamageType DamageEnum, int NewValue = 0);
	UFUNCTION(BlueprintCallable) void AddNaturalDamageBonus(EDamageType DamageEnum, int AddValue = 0);
	UFUNCTION(BlueprintCallable) void RemoveNaturalDamageBonus(EDamageType DamageEnum, int RemoveValue = 0);
	UFUNCTION(BlueprintCallable) void AddGearDamageBonus(EDamageType DamageEnum, int AddValue = 0);
	UFUNCTION(BlueprintCallable) void RemoveGearDamageBonus(EDamageType DamageEnum, int RemoveValue = 0);
	UFUNCTION(BlueprintCallable) int CalculateAffectDamageBonuses() const;
	UFUNCTION(BlueprintPure) int GetDamageBonus(EDamageType DamageEnum = EDamageType::ADMIN) const;
	
	UFUNCTION(BlueprintPure) float GetVitalityStat(EVitalityCategories VitalityStat,
			float &StatValue, float &StatMax) const;
	
	float GetVitalityStat(EVitalityCategories VitalityStat);
	UFUNCTION(BlueprintCallable) float SetVitalityStat(EVitalityCategories VitalityStat, float NewValue = 100.f);

	UFUNCTION(BlueprintCallable) float ModifyVitalityStat(EVitalityCategories VitalityStat, float AddValue = 0.f);
	UFUNCTION(BlueprintCallable) bool RemoveEffectByUniqueId(int UniqueId = 0);
	UFUNCTION(BlueprintCallable) bool ApplyEffect(FName EffectName, int StackCount = 1);
	bool RemoveEffect(FName EffectName, int RemoveCount);
	UFUNCTION(BlueprintCallable) bool ApplyEffectBeneficial(EEffectsBeneficial EffectBeneficial, int StackCount = 1);
	UFUNCTION(BlueprintCallable) bool RevokeEffectBeneficial(EEffectsBeneficial EffectBeneficial, int StackCount = 1);
	UFUNCTION(BlueprintCallable) bool ApplyEffectDetrimental(EEffectsDetrimental EffectDetrimental, int StackCount = 1);
	UFUNCTION(BlueprintCallable) bool RevokeEffectDetrimental(EEffectsDetrimental EffectDetrimental, int StackCount = 1);
	UFUNCTION(BlueprintCallable) bool RemoveEffectAtIndex(int IndexNumber = 0);
	UFUNCTION(BlueprintCallable) void ToggleSprint(bool DoSprint = false);
	UFUNCTION(BlueprintCallable) int GetNumActiveBenefit(EEffectsBeneficial BenefitEffect);
	UFUNCTION(BlueprintCallable) int GetNumActiveDetriment(EEffectsDetrimental DetrimentEffect);

	/** Returns a TArray of all active effects, both beneficial and detrimental.
	 * @return All active benefits at time of request.
	 */
	UFUNCTION(BlueprintPure) TArray<FStVitalityEffects> GetAllActiveEffects() { return mCurrentEffects; }
	UFUNCTION(BlueprintPure) FStVitalityEffects GetEffectByUniqueId(int UniqueId);
	UFUNCTION(BlueprintCallable) TArray<FStVitalityEffects> GetAllEffectsByDetriment(EEffectsDetrimental DetrimentEffect);
	UFUNCTION(BlueprintCallable) TArray<FStVitalityEffects> GetAllEffectsByBenefit(EEffectsBeneficial BenefitEffect);
	UFUNCTION(BlueprintPure)
	bool IsEffectActive(FName EffectName);
	bool IsEffectActive(EEffectsBeneficial EffectEnum);
	bool IsEffectActive(EEffectsDetrimental EffectEnum);

	UFUNCTION(BlueprintCallable) void SetCombatState(ECombatState CombatState);
	UFUNCTION(BlueprintCallable) ECombatState GetCombatState() const { return _CombatState; }	
protected: // protected functions

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Handles stamina decrease, stamina regen and sprinting logic
	virtual void TickStamina();

	// Handles health regeneration
	virtual void TickHealth();

	// Handles passive consumption of Calories & hunger effect
	virtual void TickCalories();

	// Handles passive consumption of Hydration & thirst effect
	virtual void TickHydration();

	// Handles effects wearing off
	virtual void TickEffects();

private: // private functions
	
	// Manages Tick Timers
	void TickManager();

	// Disables the sprinting mechanic and allows stamina regeneration
	void StopSprinting();

	// Enables the sprinting mechanic. Assumes all conditions are met.
	void StartSprinting();

	// Called when the stamina cooldown period has elapsed and stamina can regen
	void EndStaminaCooldown();

	// Helper function in the case ToggleSprint() is called on the client
	UFUNCTION(Server, Reliable) void Server_ToggleSprint(bool DoSprint = false);

	UFUNCTION() void DowngradeCombatState();
	
	int GenerateUniqueId();

	UFUNCTION(Client, Reliable)
	void OnRep_CurrentEffects();

	void InitializeTimer(FTimerHandle& timerHandle, FTimerDelegate timerDelegate);

	void CancelTimer(FTimerHandle& timerHandle);

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
	
public: // public members

	// The tick rate of the vitality timers, in seconds. "0.25" means 4 times as fast, while "4" means 4x slower.
	// Only set during initialization.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Settings")
	float VitalityTickRate = 0.25;

	// Sprint Speed Modifier, as a percentage of base max speed
	float SprintSpeedMultiplier = 1.2;
	
	// True if the player SHOULD regen stamina when thirsty
	// Can be changed during play on the server only
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Settings")
	bool StaminaRegenIfThirsty = false;
	
	// The number of time, in seconds, when cooldown starts after becoming tired
	// Only set during initialization.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Settings")
	float StaminaCooldown = 3.f;

	// The maximum value of stamina this actor should have
	// Only set during initialization.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Settings")
	float StaminaMaximum = 100.f;

	// The stamina regeneration rate (per tick) when the actor is not sprinting
	// Only set during initialization.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Settings")
	float StaminaRegenRate = 2.f;

	// The stamina drain rate (per tick) when the actor is sprinting
	// Only set during initialization.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Settings")
	float StaminaDrainRate = 1.f;

	// True if the player SHOULD regen health when engaged in combat
	// Can be changed during play on the server only
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Settings")
	float HealthRegenIfAlert = false;

	// The health regeneration rate (per tick) when the actor is not alert
	// Only set during initialization.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Settings")
	float HealthRegenRate = 0.5;
	
	// The amount of health this actor has when full
	// Only set during initialization.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Settings")
	float HealthMaximum = 100.f;
	
	// The amount of magic this actor has when full
	// Only set during initialization.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Settings")
	float MagicMaximum = 100.f;
	
	// The amount of magic this actor has when full
	// Only set during initialization.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Settings")
	float CaloriesMaximum = 500.f;
	
	// The amount of calories consumed when at rest (not performing any action)
	// Only set during initialization.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Settings")
	float CaloriesAtRest = 0.002;
	
	// The amount of magic this actor has when full
	// Only set during initialization.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Settings")
	float HydrationMaximum = 500.f;
	
	// The amount of calories consumed when at rest (not performing any action)
	// Only set during initialization.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Settings")
	float HungerAtRest = 0.082;
	

private: // private members

	// Mutex Lock for thread safe operation
	FRWLock mMutexLock;

	UPROPERTY(Replicated, ReplicatedUsing=OnRep_CurrentEffects)
	TArray<FStVitalityEffects> mCurrentEffects;
	
	TArray<FStVitalityEffects> mEffectsAddQueue;
	TArray<int> mEffectsRemoveQueue;
	
	UPROPERTY() FTimerHandle mStaminaTimer;		// Handles ticking of stamina. Handled internally.
	UPROPERTY() FTimerHandle mHealthTimer;
	UPROPERTY() FTimerHandle mEffectsTimer;
	UPROPERTY() FTimerHandle mCaloriesTimer;
	UPROPERTY() FTimerHandle mHydrationTimer;
	UPROPERTY() FTimerHandle mCombatStateTimer;
	
	// Handles stamina cooldown before regen can occur
	UPROPERTY() FTimerHandle mStaminaCooldownTimer;

	// General Members
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_CombatState)
	ECombatState _CombatState = ECombatState::RELAXED;
	UFUNCTION(NetMulticast, Reliable) void OnRep_CombatState(ECombatState OldCombatState);
	
	// Stamina Subsystem
	UFUNCTION(Client, Unreliable) void OnRep_StaminaValue();
	UPROPERTY(Replicated) bool mIsSprinting = false;	// True if actively sprinting
	float mStaminaDrain = 1.f;							// Drain Rate when Sprinting
	float mStaminaRegen = 1.f;							// Regen Rate
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_StaminaValue) float mStaminaValue = 1.f;	// Current Stamina
	UPROPERTY(Replicated) float mStaminaMax   = 1.f;    // Maximum Stamina
	float mSprintSpeed  = 1.2; // Percentage of base speed increase when sprinting
	bool mCanSprint		= true;

	// Health Subsystem
	UFUNCTION(Client, Unreliable) void OnRep_HealthValue();
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_HealthValue) float mHealthValue = 1.f;		// Current Health
	UPROPERTY(Replicated) float mHealthMax   = 1.f;		// Maximum Health
	float mHealthRegen = 1.f;							// Passive health regen rate

	// Nutrition Subsystem
	UFUNCTION(Client, Unreliable) void OnRep_CaloriesValue();
	UFUNCTION(Client, Unreliable) void OnRep_HydrationValue();
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_CaloriesValue) float mCaloriesValue  = 1.f;		// Current Calories
	UPROPERTY(Replicated) float mCaloriesMax    = 500.f;	// Maximum Hydration
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_HydrationValue) float mHydrationValue = 1.f;		// Current Hydration
	UPROPERTY(Replicated) float mHydrationMax   = 500.f;	// Maximum Hydration
	float mCaloriesDrainRest   = 0.082;						// Rate calories are lost at rest
	float mHydrationDrainRest  = 0.082;						// Rate of dehydration at rest
	
	// Magic Subsystem
	UFUNCTION(Client, Unreliable) void OnRep_MagicValue();
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_MagicValue) float mMagicValue		= 1.f;	// Current Magic/mana
	UPROPERTY(Replicated) float mMagicMax		= 1.f;  // Maximum Magic/mana

	// Natural stats of the character with progression
	UPROPERTY(Replicated) FStCharacterStats _BaseStats = FStCharacterStats();

	// Stats added by equipment in use
	UPROPERTY(Replicated) FStCharacterStats _GearStats = FStCharacterStats();

	// Stats added by affects currently active
	UPROPERTY(Replicated) FStCharacterStats _AffectStats = FStCharacterStats(); 
	
};
