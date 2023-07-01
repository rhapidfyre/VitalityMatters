// Copyright Take Five Games, LLC 2023 - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "VitalityEnums.h"
#include "UObject/Object.h"
#include "VitalityData.generated.h"

USTRUCT(BlueprintType)
struct FStDamageIntMap
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EDamageType DamageEnum;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int MapValue;
};

/**
 * 
*/
USTRUCT(BlueprintType)
struct FStCharacterStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int Strength 	= 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int Agility  	= 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int Fortitude	= 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int Astuteness	= 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int Intellect	= 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int Charisma	= 100;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FStDamageIntMap> DamageBonuses;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FStDamageIntMap> DamageResists;
	
};

/**
 * The base damage type for all Adventure Zero related damage
 * Should be for inheritance only. Don't implement this at the production level.
 */
UCLASS()
class UDamageTypeBase : public UDamageType
{
	GENERATED_BODY()
	
public:
	
	UDamageTypeBase() {};
	
	UFUNCTION(BlueprintCallable)
	float GetDamageValue() const { return DamageValue; }
	
	UPROPERTY() float DamageValue = 1.0f;
	
	UPROPERTY() EDamageType DamageEnum	= EDamageType::ADMIN;
	
};


// Generic parent class for any type of uncategorized physical damage
UCLASS(BlueprintType, Blueprintable)
class UDamageTypePhysical : public UDamageTypeBase
{
	GENERATED_BODY()
public:
	UDamageTypePhysical() { DamageEnum = EDamageType::PHYSICAL; }
};

// Generic parent class for any type of uncategorized magical damage
UCLASS(BlueprintType, Blueprintable)
class UDamageTypeMagical : public UDamageTypeBase
{
	GENERATED_BODY()
public:
	UDamageTypeMagical() { DamageEnum = EDamageType::MAGIC; }
};

// Generic parent class for any type of uncategorized elemental damage
UCLASS(BlueprintType, Blueprintable)
class UDamageTypeElemental : public UDamageTypeBase
{
	GENERATED_BODY()
public:
	UDamageTypeElemental() { DamageEnum = EDamageType::ELEMENTAL; }
};

// Generic parent class for any type of uncategorized natural damage
UCLASS(BlueprintType, Blueprintable)
class UDamageTypeNatural : public UDamageTypeBase
{
	GENERATED_BODY()
public:
	UDamageTypeNatural() { DamageEnum = EDamageType::NATURE; }
};


// Damage dealt by an unknown, non-resistible source
UCLASS(BlueprintType, Blueprintable)
class UDamageTypeAdmin : public UDamageTypeBase
{
	GENERATED_BODY()
public:
	UDamageTypeAdmin() { DamageEnum = EDamageType::ADMIN; }
};

// Damage dealt by natural acid
UCLASS(BlueprintType, Blueprintable)
class UDamageTypeAcid : public UDamageTypeNatural
{
	GENERATED_BODY()
public:
	UDamageTypeAcid() { DamageEnum = EDamageType::ACID; }
};

// Damage dealt by natural cold
UCLASS(BlueprintType, Blueprintable)
class UDamageTypeCold : public UDamageTypeElemental
{
	GENERATED_BODY()
public:
	UDamageTypeCold() { DamageEnum = EDamageType::COLD; }
};

// Damage dealt by dark magic
UCLASS(BlueprintType, Blueprintable)
class UDamageTypeDark : public UDamageTypeMagical
{
	GENERATED_BODY()
public:
	UDamageTypeDark() { DamageEnum = EDamageType::DARK; }
};

// Damage dealt by natural heat
UCLASS(BlueprintType, Blueprintable)
class UDamageTypeHeat : public UDamageTypeElemental
{
	GENERATED_BODY()
public:
	UDamageTypeHeat()  { DamageEnum = EDamageType::HEAT; }
};

// Damage dealt by divine energy
UCLASS(BlueprintType, Blueprintable)
class UDamageTypeHoly : public UDamageTypeMagical
{
	GENERATED_BODY()
public:
	UDamageTypeHoly() { DamageEnum = EDamageType::HOLY; }
};

// Damage dealt by electrical shock
UCLASS(BlueprintType, Blueprintable)
class UDamageTypeShock : public UDamageTypeElemental
{
	GENERATED_BODY()
public:
	UDamageTypeShock() { DamageEnum = EDamageType::SHOCK; }
};

// Damage dealt by sharp edges
UCLASS(BlueprintType, Blueprintable)
class UDamageTypeSlash : public UDamageTypePhysical
{
	GENERATED_BODY()
public:
	UDamageTypeSlash() { DamageEnum = EDamageType::SLASH; }
};

// Damage dealt by rigid things
UCLASS(BlueprintType, Blueprintable)
class UDamageTypeBlunt : public UDamageTypePhysical
{
	GENERATED_BODY()
public:
	UDamageTypeBlunt() { DamageEnum = EDamageType::BLUNT; }
};

// Damage dealt by sonic blasts
UCLASS(BlueprintType, Blueprintable)
class UDamageTypeSonic : public UDamageTypeMagical
{
	GENERATED_BODY()
public:
	UDamageTypeSonic() { DamageEnum = EDamageType::SONIC; }
};

// Damage dealt by natural chemicals
UCLASS(BlueprintType, Blueprintable)
class UDamageTypeToxic : public UDamageTypeNatural
{
	GENERATED_BODY()
public:
	UDamageTypeToxic() { DamageEnum = EDamageType::TOXIC; }
};

// Damage dealt by pokey things
UCLASS(BlueprintType, Blueprintable)
class UDamageTypePierce : public UDamageTypePhysical
{
	GENERATED_BODY()
public:
	UDamageTypePierce() { DamageEnum = EDamageType::PIERCE; }
};