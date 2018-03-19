// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "BatteryPickup.generated.h"

UCLASS()
class BATTERY3_API ABatteryPickup : public APickup
{
	GENERATED_BODY()

public:
	ABatteryPickup();
	void WasCollected_Implementation() override;
	float GetPower();
protected:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Power",Meta=(BlueprintProtected))
	float BatteryPower;
};
