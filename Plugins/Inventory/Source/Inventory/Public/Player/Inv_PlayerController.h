// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Inv_PlayerController.generated.h"

/**
 * 
 */

class UInputMappingContext;
class UInputAction;

UCLASS()
class INVENTORY_API AInv_PlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

private:
	//	FUNCTIONS

	void PrimaryInteract();
	virtual void SetupInputComponent() override;

	//	PARAMETERS & VARIABLES
	
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TObjectPtr<UInputAction> PrimaryInteractionAction;
};
