// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "lib/StatusEffects.h"
#include "lib/VitalityEnums.h"

#include "VitalityComponent.generated.h"

// Called when the character is killed, or the object is destroyed
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnDeath,				AActor*, KillingActor);

// Called whenever the actor starts or stops sprinting
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnSprint,				bool,  IsSprinting);

// Called whenever the actor takes damage, whether it kills them or not.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnDamageTaken,			AActor*, DamageInstigator, float, DamageTaken);

// Called whenever the actor is healed, whether dead or not
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnHealed,				AActor*, HealthInstigator, float, HealthRecovered);

// Called whenever the actor has a beneficial effect applied or removed
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnEffectBeneficial,	EEffectsBeneficial,  EffectEnum, bool, EffectActive);

// Called whenever the actor has a detrimental effect applied or removed
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnEffectDetrimental,	EEffectsDetrimental, EffectEnum, bool, EffectActive);

// Called whenever any stat changes
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FOnStatModified,		EVitalityCategories, VitalityStat,
							float, OldValue,		float, NewValue);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class VITALITYMATTERS_API UVitalityComponent : public UActorComponent
{
	GENERATED_BODY()

public: // public functions
	
	UVitalityComponent();

	FOnStatModified OnStatModified;
	FOnDamageTaken OnDamageTaken;
	FOnHealed OnHealed;
	FOnDeath OnDeath;
	FOnSprint OnSprint;
	FOnEffectBeneficial OnEffectBeneficial;
	FOnEffectDetrimental OnEffectDetrimental;

	/** Client or Server\n Checks if mIsSprinting is true (sprint mechanic on)
	 * @return True if sprinting is active, false if it is not.
	 */
	UFUNCTION(BlueprintPure)
	bool IsSprinting() const { return mIsSprinting; }

	/** Client or Server\n Gets the request vitality enum value.
	 * @param VitalityStat The stat to be retrieved
	 * @param AsPercentage If true, returns value as a percentage of maximum.
	 * @return The value of health, either as a float or a percentage.
	 */
	UFUNCTION(BlueprintPure)
	float GetVitalityStat(EVitalityCategories VitalityStat, bool AsPercentage = true);

	/** Server Only \n Sets the given stat to the given value.
	 * Does nothing if run on the client. Straight logic, no math.
	 * @param VitalityStat The enum to modify. Defaults to health.
	 * @param NewValue The new value of the stat. Defaults to 100.f
	 */
	UFUNCTION(BlueprintCallable)
	float SetVitalityStat(EVitalityCategories VitalityStat, float NewValue = 100.f);

	/** Server Only\n Modifies the current value of the given stat, adding or
	 * subtracting value to it, respective of the signed float given.
	 * @param VitalityStat The enum to modify. Defaults to health.
	 * @param AddValue The value to add/remove. Sign sensitive. Defaults to 0.f
	 * @return The new stat value (should be input value). Negative indicates error.
	 */
	UFUNCTION(BlueprintCallable)
	float ModifyVitalityStat(EVitalityCategories VitalityStat, float AddValue = 0.f);

	/** Server Only\n Adds the requested benefit enum.
	 * @param EffectBeneficial The num to apply/revoke.
	 * @param StackCount The number of times to apply the effect
	 * @return True if the effect was added. False on failure.
	 */
	UFUNCTION(BlueprintCallable)
	bool ApplyEffectBeneficial(EEffectsBeneficial EffectBeneficial, int StackCount = 1);

	/** Server Only\n Removes the requested benefit enum.
	 * @param EffectBeneficial The num to apply/revoke.
	 * @param StackCount The number of identical effects to remove
	 * @return True if the effect was removed. False on failure.
	 */
	UFUNCTION(BlueprintCallable)
	bool RevokeEffectBeneficial(EEffectsBeneficial EffectBeneficial, int StackCount = 1);

	/** Server Only\n Adds the requested detriment enum.
	 * @param EffectDetrimental The num to apply/revoke.
	 * @param StackCount The number of times to apply the effect
	 * @return True if the effect was added. False on failure.
	 */
	UFUNCTION(BlueprintCallable)
	bool ApplyEffectDetrimental(EEffectsDetrimental EffectDetrimental, int StackCount = 1);

	/** Server Only\n Removes the requested detriment enum.
	 * @param EffectDetrimental The num to apply/revoke.
	 * @param StackCount The number of identical effects to remove
	 * @return True if the effect was removed. False on failure.
	 */
	UFUNCTION(BlueprintCallable)
	bool RevokeEffectDetrimental(EEffectsDetrimental EffectDetrimental, int StackCount = 1);

	/** Server Only\n Removes the effect at the given index.
	 * @param IndexNumber The index number of the current effect to remove
	 * @return True if the effect was removed. False on failure.
	 */
	UFUNCTION(BlueprintCallable)
	bool RevokeEffect(int IndexNumber = 0);
	
	/** Server or Client\n Starts or Stops the Sprinting Mechanic.
	 * @param DoSprint If true, attempts to start sprinting. False stops.
	 */
	UFUNCTION(BlueprintCallable)
	void ToggleSprint(bool DoSprint = false);

	FStVitalityEffects GetVitalityEffect(FName EffectName);

	FStVitalityEffects GetVitalityEffect(EEffectsBeneficial EffectEnum);

	FStVitalityEffects GetVitalityEffect(EEffectsDetrimental EffectEnum);
	
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

	// Called after InitSubsystems is complete, reloading saved values.
	virtual void ReloadFromSaveFile();

private: // private functions
	
	// Manages Tick Timers
	void TickManager();
	
	// Initializes the system, based on whether it's a character (true)
	// or an inanimate object (false).
	void InitSubsystems(bool isCharacter = true);

	// Disables the sprinting mechanic and allows stamina regeneration
	void StopSprinting();

	// Enables the sprinting mechanic. Assumes all conditions are met.
	void StartSprinting();

	// Called when the stamina cooldown period has elapsed and stamina can regen
	void EndStaminaCooldown();

	// Helper function in the case ToggleSprint() is called on the client
	void Server_ToggleSprint(bool DoSprint = false) { ToggleSprint(DoSprint); }
	
public: // public members

	// The tick rate of the vitality timers, in seconds.
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
	float CaloriesMaximum = 100.f;
	
	// The amount of calories consumed when at rest (not performing any action)
	// Only set during initialization.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Settings")
	float CaloriesAtRest = 0.002;
	
	// The amount of magic this actor has when full
	// Only set during initialization.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Settings")
	float HydrationMaximum = 100.f;
	
	// The amount of calories consumed when at rest (not performing any action)
	// Only set during initialization.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Settings")
	float HungerAtRest = 0.002;
	

private: // private members

	UPROPERTY(Replicated) TArray<FStVitalityEffects> mCurrentEffects;
	// Handles ticking of relevant ticks (effects, stamina, etc)
	UPROPERTY() FTimerHandle mTickTimer;
	UPROPERTY() FTimerHandle mStaminaTimer;
	// Handles stamina cooldown before regen can occur
	UPROPERTY() FTimerHandle mStaminaCooldownTimer;

	// Mutex Locks
	bool mEffectsMutex = false;
	
	// Stamina Subsystem
	UPROPERTY(Replicated) bool mIsSprinting = false;	// True if actively sprinting
	float mStaminaDrain = 1.f;							// Drain Rate when Sprinting
	float mStaminaRegen = 1.f;							// Regen Rate
	UPROPERTY(Replicated) float mStaminaValue = 100.f;	// Current Stamina
	UPROPERTY(Replicated) float mStaminaMax   = 100.f;  // Maximum Stamina
	float mSprintSpeed = 1.2; // Percentage of base speed increase when sprinting
	bool mCanSprint		= false;

	// Health Subsystem
	UPROPERTY(Replicated) float mHealthValue = 100.f;	// Current Health
	UPROPERTY(Replicated) float mHealthMax   = 100.f;   // Maximum Health
	float mHealthRegen = 1.f;							// Passive health regen rate

	// Nutrition Subsystem
	UPROPERTY(Replicated) float mCaloriesValue  = 1.f;	// Current Calories
	UPROPERTY(Replicated) float mCaloriesMax    = 1.f;  // Maximum Hydration
	UPROPERTY(Replicated) float mHydrationValue = 1.f;	// Current Hydration
	UPROPERTY(Replicated) float mHydrationMax   = 1.f;  // Maximum Hydration
	float mCaloriesDrainRest   = 0.002;					// Rate calories are lost at rest
	float mHydrationDrainRest  = 0.002;					// Rate of dehydration at rest
	
	// Magic Subsystem
	UPROPERTY(Replicated) float mMagicValue		= 1.f;	// Current Magic/mana
	UPROPERTY(Replicated) float mMagicMax		= 1.f;  // Maximum Magic/mana
	
};
