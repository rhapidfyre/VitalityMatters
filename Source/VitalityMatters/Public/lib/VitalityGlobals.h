
#pragma once

#include "CoreMinimal.h"
#include "VitalityEnums.h"

#include "VitalityGlobals.generated.h"

//fwd declarations
struct FStVitalityEffects;


UCLASS(Blueprintable)
class VITALITYMATTERS_API UVitalitySystem : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	static int GetNumberOfCoreStats() { return static_cast<int>(EVitalityStat::MAX); }
	static int GetNumberOfDamageTypes() { return static_cast<int>(EDamageType::MAX); }

	static int GetCoreStatAsInt(EVitalityStat vStat) { return static_cast<int>(vStat); }
	static EVitalityStat GetCoreStatFromInt(int idx) { return static_cast<EVitalityStat>(idx); }
	
	static int GetDamageTypeAsInt(EDamageType vStat) { return static_cast<int>(vStat); }
	static EDamageType GetDamageTypeFromInt(int idx) { return static_cast<EDamageType>(idx); }
};