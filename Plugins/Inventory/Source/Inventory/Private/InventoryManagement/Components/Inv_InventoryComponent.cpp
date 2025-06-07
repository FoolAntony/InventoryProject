// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryManagement/Components/Inv_InventoryComponent.h"
#include "Widgets/Inventory/InventoryBase/Inv_InventoryBase.h"


UInv_InventoryComponent::UInv_InventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInv_InventoryComponent::ToggleInventoryMenu()
{
	
}

void UInv_InventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	ConstructInventory();
}

void UInv_InventoryComponent::ConstructInventory()
{
	OwningController = Cast<APlayerController>(GetOwner());		//Get current controller owned by player
	checkf(OwningController.IsValid(), TEXT("Inventory Component should have Player Controller as Owner")) //If controller invalid, error drops
	if (!OwningController->IsLocalController()) return; // Exit function if found controller is not a local player's controller
														//(not controller owned by some other player)

	//Creating inventory widget with found Player Controller defined as InventoryMenuClass blueprint
	InventoryMenu = CreateWidget<UInv_InventoryBase>(OwningController.Get(), InventoryMenuClass);
	//Making it visible on player's screen
	InventoryMenu->AddToViewport();
	
}



