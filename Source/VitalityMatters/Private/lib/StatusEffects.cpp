// Fill out your copyright notice in the Description page of Project Settings.


#include "lib/StatusEffects.h"

UDataTable* UVitalitySystem::GetVitalityEffectsTable()
{
	const FSoftObjectPath itemTable = FSoftObjectPath("/VitalityMatters/DataTables/DT_VitalityData.DT_VitalityData");
	UDataTable* dataTable = Cast<UDataTable>(itemTable.ResolveObject());
	if (IsValid(dataTable)) return dataTable;
	return Cast<UDataTable>(itemTable.TryLoad());
}

FStVitalityEffects UVitalitySystem::GetVitalityEffect(FName EffectName)
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

FStVitalityEffects UVitalitySystem::GetVitalityEffectByBenefit(EEffectsBeneficial EffectEnum)
{
	if (EffectEnum != EEffectsBeneficial::NONE)
	{
		const FString effectString = UEnum::GetValueAsString(EffectEnum);
		return GetVitalityEffect(*effectString);
	}
	return FStVitalityEffects();
}

FStVitalityEffects UVitalitySystem::GetVitalityEffectByDetriment(EEffectsDetrimental EffectEnum)
{
	if (EffectEnum != EEffectsDetrimental::NONE)
	{
		const FString effectString = UEnum::GetValueAsString(EffectEnum);
		return GetVitalityEffect(*effectString);
	}
	return FStVitalityEffects();
}
