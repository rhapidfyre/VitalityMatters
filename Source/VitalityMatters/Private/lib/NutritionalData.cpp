
#include "lib/NutritionalData.h"

UDataTable* UNutritionSystem::GetNutritionDataTable()
{
	const FSoftObjectPath itemTable = FSoftObjectPath(NutritionDataTable);
	UDataTable* dataTable = Cast<UDataTable>(itemTable.ResolveObject());
	if (IsValid(dataTable)) return dataTable;
	return Cast<UDataTable>(itemTable.TryLoad());
}

FStNutritionData UNutritionSystem::GetNutritionData(FName rowName)
{
	const UDataTable* nutritionData = GetNutritionDataTable();
	if (IsValid(nutritionData))
	{
		const FString errorCaught;
		FStNutritionData* vitalityPointer = nutritionData->FindRow<FStNutritionData>(rowName, errorCaught);
		if (vitalityPointer != nullptr)
		{
			return *vitalityPointer;
		}
		UE_LOG(LogTemp, Error, TEXT("GetNutritionData(): %s"), *errorCaught);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GetNutritionData: Unable to find DataTable '%s'"), *NutritionDataTable);
	}
	return FStNutritionData();
}