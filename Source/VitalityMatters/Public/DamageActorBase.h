
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/WidgetComponent.h"

#include "DamageActorBase.generated.h"

/* Spawns an actor that manages damage dealt, such as floating text widgets
 * and more. Very useful for debugging. Used by the floating damage numbers
 * component.
 */

UCLASS(BlueprintType, Blueprintable)
class VITALITYMATTERS_API ADamageActorBase : public AActor
{
	GENERATED_BODY()

public:

	ADamageActorBase();
	ADamageActorBase(float InitialHeight, float FinalHeight, float DamageValue);

	// For C++, use this on a SpawnActorDeferral call before spawning.
	// Failure to set this BEFORE spawning will result in actor being destroyed!
	void SetDamage(float DamageValue = 0.f);

protected:

	virtual void BeginPlay() override;
private:
	void SetupDefaults();
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn=true))
	FVector WidgetHeightFinal  = FVector(0.f,0.f,90.f);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn=true))
	FVector WidgetHeightOrigin = FVector(0.f,0.f,60.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn=true))
	float DamageAmount         = 0.f;
	
};
