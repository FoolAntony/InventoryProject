// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inv_InventoryComponent.generated.h"

/*
 *  Inventory object component
 *  Requires inventory widget to create in a viewport
 */

class UInv_InventoryBase;
class UInv_InventoryItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryItemChange, UInv_InventoryItem*, Item);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class INVENTORY_API UInv_InventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	//====================
	//	FUNCTIONS
	//====================
	
	UInv_InventoryComponent();

	void ToggleInventoryMenu();

	//=========================
	//	PARAMETERS & VARIABLES
	//=========================

	FInventoryItemChange OnItemAdded;
	FInventoryItemChange OnItemRemoved;

protected:
	//====================
	//	FUNCTIONS
	//====================
	
	virtual void BeginPlay() override;

private:
	//====================
	//	FUNCTIONS
	//====================
	
	void ConstructInventory();
	void OpenInventoryMenu();
	void CloseInventoryMenu();

	//=========================
	//	PARAMETERS & VARIABLES
	//=========================
	
	/*Stores currently owning PlayerController*/
	TWeakObjectPtr<APlayerController> OwningController;
	
	UPROPERTY()
	TObjectPtr<UInv_InventoryBase> InventoryMenu;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UInv_InventoryBase> InventoryMenuClass;

	bool bInventoryMenuOpen;
	
};
