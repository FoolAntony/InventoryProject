// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Inv_PlayerController.generated.h"

/**
 *  Inventory Plugin PlayerController functionality;
 *  When implemented, enables object tracing and Enhanced Input System for interaction
 */

class UInv_InventoryComponent;
class UInputMappingContext;
class UInputAction;
class UInv_HUDWidget;

UCLASS()
class INVENTORY_API AInv_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	//====================
	//	FUNCTIONS
	//====================
	
	AInv_PlayerController();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ToggleInventory();

protected:
	//====================
	//	FUNCTIONS
	//====================
	
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	//====================
	//	FUNCTIONS
	//====================

	/*Interaction function primarily used by class*/
	void PrimaryInteract();
	
	/*Creates player's HUD widget object*/
	void CreateHUDWidget();
	
	void TraceForItem();

	//=========================
	//	PARAMETERS & VARIABLES
	//=========================
	
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TObjectPtr<UInputMappingContext> DefaultMappingContext; // Function stores default mapping context blueprint used by PlayerController

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TObjectPtr<UInputAction> PrimaryInteractionAction;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TObjectPtr<UInputAction> ToggleInventoryAction; //Stores action for inventory toggling (on/off)

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UInv_HUDWidget> HUDWidgetClass;

	UPROPERTY()
	TObjectPtr<UInv_HUDWidget> HUDWidget;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	double TraceLength; // Path from trace start point (screen center) to endpoint

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TEnumAsByte<ECollisionChannel> ItemTraceChannel; //Collision channel used for item trace;

	// Creating weak pointers so it could be garbage collected with no effect on runtime
	TWeakObjectPtr<AActor> ThisActor; // Currently traced object
	TWeakObjectPtr<AActor> LastActor; // Last traced object
	TWeakObjectPtr<UInv_InventoryComponent> InventoryComponent;
};
