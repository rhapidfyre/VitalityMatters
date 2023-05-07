// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageActorBase.h"


// Sets default values
void ADamageActorBase::SetupDefaults()
{
	PrimaryActorTick.bCanEverTick = false;
}

ADamageActorBase::ADamageActorBase()
{
	SetupDefaults();
}
ADamageActorBase::ADamageActorBase(float InitialHeight, float FinalHeight, float DamageValue)
{
	SetupDefaults();
	WidgetHeightOrigin.Z = InitialHeight;
	WidgetHeightFinal.Z  = FinalHeight;
	DamageAmount		 = DamageValue;
}

void ADamageActorBase::SetDamage(float DamageValue)
{
	DamageAmount = DamageValue;
}

// Called when the game starts or when spawned
void ADamageActorBase::BeginPlay()
{
	Super::BeginPlay();

	// Verification
	if (DamageAmount == 0.f || !IsValid(GetOwner()))
	{
		UE_LOG(LogTemp, Error, TEXT("%s - Invalid Data. Destroying."), *GetName());
		Destroy();
	}
		
}

