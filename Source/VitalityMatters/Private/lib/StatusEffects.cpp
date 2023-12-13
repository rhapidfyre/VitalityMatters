// Fill out your copyright notice in the Description page of Project Settings.


#include "lib/StatusEffects.h"


UDataTable* UVitalityEffect::GetVitalityEffectsTable()
{
	const FSoftObjectPath itemTable = FSoftObjectPath("/VitalityMatters/DataTables/DT_VitalityData.DT_VitalityData");
	UDataTable* dataTable = Cast<UDataTable>(itemTable.ResolveObject());
	if (IsValid(dataTable)) return dataTable;
	return Cast<UDataTable>(itemTable.TryLoad());
}

FStVitalityEffects UVitalityEffect::GetVitalityEffect(FName EffectName)
{
	UDataTable* vitalityData = GetVitalityEffectsTable();
	if (IsValid(vitalityData))
	{
		const FString errorCaught;
		FStVitalityEffects* vitalityPointer = vitalityData->FindRow<FStVitalityEffects>(EffectName, errorCaught);
		if (vitalityPointer != nullptr)
		{
			return *vitalityPointer;
		}
	}
	return FStVitalityEffects();
}

FStVitalityEffects UVitalityEffect::GetVitalityEffectByBenefit(EEffectsBeneficial EffectEnum)
{
	if (EffectEnum != EEffectsBeneficial::MAX)
	{
		const FString effectString = UEnum::GetValueAsString(EffectEnum);
		return GetVitalityEffect(*effectString);
	}
	return FStVitalityEffects();
}

FStVitalityEffects UVitalityEffect::GetVitalityEffectByDetriment(EEffectsDetrimental EffectEnum)
{
	if (EffectEnum != EEffectsDetrimental::MAX)
	{
		const FString effectString = UEnum::GetValueAsString(EffectEnum);
		return GetVitalityEffect(*effectString);
	}
	return FStVitalityEffects();
}

bool UVitalityEffect::GetIsVitalityEffectNameValid(const FName EffectName)
{
	return GetIsVitalityEffectValid( GetVitalityEffect(EffectName) );
}

bool UVitalityEffect::GetIsVitalityEffectValid(const FStVitalityEffects& VitalityEffect)
{
	return (
		   VitalityEffect.benefitEffect   != EEffectsBeneficial::MAX
		|| VitalityEffect.detrimentEffect != EEffectsDetrimental::MAX
		);
}