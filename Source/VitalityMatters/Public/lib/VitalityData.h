// Copyright Take Five Games, LLC 2023 - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "VitalityEnums.h"
#include "VitalityGlobals.h"
#include "UObject/Object.h"
#include "Delegates/Delegate.h"

#include "VitalityData.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCoreStatUpdated,
	const EVitalityStat,	CoreStat);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamageBonusUpdated,
	const EDamageType,		DamageEnum);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamageResistUpdated,
	const EDamageType,		DamageEnum);


USTRUCT(BlueprintType)
struct VITALITYMATTERS_API FStDamageData
{
	GENERATED_BODY()
	FStDamageData() {};
	FStDamageData(AActor* DamageActor, float DamageValue)
	{
		DamagingActor    = DamageActor;
		LastDamageValue  = DamageValue;
		TotalDamageDealt = DamageValue;
	}
	UPROPERTY(EditAnywhere, BlueprintReadWrite) AActor* DamagingActor  = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float LastDamageValue  = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float TotalDamageDealt = 0.f;
};

USTRUCT(BlueprintType)
struct VITALITYMATTERS_API FStVitalityStats
{
	GENERATED_BODY()
	
	FStVitalityStats()
	{
		const int nCoreStats = UVitalitySystem::GetNumberOfCoreStats();
		CoreStats.SetNum(nCoreStats, true);
		for (int i = 0; i < nCoreStats; i++)
			CoreStats[i] = 0.f;
		
		const int nDamageTypes = UVitalitySystem::GetNumberOfDamageTypes();
		DamageBonuses.SetNum(nDamageTypes, true);
		DamageResists.SetNum(nDamageTypes, true);
		for (int i = 0; i < nDamageTypes; i++)
		{
			DamageBonuses[i] = 0.f;
			DamageResists[i] = 0.f;
		}
	}

	// Allows safe mutation of CoreStats by using the Enum. Otherwise, access directly with CoreStats[i]
	void SetCoreStat(const EVitalityStat StatEnum, const int NewValue);
	// Allows safe mutation of DamageBonuses by using the Enum. Otherwise, access directly with DamageBonuses[i]
	void SetDamageBonus(const EDamageType DamageEnum, const int NewValue);
	// Allows safe mutation of DamageResists by using the Enum. Otherwise, access directly with DamageResists[i]
	void SetDamageResistance(const EDamageType DamageEnum, const int NewValue);
	
	// Allows safe access to CoreStats by using the Enum
	float GetCoreStatValue(const EVitalityStat StatEnum) const;
	// Allows safe access to DamageBonuses by using the Enum
	float GetDamageBonusValue(const EDamageType DamageEnum) const;
	// Allows safe access to DamageResists by using the Enum
	float GetDamageResistValue(const EDamageType DamageEnum) const;
	
	UPROPERTY(BlueprintAssignable) FOnCoreStatUpdated		OnCoreStatUpdated;
	UPROPERTY(BlueprintAssignable) FOnDamageBonusUpdated	OnDamageBonusUpdated;
	UPROPERTY(BlueprintAssignable) FOnDamageResistUpdated	OnDamageResistanceUpdated;

	// Adds or removes core stat points. Access directly to set to a specific value.
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<float> CoreStats = {};
	// Adds or removes damage bonus points. Access directly to set to a specific value.
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<float> DamageBonuses = {};
	// Adds or removes damage resistance points. Access directly to set to a specific value.
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<float> DamageResists = {};
	
};

/** Used for data tables, for things like character creation.
 * For direct access, use FStVitalityStats
 */
USTRUCT()
struct VITALITYMATTERS_API FStVitalityData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FStVitalityStats StatsGroup = {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<FName, int> VitalityEffects = {};
	
};

/**
 * The base damage type for all Adventure Zero related damage
 * Should be for inheritance only. Don't implement this at the production level.
 */
UCLASS()
class VITALITYMATTERS_API UDamageTypeBase : public UDamageType
{
	GENERATED_BODY()
	
public:
	
	UDamageTypeBase() {}
	
	UFUNCTION(BlueprintCallable)
	float GetDamageValue() const { return DamageValue; }
	
	UPROPERTY() float DamageValue = 1.0f;
	
	UPROPERTY() EDamageType DamageEnum	= EDamageType::ADMIN;
	
};


// Generic parent class for any type of uncategorized physical damage
UCLASS(BlueprintType, Blueprintable)
class VITALITYMATTERS_API UDamageTypePhysical : public UDamageTypeBase
{
	GENERATED_BODY()
public:
	UDamageTypePhysical() { DamageEnum = EDamageType::PHYSICAL; }
};

// Generic parent class for any type of uncategorized magical damage
UCLASS(BlueprintType, Blueprintable)
class VITALITYMATTERS_API UDamageTypeMagical : public UDamageTypeBase
{
	GENERATED_BODY()
public:
	UDamageTypeMagical() { DamageEnum = EDamageType::MAGIC; }
};

// Generic parent class for any type of uncategorized elemental damage
UCLASS(BlueprintType, Blueprintable)
class VITALITYMATTERS_API UDamageTypeElemental : public UDamageTypeBase
{
	GENERATED_BODY()
public:
	UDamageTypeElemental() { DamageEnum = EDamageType::ELEMENTAL; }
};

// Generic parent class for any type of uncategorized natural damage
UCLASS(BlueprintType, Blueprintable)
class VITALITYMATTERS_API UDamageTypeNatural : public UDamageTypeBase
{
	GENERATED_BODY()
public:
	UDamageTypeNatural() { DamageEnum = EDamageType::NATURE; }
};


// Damage dealt by an unknown, non-resistible source
UCLASS(BlueprintType, Blueprintable)
class VITALITYMATTERS_API UDamageTypeAdmin : public UDamageTypeBase
{
	GENERATED_BODY()
public:
	UDamageTypeAdmin() { DamageEnum = EDamageType::ADMIN; }
};

// Damage dealt by natural acid
UCLASS(BlueprintType, Blueprintable)
class VITALITYMATTERS_API UDamageTypeAcid : public UDamageTypeNatural
{
	GENERATED_BODY()
public:
	UDamageTypeAcid() { DamageEnum = EDamageType::ACID; }
};

// Damage dealt by natural cold
UCLASS(BlueprintType, Blueprintable)
class VITALITYMATTERS_API UDamageTypeCold : public UDamageTypeElemental
{
	GENERATED_BODY()
public:
	UDamageTypeCold() { DamageEnum = EDamageType::COLD; }
};

// Damage dealt by dark magic
UCLASS(BlueprintType, Blueprintable)
class VITALITYMATTERS_API UDamageTypeDark : public UDamageTypeMagical
{
	GENERATED_BODY()
public:
	UDamageTypeDark() { DamageEnum = EDamageType::DARK; }
};

// Damage dealt by natural heat
UCLASS(BlueprintType, Blueprintable)
class VITALITYMATTERS_API UDamageTypeHeat : public UDamageTypeElemental
{
	GENERATED_BODY()
public:
	UDamageTypeHeat()  { DamageEnum = EDamageType::HEAT; }
};

// Damage dealt by divine energy
UCLASS(BlueprintType, Blueprintable)
class VITALITYMATTERS_API UDamageTypeHoly : public UDamageTypeMagical
{
	GENERATED_BODY()
public:
	UDamageTypeHoly() { DamageEnum = EDamageType::HOLY; }
};

// Damage dealt by electrical shock
UCLASS(BlueprintType, Blueprintable)
class VITALITYMATTERS_API UDamageTypeShock : public UDamageTypeElemental
{
	GENERATED_BODY()
public:
	UDamageTypeShock() { DamageEnum = EDamageType::SHOCK; }
};

// Damage dealt by sharp edges
UCLASS(BlueprintType, Blueprintable)
class VITALITYMATTERS_API UDamageTypeSlash : public UDamageTypePhysical
{
	GENERATED_BODY()
public:
	UDamageTypeSlash() { DamageEnum = EDamageType::SLASH; }
};

// Damage dealt by rigid things
UCLASS(BlueprintType, Blueprintable)
class VITALITYMATTERS_API UDamageTypeBlunt : public UDamageTypePhysical
{
	GENERATED_BODY()
public:
	UDamageTypeBlunt() { DamageEnum = EDamageType::BLUNT; }
};

// Damage dealt by sonic blasts
UCLASS(BlueprintType, Blueprintable)
class VITALITYMATTERS_API UDamageTypeSonic : public UDamageTypeMagical
{
	GENERATED_BODY()
public:
	UDamageTypeSonic() { DamageEnum = EDamageType::SONIC; }
};

// Damage dealt by natural chemicals
UCLASS(BlueprintType, Blueprintable)
class VITALITYMATTERS_API UDamageTypeToxic : public UDamageTypeNatural
{
	GENERATED_BODY()
public:
	UDamageTypeToxic() { DamageEnum = EDamageType::TOXIC; }
};

// Damage dealt by pokey things
UCLASS(BlueprintType, Blueprintable)
class VITALITYMATTERS_API UDamageTypePierce : public UDamageTypePhysical
{
	GENERATED_BODY()
public:
	UDamageTypePierce() { DamageEnum = EDamageType::PIERCE; }
};