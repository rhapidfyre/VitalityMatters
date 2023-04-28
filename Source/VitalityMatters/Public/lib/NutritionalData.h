
#pragma once

#include "CoreMinimal.h"
#include "StatusEffects.h"
#include "Engine/DataTable.h"

#include "NutritionalData.generated.h"

const FString NutritionDataTable = "/VitalityMatters/DataTables/DT_NutritionTable.DT_NutritionTable";

USTRUCT(Blueprintable, BlueprintType)
struct VITALITYMATTERS_API FStNutritionData : public FTableRowBase
{
	GENERATED_BODY()
	// The proper name of the item, when consumed, gives the following data
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName properName = FName();
	// The benefit to apply when the item is consumed
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EEffectsBeneficial addBenefit = EEffectsBeneficial::NONE;
	// The number of seconds to apply the benefit
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int benefitCount = 1;
	// The detriment effect to apply when the item is consumed 
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EEffectsDetrimental addDetriment = EEffectsDetrimental::NONE;
	// The number of seconds to apply the detriment
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int detrimentCount = 1;
	// Optional actor to spawn when this item is consumed
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSubclassOf<AActor> optionalSpawnActor;
};

UCLASS()
class VITALITYMATTERS_API UNutritionSystem : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure) static UDataTable* GetNutritionDataTable();
	UFUNCTION(BlueprintPure) static FStNutritionData GetNutritionData(FName rowName);
};