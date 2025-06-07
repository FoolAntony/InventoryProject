﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inv_ItemComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class INVENTORY_API UInv_ItemComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	//====================
	//	FUNCTIONS
	//====================
	
	UInv_ItemComponent();
	FString GetPickupMessage() const {return PickupMessage;}

protected:

private:
	//=========================
	//	PARAMETERS & VARIABLES
	//=========================
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FString PickupMessage;

};
