// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Inv_Highlightable.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UInv_Highlightable : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for highlightable objects;
 * Provides functions to dynamically change object's material and *Highlight* it when traced by Player
 */
class INVENTORY_API IInv_Highlightable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	//====================
	//	FUNCTIONS
	//====================

	//BlueprintNativeEvent allows to access functions as event in blueprints, and override them in classes inherited this interface
	UFUNCTION(BlueprintNativeEvent, Category = "Inventory")
	void Highlight();
	UFUNCTION(BlueprintNativeEvent, Category = "Inventory")
	void UnHighlight();
	
};
