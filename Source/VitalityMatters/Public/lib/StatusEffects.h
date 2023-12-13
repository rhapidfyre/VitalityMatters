// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VitalityEnums.h"
#include "Engine/DataTable.h"

#include "StatusEffects.generated.h"

USTRUCT(BlueprintType)
struct FStVitalityEffects : public FTableRowBase
{
	GENERATED_BODY()
	FStVitalityEffects(){}

	FStVitalityEffects(FName fName) { EffectName = fName; }
	FName EffectName = FName();
	
	// The effect to apply. Overrides detrimentEffect
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EEffectsBeneficial benefitEffect = EEffectsBeneficial::MAX;
	// The effect to apply. Overriden if benefitEffect is not NONE
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EEffectsDetrimental detrimentEffect = EEffectsDetrimental::MAX;
	// The display name of the effect (such as hover text)
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString effectTitle = "Effect";
	// The icon to show when this effect is applied
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UTexture2D* effectIcon = nullptr;
	// This effect is permanent until death, destruction or manual removal.
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bIsPersistent = false;
	// This effect stacks, multiplying the effect if it is applied multiple times.
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bEffectStacks = false;
	// The maximum ticks the effect can last (where ticks = tickRate of the Vitality Tick Rate)
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int effectTicks = 0.f;
	// If true, the optional actor spawned will attach to the Actor owning the vitality component
	// Does nothing if optionalClass is unused.
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool attachOnSpawn = true;
	// Optional actor class to spawn. Useful for sounds, effects, etc.
	// Leave as None/Empty/Null if not desired
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSubclassOf<AActor> optionalClass;
	// If true, disallows sprinting while this effect is active
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool disableSprinting = false;
	// Used internally. Setting this value does nothing.
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int uniqueId = 0;
	
};

UCLASS(Blueprintable, BlueprintType)
class UVitalityEffect : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	static UDataTable* GetVitalityEffectsTable();
	static FStVitalityEffects GetVitalityEffect(FName EffectName);
	static FStVitalityEffects GetVitalityEffectByBenefit(EEffectsBeneficial EffectEnum);
	static FStVitalityEffects GetVitalityEffectByDetriment(EEffectsDetrimental EffectEnum);
	
	UFUNCTION(BlueprintPure) static bool GetIsVitalityEffectNameValid(const FName EffectName);
	UFUNCTION(BlueprintPure) static bool GetIsVitalityEffectValid(const FStVitalityEffects& VitalityEffect);
};